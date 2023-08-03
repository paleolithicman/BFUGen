#include "tb.h"
#include <map>
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

void tb::source() {
    string indata;
    int    empty;
    bool   last;

    infile.open("input.txt");

    // Reset
    stream_out.reset();
    cmd_out.reset();
    rst.write(1);
    wait();
    rst.write(0);
    wait();

    // Send stimulus to DUT
    for (int i = 0; i < NUM_PKT; i++) {
        cout << "packet " << i << endl;
        start_time[i] = sc_time_stamp();

        primate_ctrl_iu::cmd_t cmd(i, 4, 1, 0, 0);
        cmd_out.write(cmd);

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

void tb::sink() {
    map<int, int> reg2idx{{1, 0}, {2, 1}, {3, 2}, {4, 3}, {5, 4}, {22, 5}};
    int idx2reg[6] = {1, 2, 3, 4, 5, 22};
    sc_biguint<REG_WIDTH> regs[6];
    primate_stream_512_4::payload_t pkt_buf[4];

    // Extract clock period
    sc_clock *clk_p = dynamic_cast<sc_clock*>(clk.get_interface());
    clock_period = clk_p->period();

    outfile.open("output.txt");

    // Initialize port
    bfu_in.ready.write(1);
    pkt_buf_in.reset();
    primate_stream_512_4::payload_t payload;

    double total_cycles = 0;

    // Read output coming from DUT
    for (int i = 0; i < NUM_PKT; i++) {
        outfile << "Thread " << i << ":" << endl;
        int num_pkt_buf = 0;
        do {
            // if (out_wen[0].read() || out_wen[1].read()) {
            //     cout << sc_time_stamp() << ": waddr0 " << out_addr[0].read() << ", wadd1 " << out_addr[1].read() << endl;
            //     cout << sc_time_stamp() << ": wdata0 " << hex << out_data[0].read() << ", wdata1 " << out_data[1].read() << dec << endl;
            // }
            for (int j = 0; j < 2; j++) {
                if (bfu_in.wen[j].read()) {
                    int regid = bfu_in.addr[j].read();
                    regs[reg2idx[regid]] = bfu_in.data[j].read();
                }
            }
            if (pkt_buf_in.nb_read(payload)) {
                pkt_buf[num_pkt_buf] = payload;
                num_pkt_buf++;
            }
            wait();
        } while (!bfu_in.valid.read());
        end_time[i] = sc_time_stamp();
        total_cycles += (end_time[i] - start_time[i]) / clock_period;

        // Print outputs
        for (int j = 0; j < 6; j++) {
            outfile << "REG " << idx2reg[j] << ": " << hex << regs[j] << dec << endl;
        }
        for (int j = 0; j < num_pkt_buf; j++) {
            outfile << pkt_buf[j];
        }
    }

    // Print latency
    double total_throughput = (start_time[NUM_PKT-1] - start_time[0]) / clock_period;
    printf("Average lantency is %g cycles.\n", (double)(total_cycles/64));
    printf("Average throughput is %g cycles per input.\n", (double)(total_throughput/64));

    // End Simulation
    sc_stop();
}