#include "inputUnit.h"

void inputUnit::inputUnit_main() {
    sc_biguint<512> in_data_buf;
    sc_biguint<512> pkt_data_buf;
    sc_uint<7> pkt_empty;
    bool last_buf;
    sc_uint<4> state;
    sc_biguint<4> hdr_count;
    sc_uint<NUM_THREADS_LG> tag;
    ethernet_t eth;
    ptp_l_t ptp_l;
    sc_biguint<192> ptp_h;
    header_t header_0;
    header_t header_1;
    header_t header_2;
    header_t header_3;
    header_t header_4;
    header_t header_5;
    header_t header_6;
    header_t header_7;
    primate_stream_512_4::payload_t payload;
    primate_ctrl_iu::cmd_t cmd;

    // initialize handshake
    stream_in.reset();
    cmd_in.reset();
    pkt_buf_out.reset();
    bfu_out.reset();
    state = 15;
    wait();

    // main FSM
    while (true) {
        cout << sc_time_stamp() << ": state " << state << endl;
        if (state == 15) {
            cmd = cmd_in.read();
            state = 0;
        } else if (state == 0) {
            payload = stream_in.read();
            
            cout << sc_time_stamp() << ": in_data " << payload << endl;
            in_data_buf = payload.data;
            last_buf = payload.last;
            tag = cmd.ar_tag;
            eth.set(payload.data.range(111, 0));
            ptp_l.set(payload.data.range(271, 112));
            if (eth.etherType == 0x88f7) {
                // write ethernet header and ptp header
                state = 1;
                bfu_out.write(cmd.ar_tag, 0, 1, eth.to_uint(), 2, ptp_l.to_uint());
            } else {
                // write ethernet header only
                state = 11;
                pkt_data_buf = (payload.data) >> 112;
                pkt_empty = 112 / 8;
                bfu_out.write(cmd.ar_tag, 0, 1, eth.to_uint(), 22, 1, payload.last);
            }
        } else if (state == 1) {
            ptp_h = in_data_buf.range(463, 272);
            if (ptp_l.reserved2 == 1) {
                payload = stream_in.read();

                cout << sc_time_stamp() << ": in_data " << payload << endl;
                sc_biguint<64> hdr_val = (payload.data.range(15, 0), in_data_buf.range(511, 464));
                header_0.set(hdr_val);
                in_data_buf = payload.data;
                last_buf = payload.last;
                if (header_0.field_0 == 0) {
                    state = 2;
                    bfu_out.write(tag, 0, 3, ptp_h, 4, header_0.to_uint());
                } else {
                    state = 3;
                    bfu_out.write(tag, 0, 3, ptp_h);
                }
            } else {
                pkt_data_buf = in_data_buf >> 464;
                pkt_empty = 464 / 8;
                state = 11;
                bfu_out.write(tag, 0, 3, ptp_h, 22, 2, last_buf);
            }
        } else if (state == 2) {
            pkt_data_buf = in_data_buf >> 16;
            pkt_empty = 16 / 8;
            state = 11;
            bfu_out.write(tag, 0, 22, 3, last_buf);
        } else if (state == 3) {
            header_1.set(in_data_buf.range(79, 16));
            header_2.set(in_data_buf.range(143, 80));
            header_3.set(in_data_buf.range(207, 144));
            cout << "in_data_buf: " << hex << in_data_buf << dec << endl;
            if (header_1.field_0 == 0) {
                pkt_data_buf = in_data_buf >> 80;
                pkt_empty = 80 / 8;
                state = 11;
                bfu_out.write(tag, 0, 22, 4, 4, cat_header(header_1, header_0), last_buf);
            } else if (header_2.field_0 == 0) {
                pkt_data_buf = in_data_buf >> 144;
                pkt_empty = 144 / 8;
                hdr_count = 5;
                state = 5;
                bfu_out.write(tag, 0, 5, cat_header(header_3, header_2), 4, cat_header(header_1, header_0));
            } else if (header_3.field_0 == 0) {
                pkt_data_buf = in_data_buf >> 208;
                pkt_empty = 208 / 8;
                hdr_count = 6;
                state = 5;
                bfu_out.write(tag, 0, 5, cat_header(header_3, header_2), 4, cat_header(header_1, header_0));
            } else {
                state = 4;
                bfu_out.write(tag, 0, 5, cat_header(header_3, header_2), 4, cat_header(header_1, header_0));
            }
        } else if (state == 4) {
            header_4.set(in_data_buf.range(271, 208));
            header_5.set(in_data_buf.range(335, 272));
            header_6.set(in_data_buf.range(399, 336));
            header_7.set(in_data_buf.range(463, 400));
            cout << "in_data_buf: " << hex << in_data_buf << dec << endl;
            if (header_4.field_0 == 0) {
                pkt_data_buf = in_data_buf >> 272;
                pkt_empty = 272 / 8;
                state = 11;
                bfu_out.write(tag, 0, 22, 7, 6, cat_header(header_5, header_4), last_buf);
            } else if (header_5.field_0 == 0) {
                pkt_data_buf = in_data_buf >> 336;
                pkt_empty = 336 / 8;
                state = 11;
                bfu_out.write(tag, 0, 22, 8, 6, cat_header(header_5, header_4), last_buf);
            } else if (header_6.field_0 == 0) {
                pkt_data_buf = in_data_buf >> 400;
                pkt_empty = 400 / 8;
                hdr_count = 9;
                state = 5;
                bfu_out.write(tag, 0, 7, cat_header(header_7, header_6), 6, cat_header(header_5, header_4));
            } else {
                pkt_data_buf = in_data_buf >> 464;
                pkt_empty = 464 / 8;
                hdr_count = 10;
                state = 5;
                bfu_out.write(tag, 0, 7, cat_header(header_7, header_6), 6, cat_header(header_5, header_4));
            }
        } else if (state == 5) {
            state = 11;
            bfu_out.write(tag, 0, 22, hdr_count, last_buf);
        } else if (state == 11) {
            primate_stream_512_4::payload_t pkt_buf(pkt_data_buf, tag, pkt_empty, last_buf);
            if (last_buf) {
                state = 0;
            } else {
                state = 12;
            }
            pkt_buf_out.write(pkt_buf);
        } else if (state == 12) {
            payload = stream_in.read();
            cout << sc_time_stamp() << ": in_data " << payload << endl;
            if (payload.last) {
                state = 13;
            }
            pkt_buf_out.write(payload);
        } else if (state == 13) {
            state = 15;
            bfu_out.write_last(tag, 0);
        }
    }


}