#include "inputUnit.h"

void inputUnit::inputUnit_main() {
    primate_ctrl_iu::cmd_t cmd;

    // initialize handshake
    bt0 = 0;
    bt1 = 0;
    bt2 = 0;
    stream_in.reset();
    cmd_in.reset();
    pkt_buf_out.reset();
    bfu_out.reset();
    
    wait();

    // main FSM
// #pragma hls_pipeline_init_interval 1
// #pragma hls_pipeline_stall_mode flush
    while (true) {
        cmd = cmd_in.read();
        tag = cmd.ar_tag;
        opcode = cmd.ar_opcode;
        if (opcode == 0x3f) {
            if (cmd.ar_imm == 0) {
                bt0 = cmd.ar_bits;
            } else if (cmd.ar_imm == 1) {
                bt1 = cmd.ar_bits;
            } else if (cmd.ar_imm == 2) {
                bt2 = cmd.ar_bits;
            }
            wait();
        } else {
            inputUnit_core();
        }
    }
}

void inputUnit::inputUnit_core() {
    primate_stream_512_4::payload_t payload;
    sc_biguint<512> in_data_buf;
    bool last_buf;
    int path;

    sc_biguint<4> hdr_count;
    sc_uint<8> pkt_empty;
    ethernet_t eth;
    ptp_l_t ptp_l;
    ptp_h_t ptp_h;
    header_t header_0;
    header_t header_1;
    header_t header_2;
    header_t header_3;
    header_t header_4;
    header_t header_5;
    header_t header_6;
    header_t header_7;
    hdr_count = 0;
    pkt_empty = 0;

    // Input_header<ethernet_t>(14, eth); {
    payload = stream_in.read();
    in_data_buf = payload.data;
    last_buf = payload.last;
    eth.set(payload.data.range(111, 0));
    pkt_empty = 14;
    // bfu_out.write(tag, bt0, 1, eth.to_uint());
    // }
    if (eth.etherType == 0x88f7) {
        // Input_header<ptp_l_t>(20, ptp_l); {
        ptp_l.set(payload.data.range(271, 112));
        pkt_empty = 34;
        // }
        // Input_header<ptp_h_t>(24, ptp_h); {
        ptp_h.set(payload.data.range(463, 272));
        pkt_empty = 58;
        // }
        // bfu_out.write(tag, bt0, 2, ptp_l.to_uint(), 3, ptp_h.to_uint());
        hdr_count = 1;
        if (ptp_l.reserved2 == 1) {
            // Input_header<header_t>(8, header_0); {
            payload = stream_in.read();
            header_0.set((payload.data.range(15, 0), in_data_buf.range(511, 464)));
            pkt_empty = 2;
            in_data_buf = payload.data;
            last_buf = payload.last;
            // }
            // bfu_out.write(tag, bt0, 4, header_0.to_uint());
            hdr_count = 2;
            if (header_0.field_0 != 0) {
                // Input_header<header_t>(8, header_1); {
                header_1.set(in_data_buf.range(79, 16));
                pkt_empty = 10;
                // }
                // bfu_out.write(tag, bt0, 4, header_1.to_uint());
                hdr_count = 3;
                if (header_1.field_0 != 0) {
                    // Input_header<header_t>(8, header_2); {
                    header_2.set(in_data_buf.range(143, 80));
                    pkt_empty = 18;
                    // }
                    // bfu_out.write(tag, bt0, 5, header_2.to_uint());
                    hdr_count = 4;
                    if (header_2.field_0 != 0) {
                        // Input_header<header_t>(8, header_3); {
                        header_3.set(in_data_buf.range(207, 144));
                        pkt_empty = 26;
                        // }
                        // bfu_out.write(tag, bt0, 6, header_3.to_uint());
                        hdr_count = 5;
                        if (header_3.field_0 != 0) {
                            // early exit
                            bfu_out.write(tag, bt1, 1, eth.to_uint(), 2, ptp_l.to_uint());
                            bfu_out.write(tag, bt1, 3, ptp_h.to_uint(), 4, header_0.to_uint());
                            bfu_out.write(tag, bt1, 5, header_1.to_uint(), 6, header_2.to_uint());
                            bfu_out.write(tag, bt1, 7, header_3.to_uint(), 22, hdr_count, true);
                            return;
                        }
                    }
                }
            }
        }
    } else if (eth.etherType == 0x800) {
        // early exit
        bfu_out.write(tag, bt2, 1, eth.to_uint(), 22, hdr_count, true);
        return;
    }

    // Input_done(); {
    payload.data = in_data_buf.range(511, pkt_empty*8);
    payload.empty = pkt_empty;
    payload.last = last_buf;
    pkt_buf_out.write(payload);
    while (!last_buf) {
        payload = stream_in.read();
        last_buf = payload.last;
        pkt_buf_out.write(payload);
    }
    bfu_out.write(tag, bt0, 1, eth.to_uint(), 2, ptp_l.to_uint());
    bfu_out.write(tag, bt0, 3, ptp_h.to_uint(), 4, header_0.to_uint());
    bfu_out.write(tag, bt0, 5, header_1.to_uint(), 6, header_2.to_uint());
    bfu_out.write(tag, bt1, 7, header_3.to_uint(), 22, hdr_count, true);
    // }
}
