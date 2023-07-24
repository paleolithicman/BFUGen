#include <systemc.h>
#include "common.h"
#define NUM_PKT 2

SC_MODULE(tb) {
    sc_in<bool>                      clk;
    sc_out<bool>                     rst;

    primate_stream_512_4::out        stream_out;

    primate_ctrl_iu::master          cmd_out;

    sc_out<bool>                     out_ready;
    sc_in<bool>                      out_valid;
    sc_in<sc_uint<NUM_THREADS_LG>>   out_tag;
    sc_in<sc_uint<IP_WIDTH>>         out_flag;
    sc_vector<sc_in<bool>>                    out_wen{"out_wen", 2};
    sc_vector<sc_in<sc_uint<NUM_REGS_LG>>>    out_addr{"out_addr", 2};
    sc_vector<sc_in<sc_biguint<REG_WIDTH>>>   out_data{"out_data", 2};

    sc_in<bool>                            pkt_buf_valid;
    sc_in<primate_stream_512_4::payload_t> pkt_buf_data;
    sc_out<bool>                           pkt_buf_ready;

    void source();
    void sink();

    std::ifstream infile;
    std::ofstream outfile;
    sc_time start_time[NUM_PKT], end_time[NUM_PKT], clock_period;

    SC_CTOR(tb){
        SC_CTHREAD(source, clk.pos());
        SC_CTHREAD(sink, clk.pos());
    };
};