#include "tb.h"
#include <map>
using namespace std;

string pkt_data[3] = {
    "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    "00000000f0000000e0000000d0000000c0000000b0000000a00000009000000080000000700000006000000050000000400000003000000020000000100000000",
    "00000000f0000000e0000000d0000000c0000000b0000000a00000009000000080000000700000006000000050000000400000003000000020000000100000000"
};

string regfile_data[5] = {
    "00000000000000000000088f700000000beef00000000dead",
    "00000000000000000000000000000cafe0000010000081000",
    "00000000000000000aaaa0000beef00000000000000000000",
    "0000000000000000000040003000200010004000300020001",
    "0000000000000000000040003000200000004000300020001"
};

sc_biguint<512> str2biguint512(string data) {
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

sc_biguint<192> str2biguint192(string data) {
    sc_biguint<192> res;
    int length = data.length();
    long long unsigned int val[3];
    for (int i = 2; i >= 0; i--) {
        val[2-i] = stoull(data.substr(16*i, 16), NULL, 16);
    }
    res = (sc_biguint<64>(val[2]), sc_biguint<64>(val[1]), sc_biguint<64>(val[0]));
    return res;
}

void tb::regfile() {
    primate_bfu_req_t req;

    // Reset
    bfu_rdreq.reset();
    bfu_rdrsp.reset();
    wait();

    while (true) {
        req = bfu_rdreq.read();
        int addr0 = req.addr0;
        int addr1 = req.addr1;
        bfu_rdrsp.write(primate_bfu_rsp_t(str2biguint192(regfile_data[addr0-1]), str2biguint192(regfile_data[addr1-1])));
    }
}

void tb::source() {
    sc_uint<NUM_THREADS_LG> tag;
    sc_uint<IP_WIDTH> flag;
    // Reset
    cmd_out.reset();
    pkt_buf_out.reset();
    rst.write(1);
    wait();
    rst.write(0);
    wait();

    // Send stimulus to DUT
    for (int i = 0; i < NUM_PKT; i++) {
        cout << "packet " << i << endl;
        start_time[i] = sc_time_stamp();

        primate_ctrl_ou::cmd_t cmd(i, 4, 1, 6, 0);
        cmd_out.write(cmd);

        pkt_buf_out.write(primate_io_payload_t(str2biguint512(pkt_data[0]), i, 26, 0));
        pkt_buf_out.write(primate_io_payload_t(str2biguint512(pkt_data[1]), i, 0, 0));
        pkt_buf_out.write(primate_io_payload_t(str2biguint512(pkt_data[2]), i, 0, 1));

        bfu_in.read(tag, flag);
        cout << "Thread " << tag << ", flag " << flag << endl;
    }

    wait(10000);
    cout << "Hanging simulaiton stopped by TB source thread. Please check DUT module." << endl;
    sc_stop();

}

void tb::sink() {
    primate_io_payload_t out_data;
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

    sc_stop();
}