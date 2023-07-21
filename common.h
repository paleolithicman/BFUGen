#ifndef _COMMON_
#define _COMMON_

#include <systemc.h>
#include <iostream>
#include <fstream>

#define NUM_THREADS 16
#define NUM_THREADS_LG 4
#define REG_WIDTH 192
#define NUM_REGS_LG 5
#define OPCODE_WIDTH 6
#define IP_WIDTH 32

struct pkt_buf_t {
    sc_biguint<512>         data;
    sc_uint<NUM_THREADS_LG> tag;
    sc_uint<7>              empty;
    bool                    last;

    pkt_buf_t(sc_biguint<512> data = 0, sc_uint<NUM_THREADS_LG> tag = 0, sc_uint<7> empty = 0, bool last = true) : 
        data(data), tag(tag), empty(empty), last(last) {}

    pkt_buf_t& operator= (const pkt_buf_t& arg) {
        data = arg.data;
        tag = arg.tag;
        empty = arg.empty;
        last = arg.last;
        return (*this);
    }

    bool operator== (const pkt_buf_t& arg) const {
        return ((data == arg.data) && (tag == arg.tag) &&
            (empty == arg.empty) && (last == arg.last));
    }
};

inline std::ostream& operator<<(std::ostream& os, const pkt_buf_t& val) {
    os << "data = " << val.data << "; tag = " << val.tag << 
        "; empty = " << val.empty << "; last = " << val.last << std::endl;
    return os;
}

inline void sc_trace(sc_trace_file*& f, const pkt_buf_t& val, std::string name) {
    sc_trace(f, val.data, name + ".data");
    sc_trace(f, val.tag, name + ".tag");
    sc_trace(f, val.empty, name + ".empty");
    sc_trace(f, val.last, name + ".last");
}

#endif