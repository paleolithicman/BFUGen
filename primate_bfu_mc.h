#ifndef _PRIMATE_BFU_MC_
#define _PRIMATE_BFU_MC_

#include <systemc.h>
#include <string>

template<int TAG_W, int IP_W, int ADDR_W, int DATA_W>
class primate_bfu_mc
{
public:
    class write_mc {
    public:
        class master {
        public:
            master(const char* name=sc_gen_unique_name("primate_bfu_mc_wr_m")) :
                ready((std::string(name)+"_ready").c_str()),
                valid((std::string(name)+"_valid").c_str()),
                tag((std::string(name)+"_tag").c_str()),
                flag((std::string(name)+"_flag").c_str()),
                wen((std::string(name)+"_wen").c_str(), 2),
                addr((std::string(name)+"_addr").c_str(), 2),
                data((std::string(name)+"_data").c_str(), 2)
            {}

            template<typename CHANNEL>
            inline void bind(CHANNEL &channel) {
                ready(channel.ready);
                valid(channel.valid);
                tag(channel.tag);
                flag(channel.flag);
                wen(channel.wen);
                addr(channel.addr);
                data(channel.data);
            }

            template<typename CHANNEL>
            inline void operator () (CHANNEL &channel) {
                bind(channel);
            }

            inline void reset() {
                valid = false;
                tag.write(0);
                flag.write(0);
                wen[0] = false;
                wen[1] = false;
            }

            inline void write_last(sc_uint<TAG_W> out_tag, sc_uint<IP_W> out_flag) {
                valid = true;
                tag.write(out_tag);
                flag.write(out_flag);
                wen[0] = false;
                wen[1] = false;
                wait();
                valid = false;
            }

            inline void write(sc_uint<TAG_W> out_tag, sc_uint<IP_W> out_flag, sc_uint<ADDR_W> out_addr,
             sc_biguint<DATA_W> out_data, bool is_last = false) {
                valid.write(is_last);
                tag.write(out_tag);
                flag.write(out_flag);
                wen[0] = true;
                addr[0].write(out_addr);
                data[0].write(out_data);
                wen[1] = false;
                do { ::wait(); } while (ready.read() == false);
                valid = false;
                wen[0] = false;
            }

            inline void write(sc_uint<TAG_W> out_tag, sc_uint<IP_W> out_flag, sc_uint<ADDR_W> out_addr0,
             sc_biguint<DATA_W> out_data0, sc_uint<ADDR_W> out_addr1, sc_biguint<DATA_W> out_data1, bool is_last = false) {
                valid.write(is_last);
                tag.write(out_tag);
                flag.write(out_flag);
                wen[0] = true;
                addr[0].write(out_addr0);
                data[0].write(out_data0);
                wen[1] = true;
                addr[1].write(out_addr1);
                data[1].write(out_data1);
                do { ::wait(); } while (ready.read() == false);
                valid = false;
                wen[0] = false;
                wen[1] = false;
            }

        protected:
            sc_in<bool>                           ready;
            sc_out<bool>                          valid;
            sc_out<sc_uint<TAG_W>>                tag;
            sc_out<sc_uint<IP_W>>                 flag;
            sc_vector<sc_out<bool>>               wen;
            sc_vector<sc_out<sc_uint<ADDR_W>>>    addr;
            sc_vector<sc_out<sc_biguint<DATA_W>>> data;
        }; // class master

    public:
        class slave {
        public:
            slave(const char* name=sc_gen_unique_name("primate_bfu_mc_wr_s")) :
                ready((std::string(name)+"_ready").c_str()),
                valid((std::string(name)+"_valid").c_str()),
                tag((std::string(name)+"_tag").c_str()),
                flag((std::string(name)+"_flag").c_str()),
                wen((std::string(name)+"_wen").c_str(), 2),
                addr((std::string(name)+"_addr").c_str(), 2),
                data((std::string(name)+"_data").c_str(), 2)
            {}

            template<typename CHANNEL>
            inline void bind(CHANNEL &channel) {
                ready(channel.ready);
                valid(channel.valid);
                tag(channel.tag);
                flag(channel.flag);
                wen(channel.wen);
                addr(channel.addr);
                data(channel.data);
            }

            template<typename CHANNEL>
            inline void operator () (CHANNEL &channel) {
                bind(channel);
            }

            inline void reset() {
                ready = false;
            }

        public:
            sc_out<bool>                         ready;
            sc_in<bool>                          valid;
            sc_in<sc_uint<TAG_W>>                tag;
            sc_in<sc_uint<IP_W>>                 flag;
            sc_vector<sc_in<bool>>               wen;
            sc_vector<sc_in<sc_uint<ADDR_W>>>    addr;
            sc_vector<sc_in<sc_biguint<DATA_W>>> data;
        }; // class slave

    public:
        sc_signal<bool>                          ready;
        sc_signal<bool>                          valid;
        sc_signal<sc_uint<TAG_W>>                tag;
        sc_signal<sc_uint<IP_W>>                 flag;
        sc_vector<sc_signal<bool>>               wen{"primate_bfu_mc_wr_wen", 2};
        sc_vector<sc_signal<sc_uint<ADDR_W>>>    addr{"primate_bfu_mc_wr_addr", 2};
        sc_vector<sc_signal<sc_biguint<DATA_W>>> data{"primate_bfu_mc_wr_data", 2};
    }; // write_mc

}; // class primate_bfu_mc

#endif