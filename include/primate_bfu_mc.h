#ifndef _PRIMATE_BFU_MC_
#define _PRIMATE_BFU_MC_

#include <systemc.h>
#include <string>
#include "ready_valid.h"

namespace primate_bfu_mc {
template<int TAG_W, int IP_W, int ADDR_W, int DATA_W>
class write_mc {
public:
    class master {
    public:
        master(const char* name=sc_gen_unique_name("primate_bfu_mc_wr_m")) :
            ready((std::string(name)+"_ready").c_str()),
            valid((std::string(name)+"_valid").c_str()),
            tag((std::string(name)+"_tag").c_str()),
            flag((std::string(name)+"_flag").c_str()),
            wen0((std::string(name)+"_wen0").c_str()),
            addr0((std::string(name)+"_addr0").c_str()),
            data0((std::string(name)+"_data0").c_str()),
            wen1((std::string(name)+"_wen1").c_str()),
            addr1((std::string(name)+"_addr1").c_str()),
            data1((std::string(name)+"_data1").c_str())
        {}

        template<typename CHANNEL>
        inline void bind(CHANNEL &channel) {
            ready(channel.ready);
            valid(channel.valid);
            tag(channel.tag);
            flag(channel.flag);
            wen0(channel.wen0);
            addr0(channel.addr0);
            data0(channel.data0);
            wen1(channel.wen1);
            addr1(channel.addr1);
            data1(channel.data1);
        }

        template<typename CHANNEL>
        inline void operator () (CHANNEL &channel) {
            bind(channel);
        }

        inline void reset() {
            valid = false;
            tag.write(0);
            flag.write(0);
            wen0 = false;
            addr0 = 0; 
            data0 = 0;
            wen1 = false;
            addr1 = 0;
            data1 = 0;
        }

        inline void write_last(sc_uint<TAG_W> out_tag, sc_uint<IP_W> out_flag) {
            valid = true;
            tag.write(out_tag);
            flag.write(out_flag);
            wen0 = false;
            wen1 = false;
            wait();
            valid = false;
        }

        inline void write(sc_uint<TAG_W> out_tag, sc_uint<IP_W> out_flag, sc_uint<ADDR_W> out_addr,
         sc_biguint<DATA_W> out_data, bool is_last = false) {
            valid.write(is_last);
            tag.write(out_tag);
            flag.write(out_flag);
            wen0 = true;
            addr0.write(out_addr);
            data0.write(out_data);
            wen1 = false;
            do { ::wait(); } while (ready.read() == false);
            valid = false;
            wen0 = false;
        }

        inline void write(sc_uint<TAG_W> out_tag, sc_uint<IP_W> out_flag, sc_uint<ADDR_W> out_addr0,
         sc_biguint<DATA_W> out_data0, sc_uint<ADDR_W> out_addr1, sc_biguint<DATA_W> out_data1, bool is_last = false) {
            valid.write(is_last);
            tag.write(out_tag);
            flag.write(out_flag);
            wen0 = true;
            addr0.write(out_addr0);
            data0.write(out_data0);
            wen1 = true;
            addr1.write(out_addr1);
            data1.write(out_data1);
            do { ::wait(); } while (ready.read() == false);
            valid = false;
            wen0 = false;
            wen1 = false;
        }

    public:
        sc_in<bool>                ready;
        sc_out<bool>               valid;
        sc_out<sc_uint<TAG_W>>     tag;
        sc_out<sc_uint<IP_W>>      flag;
        sc_out<bool>               wen0;
        sc_out<sc_uint<ADDR_W>>    addr0;
        sc_out<sc_biguint<DATA_W>> data0;
        sc_out<bool>               wen1;
        sc_out<sc_uint<ADDR_W>>    addr1;
        sc_out<sc_biguint<DATA_W>> data1;
    }; // class master

public:
    class slave {
    public:
        slave(const char* name=sc_gen_unique_name("primate_bfu_mc_wr_s")) :
            ready((std::string(name)+"_ready").c_str()),
            valid((std::string(name)+"_valid").c_str()),
            tag((std::string(name)+"_tag").c_str()),
            flag((std::string(name)+"_flag").c_str()),
            wen0((std::string(name)+"_wen0").c_str()),
            addr0((std::string(name)+"_addr0").c_str()),
            data0((std::string(name)+"_data0").c_str()),
            wen1((std::string(name)+"_wen1").c_str()),
            addr1((std::string(name)+"_addr1").c_str()),
            data1((std::string(name)+"_data1").c_str())
        {}

        template<typename CHANNEL>
        inline void bind(CHANNEL &channel) {
            ready(channel.ready);
            valid(channel.valid);
            tag(channel.tag);
            flag(channel.flag);
            wen0(channel.wen0);
            addr0(channel.addr0);
            data0(channel.data0);
            wen1(channel.wen1);
            addr1(channel.addr1);
            data1(channel.data1);
        }

        template<typename CHANNEL>
        inline void operator () (CHANNEL &channel) {
            bind(channel);
        }

        inline void reset() {
            ready = false;
        }

    public:
        sc_out<bool>              ready;
        sc_in<bool>               valid;
        sc_in<sc_uint<TAG_W>>     tag;
        sc_in<sc_uint<IP_W>>      flag;
        sc_in<bool>               wen0;
        sc_in<sc_uint<ADDR_W>>    addr0;
        sc_in<sc_biguint<DATA_W>> data0;
        sc_in<bool>               wen1;
        sc_in<sc_uint<ADDR_W>>    addr1;
        sc_in<sc_biguint<DATA_W>> data1;
    }; // class slave

public:
    sc_signal<bool>               ready;
    sc_signal<bool>               valid;
    sc_signal<sc_uint<TAG_W>>     tag;
    sc_signal<sc_uint<IP_W>>      flag;
    sc_signal<bool>               wen0;
    sc_signal<sc_uint<ADDR_W>>    addr0;
    sc_signal<sc_biguint<DATA_W>> data0;
    sc_signal<bool>               wen1;
    sc_signal<sc_uint<ADDR_W>>    addr1;
    sc_signal<sc_biguint<DATA_W>> data1;
}; // write_mc

template<int TAG_W, int IP_W>
class out_simple {
public:
    class master {
    public:
        master(const char* name=sc_gen_unique_name("primate_bfu_mc_out_m")) :
            valid((std::string(name)+"_valid").c_str()),
            tag((std::string(name)+"_tag").c_str()),
            flag((std::string(name)+"_flag").c_str())
        {}

        template<typename CHANNEL>
        inline void bind(CHANNEL &channel) {
            valid(channel.valid);
            tag(channel.tag);
            flag(channel.flag);
        }

        template<typename CHANNEL>
        inline void operator () (CHANNEL &channel) {
            bind(channel);
        }

        inline void reset() {
            valid = false;
            tag.write(0);
            flag.write(0);
        }

        inline void write(sc_uint<TAG_W> out_tag, sc_uint<IP_W> out_flag) {
            valid.write(true);
            tag.write(out_tag);
            flag.write(out_flag);
            wait();
            valid.write(false);
        }

    public:
        sc_out<bool>                          valid;
        sc_out<sc_uint<TAG_W>>                tag;
        sc_out<sc_uint<IP_W>>                 flag;
    }; // master

    class slave {
    public:
        slave(const char* name=sc_gen_unique_name("primate_bfu_mc_out_s")) :
            valid((std::string(name)+"_valid").c_str()),
            tag((std::string(name)+"_tag").c_str()),
            flag((std::string(name)+"_flag").c_str())
        {}

        template<typename CHANNEL>
        inline void bind(CHANNEL &channel) {
            valid(channel.valid);
            tag(channel.tag);
            flag(channel.flag);
        }

        template<typename CHANNEL>
        inline void operator () (CHANNEL &channel) {
            bind(channel);
        }

        inline void read(sc_uint<TAG_W> &out_tag, sc_uint<IP_W> &out_flag) {
            do { ::wait(); } while (valid.read() == false);
            out_tag = tag;
            out_flag = flag;
        }

    public:
        sc_in<bool>                          valid;
        sc_in<sc_uint<TAG_W>>                tag;
        sc_in<sc_uint<IP_W>>                 flag;
    }; // slave

public:
    sc_signal<bool>                          valid;
    sc_signal<sc_uint<TAG_W>>                tag;
    sc_signal<sc_uint<IP_W>>                 flag;
}; // out_simple

namespace read_mc {

template<int TAG_W, int ADDR_W>
struct reqPayload_t {
    sc_uint<TAG_W>  tag;
    sc_uint<ADDR_W> addr0;
    sc_uint<ADDR_W> addr1;

    reqPayload_t(sc_uint<TAG_W> tag = 0, sc_uint<ADDR_W> addr0 = 0, sc_uint<ADDR_W> addr1 = 0) : 
        tag(tag), addr0(addr0), addr1(addr1) {}

    reqPayload_t& operator= (const reqPayload_t& val) {
        tag = val.tag;
        addr0 = val.addr0;
        addr1 = val.addr1;
        return (*this);
    }

    bool operator== (const reqPayload_t& val) const {
        return ((tag == val.tag) && (addr0 == val.addr0) && (addr1 == val.addr1));
    }

    inline friend std::ostream& operator<<(std::ostream& os, const reqPayload_t& val) {
        os << "tag = " << val.tag << "; addr0 = " << val.addr0 << "; addr1 = " << val.addr1 << std::endl;
        return os;
    }

    inline friend void sc_trace(sc_trace_file*& f, const reqPayload_t& val, std::string name) {
        sc_trace(f, val.tag, name + ".tag");
        sc_trace(f, val.addr0, name + ".addr0");
        sc_trace(f, val.addr1, name + ".addr1");
    }
};

template<int DATA_W>
struct rspPayload_t {
    sc_biguint<DATA_W> data0;
    sc_biguint<DATA_W> data1;

    rspPayload_t(sc_biguint<DATA_W> data0 = 0, sc_biguint<DATA_W> data1 = 0) : 
        data0(data0), data1(data1) {}

    rspPayload_t& operator= (const rspPayload_t& val) {
        data0 = val.data0;
        data1 = val.data1;
        return (*this);
    }

    bool operator== (const rspPayload_t& val) const {
        return ((data0 == val.data0) && (data1 == val.data1));
    }

    inline friend std::ostream& operator<<(std::ostream& os, const rspPayload_t& val) {
        os << hex << "data0 = " << val.data0 << "; data1 = " << val.data1 << dec << std::endl;
        return os;
    }

    inline friend void sc_trace(sc_trace_file*& f, const rspPayload_t& val, std::string name) {
        sc_trace(f, val.data0, name + ".data0");
        sc_trace(f, val.data1, name + ".data1");
    }
};

template<int TAG_W, int ADDR_W>
class req {
public:
    typedef typename ready_valid<reqPayload_t<TAG_W, ADDR_W>>::out master;
    typedef typename ready_valid<reqPayload_t<TAG_W, ADDR_W>>::in slave;
    typedef ready_valid<reqPayload_t<TAG_W, ADDR_W>> channel;
};

template<int DATA_W>
class rsp {
public:
    typedef typename ready_valid<rspPayload_t<DATA_W>>::out master;
    typedef typename ready_valid<rspPayload_t<DATA_W>>::in slave;
    typedef ready_valid<rspPayload_t<DATA_W>> channel;
};

}; // read_mc
}; // primate_bfu_mc



#endif
