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

    sc_signal<bool>                     out_ready;
    sc_signal<bool>                     out_valid;
    sc_signal<sc_uint<NUM_THREADS_LG>>  out_tag;
    sc_signal<sc_uint<IP_WIDTH>>        out_flag;
    sc_vector<sc_signal<bool>>                    out_wen{"out_wen", 2};
    sc_vector<sc_signal<sc_uint<NUM_REGS_LG>>>    out_addr{"out_addr", 2};
    sc_vector<sc_signal<sc_biguint<REG_WIDTH>>>   out_data{"out_data", 2};

    sc_signal<bool>                            pkt_buf_valid;
    sc_signal<primate_stream_512_4::payload_t> pkt_buf_data;
    sc_signal<bool>                            pkt_buf_ready;

    SC_CTOR(SYSTEM) : clk_sig("clk_sig", 4, SC_NS) {
        tb_inst = new tb("tb_inst");
        tb_inst->clk(clk_sig);
        tb_inst->rst(rst_sig);
        tb_inst->stream_out(tb_to_dut_data);
        tb_inst->cmd_out(tb_to_dut_ctrl);
        tb_inst->out_ready(out_ready);
        tb_inst->out_valid(out_valid);
        tb_inst->out_tag(out_tag);
        tb_inst->out_flag(out_flag);
        tb_inst->out_wen(out_wen);
        tb_inst->out_addr(out_addr);
        tb_inst->out_data(out_data);
        tb_inst->pkt_buf_valid(pkt_buf_valid);
        tb_inst->pkt_buf_data(pkt_buf_data);
        tb_inst->pkt_buf_ready(pkt_buf_ready);

        inputUnit_inst = new inputUnit("inputUnit_inst");
        inputUnit_inst->clk(clk_sig);
        inputUnit_inst->rst(rst_sig);
        inputUnit_inst->stream_in(tb_to_dut_data);
        inputUnit_inst->cmd_in(tb_to_dut_ctrl);
        inputUnit_inst->out_ready(out_ready);
        inputUnit_inst->out_valid(out_valid);
        inputUnit_inst->out_tag(out_tag);
        inputUnit_inst->out_flag(out_flag);
        inputUnit_inst->out_wen(out_wen);
        inputUnit_inst->out_addr(out_addr);
        inputUnit_inst->out_data(out_data);
        inputUnit_inst->pkt_buf_valid(pkt_buf_valid);
        inputUnit_inst->pkt_buf_data(pkt_buf_data);
        inputUnit_inst->pkt_buf_ready(pkt_buf_ready);
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