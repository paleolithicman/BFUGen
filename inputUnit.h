#include "common.h"

SC_MODULE(inputUnit) {
    sc_in<bool> clk;
    sc_in<bool> rst;

    sc_in<bool>                     in_valid;
    sc_in<sc_uint<NUM_THREADS_LG>>  in_tag;
    sc_in<sc_biguint<512>>          in_data;
    sc_in<sc_uint<6>>               in_empty;
    sc_in<bool>                     in_last;
    sc_out<bool>                    in_ready;

    sc_in<bool>                     ar_valid;
    sc_in<sc_uint<NUM_THREADS_LG>>  ar_tag;
    sc_in<sc_uint<OPCODE_WIDTH>>    ar_opcode;
    sc_in<sc_uint<NUM_REGS_LG>>     ar_rd;
    sc_in<sc_uint<32>>              ar_bits;
    sc_in<sc_uint<32>>              ar_imm;
    sc_out<bool>                    ar_ready;


    sc_in<bool>                     out_ready;
    sc_out<bool>                    out_valid;
    sc_out<sc_uint<NUM_THREADS_LG>> out_tag;
    sc_out<sc_uint<IP_WIDTH>>       out_flag;
    sc_vector<sc_out<bool>>                    out_wen{"out_wen", 2};
    sc_vector<sc_out<sc_uint<NUM_REGS_LG>>>    out_addr{"out_addr", 2};
    sc_vector<sc_out<sc_biguint<REG_WIDTH>>>   out_data{"out_data", 2};

    sc_out<bool>                    pkt_buf_valid;
    sc_out<pkt_buf_t>               pkt_buf_data;
    sc_in<bool>                     pkt_buf_ready;

    void inputUnit_main();

    SC_CTOR(inputUnit) {
        SC_CTHREAD(inputUnit_main, clk.pos());
        reset_signal_is(rst, true);  // true is hihg, flase is low
    };
};