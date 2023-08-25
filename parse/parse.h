#include "common.h"

SC_MODULE(parse) {
    sc_in<bool> i_clk;
    sc_in<bool> i_rst;

    primate_stream_512_4::in          stream_in;

    primate_stream_512_4::out         stream_out;

    sc_biguint<IO_W> input_buf;
    sc_uint<IO_BW_LG+1> input_buf_len;
    sc_uint<IO_BW_LG+1> fifo_empty;
    bool last_buf;

    std::vector<primate_stream_512_4::payload_t> pkt_buf;

    void Init() {
        input_buf_len = 0;
        fifo_empty = 0;
        last_buf = false;
        pkt_buf.clear();
        stream_in.reset();
        stream_out.reset();
        wait();
    }

    template<typename h_t>
    void Input_header(const int &length, h_t &header) {
        primate_stream_512_4::payload_t payload;
        bool res_valid = false;
        sc_uint<IO_BW_LG> shift;
        // int debug = 0;
        while (!res_valid) {
            shift = 0;
            if (length <= input_buf_len) {
                shift = length;
                res_valid = true;
            }
            bool stream_in_valid = stream_in.nb_read(payload);
            header.set(input_buf);
            // if (debug < 10) {
            //     cout << "valid: " << stream_in_valid << ", ";
            //     cout << "length: " << length << ", ";
            //     cout << "fifo_empty: " << fifo_empty << ", ";
            //     cout << "input_buf_len: " << input_buf_len << endl;
            //     cout << "input_buf: " << hex << input_buf << endl;
            //     cout << "fifo_data: " << payload.data << dec << endl;
            //     debug++;
            // }
            // shift and fill input buffer
            if (input_buf_len == 0) {
                input_buf = payload.data.range(IO_W-1, fifo_empty*8);
            } else {
                input_buf = (payload.data.range(IO_W-1, fifo_empty*8), input_buf.range(input_buf_len*8-1, shift*8));
            }
            if (stream_in_valid) {
                if (input_buf_len - shift <= fifo_empty) {
                    // fill input buffer and pop input
                    input_buf_len = input_buf_len - shift + IO_BW - fifo_empty;
                    last_buf = payload.last;
                    fifo_empty = 0;
                    stream_in.nb_pop();
                } else {
                    // fill input buffer
                    input_buf_len = IO_BW;
                    fifo_empty += shift;
                }
            } else {
                input_buf_len -= shift;
            }
            wait();
        }
    }

    void Input_done() {
        primate_stream_512_4::payload_t payload;
        payload.data = input_buf;
        payload.empty = IO_BW - input_buf_len + fifo_empty;
        payload.last = last_buf;
        pkt_buf.push_back(payload);
        input_buf_len = 0;
        fifo_empty = 0;
        if (!last_buf) {
            bool is_last = false;
            while (!is_last) {
                bool valid = false;
                while (!valid) {
                    valid = stream_in.nb_read(payload);
                    if (valid) {
                        stream_in.nb_pop();
                    }
                    wait();
                }
                is_last = payload.last;
                pkt_buf.push_back(payload);
            }
        }
    }

    template<typename h_t>
    void Output_header(const int &length, h_t &header) {
        primate_stream_512_4::payload_t payload;
        payload.data = header.to_uint();
        payload.empty = IO_BW - length;
        payload.last = false;
        stream_out.write(payload);
    }

    void Output_done() {
        primate_stream_512_4::payload_t payload;
        for (auto it = pkt_buf.begin(); it != pkt_buf.end(); it++) {
            stream_out.write(*it);
        }
        pkt_buf.clear();
    }

    void Output_meta(standard_metadata_t &standard_metadata) {
        cout << "port: " << standard_metadata.egress_spec << endl;
    } //outputMeta inst

    void parse_main();

    SC_CTOR(parse) {
        SC_CTHREAD(parse_main, i_clk.pos());
        reset_signal_is(i_rst, true);  // true is hihg, flase is low
    };
};
