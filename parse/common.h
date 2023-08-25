#ifndef _COMMON_
#define _COMMON_

#include <systemc.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "../include/primate_stream.h"
#include "../include/primate_ctrl.h"
#include "../include/primate_bfu_mc.h"

#define IO_W 512
#define IO_BW 64
#define IO_BW_LG 6
#define NUM_THREADS 16
#define NUM_THREADS_LG 4
#define REG_WIDTH 192
#define NUM_REGS_LG 5
#define OPCODE_WIDTH 6
#define IP_WIDTH 32

typedef primate_ctrl<NUM_THREADS_LG, OPCODE_WIDTH, NUM_REGS_LG, 32> primate_ctrl_iu;
typedef primate_bfu_mc::write_mc<NUM_THREADS_LG, IP_WIDTH, NUM_REGS_LG, REG_WIDTH> primate_bfu_iu;
typedef primate_bfu_mc::read_mc::req<NUM_THREADS_LG, NUM_REGS_LG>::master primate_bfu_rdreq_ou;
typedef primate_bfu_mc::read_mc::rsp<REG_WIDTH>::slave primate_bfu_rdrsp_ou;

typedef struct {
    sc_biguint<48> dstAddr;
    sc_biguint<48> srcAddr;
    sc_biguint<16> etherType;

    void set(sc_biguint<112> bv) {
        dstAddr = bv.range(47, 0);
        srcAddr = bv.range(95, 48);
        etherType = bv.range(111, 96);
    }

    sc_biguint<REG_WIDTH> to_uint() {
        sc_biguint<REG_WIDTH> val = (0, etherType, srcAddr, dstAddr);
        return val;
    }
} ethernet_t;

typedef struct {
    sc_biguint<40> transportSpecific_domainNumber;
    sc_biguint<8> reserved2;
    sc_biguint<112> flags_reserved3;

    void set(sc_biguint<160> bv) {
        transportSpecific_domainNumber = bv.range(39, 0);
        reserved2 = bv.range(47, 40);
        flags_reserved3 = bv.range(159, 48);
    }

    sc_biguint<REG_WIDTH> to_uint() {
        sc_biguint<REG_WIDTH> val = (0, flags_reserved3, reserved2, transportSpecific_domainNumber);
        return val;
    }
} ptp_l_t;

typedef struct {
    sc_biguint<192> data;

    void set(sc_biguint<192> bv) {
        data = bv;
    }

    sc_biguint<REG_WIDTH> to_uint() {
        return data;
    }
} ptp_h_t;

typedef struct {
    sc_biguint<16> field_0;
    sc_biguint<16> field_1;
    sc_biguint<16> field_2;
    sc_biguint<16> field_3;

    void set(sc_biguint<64> bv) {
        field_0 = bv.range(15, 0);
        field_0 = bv.range(31, 16);
        field_0 = bv.range(47, 32);
        field_0 = bv.range(63, 48);
    }

    sc_biguint<REG_WIDTH> to_uint() {
        sc_biguint<REG_WIDTH> val = (0, field_3, field_2, field_1, field_0);
        return val;
    }
} header_t;

typedef struct {
    sc_biguint<72> version_ttl;
    sc_biguint<8> protocol;
    sc_biguint<80> hdrChecksum_dstAddr;

    void set(sc_biguint<160> bv) {
        version_ttl = bv.range(71, 0);
        protocol = bv.range(79, 72);
        hdrChecksum_dstAddr = bv.range(159, 80);
    }

    sc_biguint<REG_WIDTH> to_uint() {
        sc_biguint<REG_WIDTH> val = (0, hdrChecksum_dstAddr, protocol, version_ttl);
        return val;
    }
} ipv4_t;

typedef struct {
    sc_biguint<160> srcPort_urgentPtr;

    void set(sc_biguint<160> bv) {
        srcPort_urgentPtr = bv;
    }

    sc_biguint<REG_WIDTH> to_uint() {
        return srcPort_urgentPtr;
    }
} tcp_t;

typedef struct {
    sc_biguint<64> srcPort_checksum;

    void set(sc_biguint<64> bv) {
        srcPort_checksum = bv;
    }

    sc_biguint<REG_WIDTH> to_uint() {
        return srcPort_checksum;
    }
} udp_t;

typedef struct {
    sc_biguint<16> egress_spec;
    sc_biguint<16> mcast_grp;

    void set(sc_biguint<32> bv) {
        egress_spec = bv.range(15, 0);
        mcast_grp = bv.range(31, 16);
    }

    sc_biguint<REG_WIDTH> to_uint() {
        sc_biguint<REG_WIDTH> val = (0, mcast_grp, egress_spec);
        return val;
    }
} standard_metadata_t;

inline sc_biguint<REG_WIDTH> cat_header(header_t hdr1, header_t hdr0) {
    sc_biguint<REG_WIDTH> val = (0, hdr1.field_3, hdr1.field_2, hdr1.field_1, hdr1.field_0, hdr0.field_3, hdr0.field_2, hdr0.field_1, hdr0.field_0);
    return val;
}

// struct pkt_buf_t {
//     sc_biguint<512>         data;
//     sc_uint<NUM_THREADS_LG> tag;
//     sc_uint<7>              empty;
//     bool                    last;

//     pkt_buf_t(sc_biguint<512> data = 0, sc_uint<NUM_THREADS_LG> tag = 0, sc_uint<7> empty = 0, bool last = true) : 
//         data(data), tag(tag), empty(empty), last(last) {}

//     pkt_buf_t& operator= (const pkt_buf_t& arg) {
//         data = arg.data;
//         tag = arg.tag;
//         empty = arg.empty;
//         last = arg.last;
//         return (*this);
//     }

//     bool operator== (const pkt_buf_t& arg) const {
//         return ((data == arg.data) && (tag == arg.tag) &&
//             (empty == arg.empty) && (last == arg.last));
//     }
// };

// inline std::ostream& operator<<(std::ostream& os, const pkt_buf_t& val) {
//     os << "data = " << val.data << "; tag = " << val.tag << 
//         "; empty = " << val.empty << "; last = " << val.last << std::endl;
//     return os;
// }

// inline void sc_trace(sc_trace_file*& f, const pkt_buf_t& val, std::string name) {
//     sc_trace(f, val.data, name + ".data");
//     sc_trace(f, val.tag, name + ".tag");
//     sc_trace(f, val.empty, name + ".empty");
//     sc_trace(f, val.last, name + ".last");
// }

#endif
