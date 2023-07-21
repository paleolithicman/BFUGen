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

    sc_signal<bool>                     in_valid;
    sc_signal<sc_uint<NUM_THREADS_LG>>  in_tag;
    sc_signal<sc_biguint<512>>          in_data;
    sc_signal<sc_uint<6>>               in_empty;
    sc_signal<bool>                     in_last;
    sc_signal<bool>                     in_ready;

    sc_signal<bool>                     ar_valid;
    sc_signal<sc_uint<NUM_THREADS_LG>>  ar_tag;
    sc_signal<sc_uint<OPCODE_WIDTH>>    ar_opcode;
    sc_signal<sc_uint<NUM_REGS_LG>>     ar_rd;
    sc_signal<sc_uint<32>>              ar_bits;
    sc_signal<sc_uint<32>>              ar_imm;
    sc_signal<bool>                     ar_ready;


    sc_signal<bool>                     out_ready;
    sc_signal<bool>                     out_valid;
    sc_signal<sc_uint<NUM_THREADS_LG>>  out_tag;
    sc_signal<sc_uint<IP_WIDTH>>        out_flag;
    sc_vector<sc_signal<bool>>                    out_wen{"out_wen", 2};
    sc_vector<sc_signal<sc_uint<NUM_REGS_LG>>>    out_addr{"out_addr", 2};
    sc_vector<sc_signal<sc_biguint<REG_WIDTH>>>   out_data{"out_data", 2};

    sc_signal<bool>                     pkt_buf_valid;
    sc_signal<pkt_buf_t>                pkt_buf_data;
    sc_signal<bool>                     pkt_buf_ready;

    SC_CTOR(SYSTEM) : clk_sig("clk_sig", 4, SC_NS) {
        tb_inst = new tb("tb_inst");
        tb_inst->clk(clk_sig);
        tb_inst->rst(rst_sig);
        tb_inst->in_valid(in_valid);
        tb_inst->in_tag(in_tag);
        tb_inst->in_data(in_data);
        tb_inst->in_empty(in_empty);
        tb_inst->in_last(in_last);
        tb_inst->in_ready(in_ready);
        tb_inst->ar_valid(ar_valid);
        tb_inst->ar_tag(ar_tag);
        tb_inst->ar_opcode(ar_opcode);
        tb_inst->ar_rd(ar_rd);
        tb_inst->ar_bits(ar_bits);
        tb_inst->ar_imm(ar_imm);
        tb_inst->ar_ready(ar_ready);
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
        inputUnit_inst->in_valid(in_valid);
        inputUnit_inst->in_tag(in_tag);
        inputUnit_inst->in_data(in_data);
        inputUnit_inst->in_empty(in_empty);
        inputUnit_inst->in_last(in_last);
        inputUnit_inst->in_ready(in_ready);
        inputUnit_inst->ar_valid(ar_valid);
        inputUnit_inst->ar_tag(ar_tag);
        inputUnit_inst->ar_opcode(ar_opcode);
        inputUnit_inst->ar_rd(ar_rd);
        inputUnit_inst->ar_bits(ar_bits);
        inputUnit_inst->ar_imm(ar_imm);
        inputUnit_inst->ar_ready(ar_ready);
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