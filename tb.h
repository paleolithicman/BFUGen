#include <systemc.h>
#include "common.h"
#define NUM_PKT 2

SC_MODULE(tb) {
    sc_in<bool>                      clk;
    sc_out<bool>                     rst;

    sc_out<bool>                     in_valid;
    sc_out<sc_uint<NUM_THREADS_LG>>  in_tag;
    sc_out<sc_biguint<512>>          in_data;
    sc_out<sc_uint<6>>               in_empty;
    sc_out<bool>                     in_last;
    sc_in<bool>                      in_ready;

    sc_out<bool>                     ar_valid;
    sc_out<sc_uint<NUM_THREADS_LG>>  ar_tag;
    sc_out<sc_uint<OPCODE_WIDTH>>    ar_opcode;
    sc_out<sc_uint<NUM_REGS_LG>>     ar_rd;
    sc_out<sc_uint<32>>              ar_bits;
    sc_out<sc_uint<32>>              ar_imm;
    sc_in<bool>                      ar_ready;


    sc_out<bool>                     out_ready;
    sc_in<bool>                      out_valid;
    sc_in<sc_uint<NUM_THREADS_LG>>   out_tag;
    sc_in<sc_uint<IP_WIDTH>>         out_flag;
    sc_vector<sc_in<bool>>                    out_wen{"out_wen", 2};
    sc_vector<sc_in<sc_uint<NUM_REGS_LG>>>    out_addr{"out_addr", 2};
    sc_vector<sc_in<sc_biguint<REG_WIDTH>>>   out_data{"out_data", 2};

    sc_in<bool>                      pkt_buf_valid;
    sc_in<pkt_buf_t>                 pkt_buf_data;
    sc_out<bool>                     pkt_buf_ready;

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