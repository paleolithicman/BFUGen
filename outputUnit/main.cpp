#include <systemc.h>
#include "outputUnit.h"
#include "tb.h"

SC_MODULE(SYSTEM) {
    // Module declarations
    tb *tb_inst;
    outputUnit *outputUnit_inst;

    // local signal declarations
    sc_signal<bool> rst_sig;
    sc_clock        clk_sig;

    primate_stream_512_4                dut_to_tb_data;
    primate_ctrl_ou                     tb_to_dut_ctrl;
    primate_bfu_ou                      dut_to_tb_ctrl;
    primate_stream_512_4                pkt_buf;
    primate_bfu_rdreq_ou::channel       bfu_rdreq;
    primate_bfu_rdrsp_ou::channel       bfu_rdrsp;

    SC_CTOR(SYSTEM) : clk_sig("clk_sig", 4, SC_NS) {
        tb_inst = new tb("tb_inst");
        tb_inst->clk(clk_sig);
        tb_inst->rst(rst_sig);
        tb_inst->stream_in(dut_to_tb_data);
        tb_inst->cmd_out(tb_to_dut_ctrl);
        tb_inst->bfu_in(dut_to_tb_ctrl);
        tb_inst->bfu_rdreq(bfu_rdreq);
        tb_inst->bfu_rdrsp(bfu_rdrsp);
        tb_inst->pkt_buf_out(pkt_buf);

        outputUnit_inst = new outputUnit("outputUnit_inst");
        outputUnit_inst->clk(clk_sig);
        outputUnit_inst->rst(rst_sig);
        outputUnit_inst->stream_out(dut_to_tb_data);
        outputUnit_inst->cmd_in(tb_to_dut_ctrl);
        outputUnit_inst->bfu_out(dut_to_tb_ctrl);
        outputUnit_inst->bfu_rdreq(bfu_rdreq);
        outputUnit_inst->bfu_rdrsp(bfu_rdrsp);
        outputUnit_inst->pkt_buf_in(pkt_buf);
    }

    ~SYSTEM() {
        delete tb_inst;
        delete outputUnit_inst;
    }
};

SYSTEM *top = NULL;
int sc_main(int argc, char* argv[])
{
    top = new SYSTEM("top");
    sc_start();
    return 0;
}