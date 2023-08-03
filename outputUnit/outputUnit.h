#include "common.h"

SC_MODULE(outputUnit) {
    sc_in<bool> clk;
    sc_in<bool> rst;

    primate_stream_512_4::out         stream_out;

    primate_ctrl_ou::slave            cmd_in;

    primate_bfu_ou::master            bfu_out;

    primate_bfu_rdreq_ou::master      bfu_rdreq;
    primate_bfu_rdrsp_ou::slave       bfu_rdrsp;

    primate_stream_512_4::in          pkt_buf_in;

    bool hdr_done[NUM_THREADS];
    sc_uint<8> hdr_mode[NUM_THREADS];

    sc_uint<9> port;

    void outputUnit_cmd();
    void outputUnit_req();
    void outputUnit_rsp();

    SC_CTOR(outputUnit) {
        SC_CTHREAD(outputUnit_cmd, clk.pos());
        SC_CTHREAD(outputUnit_req, clk.pos());
        SC_CTHREAD(outputUnit_rsp, clk.pos());
        reset_signal_is(rst, true);  // true is hihg, flase is low
    };
};