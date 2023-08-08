#include "common.h"

SC_MODULE(outputUnit) {
    sc_in<bool> i_clk;
    sc_in<bool> i_rst;

    primate_stream_512_4::out         stream_out;

    primate_ctrl_ou::slave            cmd_in;

    primate_bfu_ou::master            bfu_out;

    primate_bfu_rdreq_ou::master      bfu_rdreq;
    primate_bfu_rdrsp_ou::slave       bfu_rdrsp;

    primate_stream_512_4::in          pkt_buf_in;

    sc_signal<bool> done;
    sc_signal<sc_uint<NUM_THREADS_LG>> done_tag;
    sc_signal<sc_uint<1>> hdr_done[NUM_THREADS];
    sc_signal<sc_uint<8>> hdr_mode[NUM_THREADS];

    sc_uint<9> port;

    void outputUnit_cmd();
    void outputUnit_req();
    void outputUnit_rsp();

    SC_CTOR(outputUnit) {
        SC_CTHREAD(outputUnit_cmd, i_clk.pos());
        reset_signal_is(i_rst, true);  // true is hihg, flase is low
        SC_CTHREAD(outputUnit_req, i_clk.pos());
        reset_signal_is(i_rst, true);  // true is hihg, flase is low
        SC_CTHREAD(outputUnit_rsp, i_clk.pos());
        reset_signal_is(i_rst, true);  // true is hihg, flase is low
    };
};

