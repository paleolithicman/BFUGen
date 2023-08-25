#include <systemc.h>
#include "common.h"
#include "parse.h"
#include <map>
#define NUM_PKT 2
using namespace std;

sc_biguint<512> str2biguint(string data) {
    sc_biguint<512> res;
    int length = data.length();
    long long unsigned int val[8];
    for (int i = 7; i >= 0; i--) {
        val[7-i] = stoull(data.substr(16*i, 16), NULL, 16);
    }
    res = (sc_biguint<64>(val[7]), sc_biguint<64>(val[6]), sc_biguint<64>(val[5]), sc_biguint<64>(val[4]), 
        sc_biguint<64>(val[3]), sc_biguint<64>(val[2]), sc_biguint<64>(val[1]), sc_biguint<64>(val[0]));
    return res;
}

SC_MODULE(source) {
public:
    sc_in<bool>                      i_clk;
    sc_out<bool>                     i_rst;

    primate_stream_512_4::out        stream_out;

    std::ifstream infile;

    SC_HAS_PROCESS(source);
    source(sc_module_name name_) {
        SC_CTHREAD(th_run, i_clk.pos());
    }

    void th_run() {
        string indata;
        int    empty;
        bool   last;

        // infile.open("/home/rui.ma/BFUGen/parse/input.txt");
        infile.open("input.txt");

        // Reset
        stream_out.reset();
        i_rst.write(1);
        wait();
        i_rst.write(0);
        wait();

        // Send stimulus to DUT
        for (int i = 0; i < NUM_PKT; i++) {
            cout << "packet " << i << endl;
            // start_time[i] = sc_time_stamp();

            do {
                infile >> last >> empty >> indata;
                primate_stream_512_4::payload_t payload(str2biguint(indata), i, empty, last);
                stream_out.write(payload);
            } while (!last);
        }

        wait(10000);
        cout << "Hanging simulaiton stopped by TB source thread. Please check DUT module." << endl;
        sc_stop();
    }

};

SC_MODULE(sink) {
public:
    sc_in<bool>                      i_clk;

    primate_stream_512_4::in         stream_in;

    std::ofstream outfile;

    SC_HAS_PROCESS(sink);
    sink(sc_module_name name_) {
        SC_CTHREAD(th_run, i_clk.pos());
    }

    void th_run() {
        primate_stream_512_4::payload_t out_data;
        outfile.open("output.txt");
        // Reset
        stream_in.reset();
        wait();

        for (int i = 0; i < NUM_PKT; i++) {
            do {
                out_data = stream_in.read();
                outfile << out_data;
            } while (!out_data.last);
        }

        // End Simulation
        sc_stop();
    }
};

SC_MODULE(tb) {
public:
    // Module declarations
    source *source_inst;
    sink *sink_inst;
    parse *parse_inst;

    // local signal declarations
    sc_signal<bool> rst_sig;
    sc_clock        clk_sig;

    primate_stream_512_4                tb_to_dut_data;

    primate_stream_512_4                dut_to_tb_data;

    SC_CTOR(tb) : clk_sig("clk_sig", 4, SC_NS) {
        source_inst = new source("source_inst");
        source_inst->i_clk(clk_sig);
        source_inst->i_rst(rst_sig);
        source_inst->stream_out(tb_to_dut_data);

        sink_inst = new sink("sink_inst");
        sink_inst->i_clk(clk_sig);
        sink_inst->stream_in(dut_to_tb_data);

        parse_inst = new parse("parse_inst");
        parse_inst->i_clk(clk_sig);
        parse_inst->i_rst(rst_sig);
        parse_inst->stream_in(tb_to_dut_data);
        parse_inst->stream_out(dut_to_tb_data);
    }

    ~tb() {
        delete source_inst;
        delete sink_inst;
        delete parse_inst;
    }
};

int sc_main(int argc, char* argv[])
{
    tb *top = new tb("my_tb");
    sc_report_handler::set_actions(SC_ERROR, SC_DISPLAY);
    sc_start();
    if(sc_report_handler::get_count(SC_ERROR) > 0) {
        cout << "Simulation FAILED" << endl;
        return -1;
    } else {
        cout << "Simulation PASSED" << endl;
    }
    return 0;
}
