#include <systemc.h>
#include "common.h"
#define NUM_PKT 2

SC_MODULE(tb) {
    sc_in<bool>                      clk;
    sc_out<bool>                     rst;

    primate_stream_512_4::in         stream_in;

    primate_ctrl_ou::master          cmd_out;

    primate_bfu_ou::slave            bfu_in;

    primate_bfu_rdreq_ou::slave      bfu_rdreq;
    primate_bfu_rdrsp_ou::master     bfu_rdrsp;

    primate_stream_512_4::out        pkt_buf_out;

    void regfile();
    void source();
    void sink();

    std::ofstream outfile;
    sc_time start_time[NUM_PKT], end_time[NUM_PKT], clock_period;

    SC_CTOR(tb){
        SC_CTHREAD(source, clk.pos());
        SC_CTHREAD(regfile, clk.pos());
        SC_CTHREAD(sink, clk.pos());
    };
};