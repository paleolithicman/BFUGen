#include <systemc.h>
#include "inputUnit.h"
#include "tb.h"

SC_MODULE(SYSTEM) {
    // Module declarations
    tb *tb_inst;
    inputUnit *inputUnit_inst;

    // local signal declarations
    sc_signal<bool> rst_sig;
    sc_clock        clk_sig;

    primate_stream_512_4                tb_to_dut_data;

    primate_ctrl_iu                     tb_to_dut_ctrl;

    primate_bfu_iu                      dut_to_tb_data;

    primate_stream_512_4                pkt_buf;

    SC_CTOR(SYSTEM) : clk_sig("clk_sig", 4, SC_NS) {
        tb_inst = new tb("tb_inst");
        tb_inst->clk(clk_sig);
        tb_inst->rst(rst_sig);
        tb_inst->stream_out(tb_to_dut_data);
        tb_inst->cmd_out(tb_to_dut_ctrl);
        tb_inst->bfu_in(dut_to_tb_data);
        tb_inst->pkt_buf_in(pkt_buf);

        inputUnit_inst = new inputUnit("inputUnit_inst");
        inputUnit_inst->clk(clk_sig);
        inputUnit_inst->rst(rst_sig);
        inputUnit_inst->stream_in(tb_to_dut_data);
        inputUnit_inst->cmd_in(tb_to_dut_ctrl);
        inputUnit_inst->bfu_out(dut_to_tb_data);
        inputUnit_inst->pkt_buf_out(pkt_buf);
    }

    ~SYSTEM() {
        delete tb_inst;
        delete inputUnit_inst;
    }
};

SYSTEM *top = NULL;
int sc_main(int argc, char* argv[])
{
    top = new SYSTEM("top");
    sc_start();
    return 0;
}