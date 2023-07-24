#include "common.h"

SC_MODULE(inputUnit) {
    sc_in<bool> clk;
    sc_in<bool> rst;

    primate_stream_512_4::in          stream_in;

    primate_ctrl_iu::slave            cmd_in;

    primate_bfu_iu::master            bfu_out;

    primate_stream_512_4::out         pkt_buf_out;

    void inputUnit_main();

    SC_CTOR(inputUnit) {
        SC_CTHREAD(inputUnit_main, clk.pos());
        reset_signal_is(rst, true);  // true is hihg, flase is low
    };
};