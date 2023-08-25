#include "parse.h"

int forward_exact(sc_biguint<48> &dstAddr, sc_biguint<16> &port) {
    if (dstAddr == 0x0cc47aa32534) {
        port = 1;
        return 0;
    } else if (dstAddr == 0x0cc47aa32535) {
        port = 2;
        return 0;
    }
    return 1;
}

void parse::parse_main() {
    sc_biguint<4> hdr_count;
    ethernet_t eth;
    ptp_l_t ptp_l;
    ptp_h_t ptp_h;
    ipv4_t ipv4;
    tcp_t tcp;
    udp_t udp;
    header_t header_0;
    header_t header_1;
    header_t header_2;
    header_t header_3;
    header_t header_4;
    header_t header_5;
    header_t header_6;
    header_t header_7;
    standard_metadata_t standard_metadata;

    // initialize handshake
    Init();

    // main FSM
    while (true) {
        standard_metadata.egress_spec = 0;
        standard_metadata.mcast_grp = 0;
        hdr_count = 0;
        Input_header<ethernet_t>(14, eth);
        if (eth.etherType == 0x88f7) {
            Input_header<ptp_l_t>(20, ptp_l);
            Input_header<ptp_h_t>(24, ptp_h);
            hdr_count = 1;
            if (ptp_l.reserved2 == 1) {
                Input_header<header_t>(8, header_0);
                hdr_count = 2;
                if (header_0.field_0 != 0) {
                    Input_header<header_t>(8, header_1);
                    hdr_count = 3;
                    if (header_1.field_0 != 0) {
                        Input_header<header_t>(8, header_2);
                        hdr_count = 4;
                        if (header_2.field_0 != 0) {
                            Input_header<header_t>(8, header_3);
                            hdr_count = 5;
                            if (header_3.field_0 != 0) {
                                Input_header<header_t>(8, header_4);
                                hdr_count = 6;
                                if (header_4.field_0 != 0) {
                                    Input_header<header_t>(8, header_5);
                                    hdr_count = 7;
                                    if (header_5.field_0 != 0) {
                                        Input_header<header_t>(8, header_6);
                                        hdr_count = 8;
                                        if (header_6.field_0 != 0) {
                                            Input_header<header_t>(8, header_7);
                                            hdr_count = 9;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if (eth.etherType == 0x800) {
            Input_header<ipv4_t>(20, ipv4);
            hdr_count = 10;
            if (ipv4.protocol == 6) {
                Input_header<tcp_t>(20, tcp);
                hdr_count = 11;
            } else if (ipv4.protocol == 0x11) {
                Input_header<udp_t>(8, udp);
                hdr_count = 12;
            }
        }
        Input_done();

        // Ingress
        int flag;
        sc_biguint<16> port;
        flag = forward_exact(eth.dstAddr, port);
        switch (flag) {
        case 0:
            standard_metadata.egress_spec = port;
            break;
        case 1:
            standard_metadata.egress_spec = 0x1ff;
            break;
        }

        Output_meta(standard_metadata);
        Output_header<ethernet_t>(14, eth);
        if (hdr_count < 10) {
            Output_header<ptp_l_t>(20, ptp_l);
            Output_header<ptp_h_t>(24, ptp_h);
            if (hdr_count > 1) {
                Output_header<header_t>(8, header_0);
                if (hdr_count > 2) {
                    Output_header<header_t>(8, header_1);
                    if (hdr_count > 3) {
                        Output_header<header_t>(8, header_2);
                        if (hdr_count > 4) {
                            Output_header<header_t>(8, header_3);
                            if (hdr_count > 5) {
                                Output_header<header_t>(8, header_4);
                                if (hdr_count > 6) {
                                    Output_header<header_t>(8, header_5);
                                    if (hdr_count > 7) {
                                        Output_header<header_t>(8, header_6);
                                        if (hdr_count > 8) {
                                            Output_header<header_t>(8, header_7);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            Output_header<ipv4_t>(20, ipv4);
            if (hdr_count == 11) {
                Output_header<tcp_t>(20, tcp);
            } else {
                Output_header<udp_t>(8, udp);
            }
        }
        Output_done();
    }

}