#include "inputUnit.h"

struct ethernet_t {
    sc_biguint<48> dstAddr;
    sc_biguint<48> srcAddr;
    sc_biguint<16> etherType;

    void set(sc_biguint<112> bv) {
        dstAddr = bv.range(47, 0);
        srcAddr = bv.range(95, 48);
        etherType = bv.range(111, 96);
    }

    sc_biguint<192> to_uint() {
        sc_biguint<192> val = (0, etherType, srcAddr, dstAddr);
        return val;
    }
};

struct ptp_l_t {
    sc_biguint<40> transportSpecific_domainNumber;
    sc_biguint<8> reserved2;
    sc_biguint<112> flags_reserved3;

    void set(sc_biguint<160> bv) {
        transportSpecific_domainNumber = bv.range(39, 0);
        reserved2 = bv.range(47, 40);
        flags_reserved3 = bv.range(159, 48);
    }

    sc_biguint<192> to_uint() {
        sc_biguint<192> val = (0, flags_reserved3, reserved2, transportSpecific_domainNumber);
        return val;
    }
};

struct header_t {
    sc_biguint<16> field_0;
    sc_biguint<48> field_1_field_3;

    void set(sc_biguint<64> bv) {
        field_0 = bv.range(15, 0);
        field_1_field_3 = bv.range(63, 16);
    }

    sc_biguint<192> to_uint() {
        sc_biguint<192> val = (0, field_1_field_3, field_0);
        return val;
    }
};

sc_biguint<192> cat_header(header_t hdr1, header_t hdr0) {
    sc_biguint<192> val = (0, hdr1.field_1_field_3, hdr1.field_0, hdr0.field_1_field_3, hdr0.field_0);
    return val;
}

void inputUnit::inputUnit_main() {
    sc_biguint<512> in_data_buf;
    sc_biguint<512> pkt_data_buf;
    sc_uint<7> pkt_empty;
    bool last_buf;
    sc_uint<4> state;
    sc_biguint<4> hdr_count;
    sc_uint<NUM_THREADS_LG> tag;
    ethernet_t eth;
    ptp_l_t ptp_l;
    sc_biguint<192> ptp_h;
    header_t header_0;
    header_t header_1;
    header_t header_2;
    header_t header_3;

    // initialize handshake
    in_ready.write(0);
    ar_ready.write(0);
    pkt_buf_valid.write(0);
    out_valid.write(0);
    out_tag.write(0);
    out_flag.write(0);
    for (int i = 0; i < 2; i++) {
        out_wen[i].write(0);
    }
    state = 0;
    wait();

    // main FSM
    while (true) {
        ar_ready.write(0);
        in_ready.write(0);
        cout << sc_time_stamp() << ": state " << state << endl;
        cout << sc_time_stamp() << ": in_valid " << in_valid.read() << endl;
        if (state == 0) {
            ar_ready.write(1);
            in_ready.write(1);
            bool ar_started = false;
            bool in_started = false;
            do {
                wait();
                if (ar_valid.read()) {
                    ar_started = true;
                    ar_ready.write(0);
                }
                if (in_valid.read()) {
                    in_started = true;
                    in_ready.write(0);
                }
            } while (!(ar_started && in_started));
            
            cout << sc_time_stamp() << ": in_data " << hex << in_data.read() << dec << endl;
            in_data_buf = in_data.read();
            last_buf = in_last.read();
            tag = ar_tag.read();
            eth.set(in_data_buf.range(111, 0));
            ptp_l.set(in_data_buf.range(271, 112));
            if (eth.etherType == 0x88f7) {
                // write ethernet header and ptp header
                out_wen[0].write(1);
                out_addr[0].write(1);
                out_data[0].write(eth.to_uint());
                out_wen[1].write(1);
                out_addr[1].write(2);
                out_data[1].write(ptp_l.to_uint());
                out_valid.write(0);
                out_tag.write(ar_tag.read());
                state = 1;
            } else {
                // write ethernet header only
                out_wen[0].write(1);
                out_addr[0].write(1);
                out_data[0].write(eth.to_uint());
                out_wen[1].write(1);
                out_addr[1].write(22);
                out_data[1].write(1);
                out_valid.write(in_last.read());
                out_tag.write(ar_tag.read());
                pkt_data_buf = in_data_buf >> 112;
                pkt_empty = 112 / 8;
                state = 11;
            }

            do {
                wait();
            } while (!out_ready.read());
            out_wen[0].write(0);
            out_wen[1].write(0);
            out_valid.write(0);
        } else if (state == 1) {
            ptp_h = in_data_buf.range(463, 272);
            if (ptp_l.reserved2 == 1) {
                in_ready.write(1);
                do {
                    wait();
                } while (!in_valid.read());
                in_ready.write(0);

                cout << sc_time_stamp() << ": in_data " << hex << in_data.read() << dec << endl;
                sc_biguint<64> hdr_val = (in_data.read().range(15, 0), in_data_buf.range(511, 464));
                header_0.set(hdr_val);
                in_data_buf = in_data.read();
                last_buf = in_last.read();
                out_wen[0].write(1);
                out_addr[0].write(3);
                out_data[0].write(ptp_h);
                out_addr[1].write(4);
                out_data[1].write(header_0.to_uint());
                if (header_0.field_0 == 0) {
                    out_wen[1].write(1);
                    state = 2;
                } else {
                    state = 3;
                }
            } else {
                out_wen[0].write(1);
                out_addr[0].write(3);
                out_data[0].write(ptp_h);
                out_wen[1].write(1);
                out_addr[1].write(22);
                out_data[1].write(2);
                out_valid.write(last_buf);
                pkt_data_buf = in_data_buf >> 464;
                pkt_empty = 464 / 8;
                state = 11;
            }

            do {
                wait();
            } while (!out_ready.read());
            out_wen[0].write(0);
            out_wen[1].write(0);
            out_valid.write(0);
        } else if (state == 2) {
            out_wen[0].write(1);
            out_addr[0].write(22);
            out_data[0].write(3);
            out_wen[1].write(0);
            out_valid.write(last_buf);
            pkt_data_buf = in_data_buf >> 16;
            pkt_empty = 16 / 8;
            state = 11;

            do {
                wait();
            } while (!out_ready.read());
            out_wen[0].write(0);
            out_wen[1].write(0);
            out_valid.write(0);
        } else if (state == 3) {
            header_1.set(in_data_buf.range(79, 16));
            header_2.set(in_data_buf.range(143, 80));
            header_3.set(in_data_buf.range(207, 144));
            out_wen[1].write(1);
            out_addr[1].write(4);
            out_data[1].write(cat_header(header_1, header_0));
            cout << "in_data_buf: " << hex << in_data_buf << dec << endl;
            cout << header_1.field_0 << endl;
            if (header_1.field_0 == 0) {
                out_wen[0].write(1);
                out_addr[0].write(22);
                out_data[0].write(4);
                pkt_data_buf = in_data_buf >> 80;
                pkt_empty = 80 / 8;
                out_valid.write(last_buf);
                state = 11;
            } else {
                out_wen[0].write(1);
                out_addr[0].write(5);
                out_data[0].write(cat_header(header_3, header_2));
                if (header_2.field_0 == 0) {
                    pkt_data_buf = in_data_buf >> 144;
                    pkt_empty = 144 / 8;
                    hdr_count = 5;
                    state = 5;
                } else {
                    pkt_data_buf = in_data_buf >> 208;
                    pkt_empty = 208 / 8;
                    hdr_count = 6;
                    state = 5;
                }
            }

            do {
                wait();
            } while (!out_ready.read());
            out_wen[0].write(0);
            out_wen[1].write(0);
            out_valid.write(0);
        } else if (state == 5) {
            out_wen[0].write(1);
            out_addr[0].write(22);
            out_data[0].write(hdr_count);
            out_wen[1].write(0);
            out_valid.write(last_buf);
            state = 11;

            do {
                wait();
            } while (!out_ready.read());
            out_wen[0].write(0);
            out_wen[1].write(0);
            out_valid.write(0);
        } else if (state == 11) {
            out_valid.write(0);
            out_wen[0].write(0);
            out_wen[1].write(0);
            pkt_buf_t pkt_buf_in(pkt_data_buf, tag, pkt_empty, last_buf);
            pkt_buf_valid.write(1);
            pkt_buf_data.write(pkt_buf_in);
            if (last_buf) {
                state = 0;
            } else {
                state = 12;
            }

            do {
                wait();
            } while (!pkt_buf_ready.read());
            pkt_buf_valid.write(0);
        } else if (state == 12) {
            in_ready.write(1);
            do {
                wait();
            } while (!in_valid.read());
            in_ready.write(0);
            cout << sc_time_stamp() << ": in_data " << hex << in_data.read() << dec << endl;
            cout << sc_time_stamp() << ": in_last " << hex << in_last.read() << dec << endl;
            pkt_buf_t pkt_buf_in(in_data.read(), tag, in_empty.read(), in_last.read());
            pkt_buf_valid.write(1);
            pkt_buf_data.write(pkt_buf_in);
            if (in_last.read()) {
                out_valid.write(1);
                state = 0;
            }

            do {
                wait();
                out_valid.write(0);
            } while (!pkt_buf_ready.read());
            pkt_buf_valid.write(0);
        }
    }


}