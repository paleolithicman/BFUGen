#include <systemc.h>
#include "common.h"
#define NUM_PKT 2

SC_MODULE(tb) {
    sc_in<bool>                      clk;
    sc_out<bool>                     rst;

    primate_stream_512_4::out        stream_out;

    primate_ctrl_iu::master          cmd_out;

    primate_bfu_iu::slave            bfu_in;

    primate_stream_512_4::in         pkt_buf_in;

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