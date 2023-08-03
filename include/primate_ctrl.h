#ifndef _PRIMATE_CTRL_
#define _PRIMATE_CTRL_

#include <systemc.h>
#include <string>

template<int TAG_W, int OP_W, int ADDR_W, int DATA_W>
class primate_ctrl
{
public:
    struct cmd_t {
        sc_uint<TAG_W>          ar_tag;
        sc_uint<OP_W>           ar_opcode;
        sc_uint<ADDR_W>         ar_rd;
        sc_uint<DATA_W>         ar_bits;
        sc_uint<DATA_W>         ar_imm;

        cmd_t(sc_uint<TAG_W> ar_tag = 0, sc_uint<OP_W> ar_opcode = 0, sc_uint<ADDR_W> ar_rd = 0, 
            sc_uint<DATA_W> ar_bits = 0, sc_uint<DATA_W> ar_imm = 0) : 
            ar_tag(ar_tag), ar_opcode(ar_opcode), ar_rd(ar_rd), ar_bits(ar_bits), ar_imm(ar_imm) {}

        cmd_t& operator= (const cmd_t& val) {
            ar_tag = val.ar_tag;
            ar_opcode = val.ar_opcode;
            ar_rd = val.ar_rd;
            ar_bits = val.ar_bits;
            ar_imm = val.ar_imm;
            return (*this);
        }

        bool operator== (const cmd_t& val) const {
            return ((ar_tag == val.ar_tag) && (ar_opcode == val.ar_opcode) &&
                (ar_rd == val.ar_rd) && (ar_bits == val.ar_bits) && (ar_imm == val.ar_imm));
        }

        inline friend std::ostream& operator<<(std::ostream& os, const cmd_t& val) {
            os << "tag = " << val.ar_tag << hex << "; opcode = " << val.ar_opcode << 
                "; rd = " << val.ar_rd << "; bits = " << val.ar_bits <<
                "; imm = " << val.ar_imm << dec << std::endl;
            return os;
        }

        inline friend void sc_trace(sc_trace_file*& f, const cmd_t& val, std::string name) {
            sc_trace(f, val.ar_tag, name + ".ar_tag");
            sc_trace(f, val.ar_opcode, name + ".ar_opcode");
            sc_trace(f, val.ar_rd, name + ".ar_rd");
            sc_trace(f, val.ar_bits, name + ".ar_bits");
            sc_trace(f, val.ar_imm, name + ".ar_imm");
        }
    };

    class slave {
    public:
        slave(const char* name=sc_gen_unique_name("primate_ctrl_slave")) :
            cmd((std::string(name)+"_cmd").c_str()),
            valid((std::string(name)+"_valid").c_str()),
            ready((std::string(name)+"_ready").c_str())
        {}

        template<typename CHANNEL>
        inline void bind(CHANNEL &channel) {
            valid(channel.valid);
            cmd(channel.cmd);
            ready(channel.ready);
        }

        template<typename CHANNEL>
        inline void operator () (CHANNEL &channel) {
            bind(channel);
        }

        inline bool nb_can_read() {
            return valid.read();
        }

        inline bool nb_read(cmd_t &data) {
            data = cmd;
            ready = true;
            if (valid.read()) {
                return true;
            } else {
                return false;
            }
        }

        inline void reset() {
            ready = false;
        }

        inline cmd_t read() {
            ready = true;
            do { ::wait(); } while (valid.read() == false);
            ready = false;
            return cmd.read();
        }

        inline operator cmd_t () {
            return read();
        }


    protected:
        sc_in<cmd_t> cmd;
        sc_out<bool> ready;
        sc_in<bool>  valid;
    }; // class in

    class master {
    public:
        master(const char* name=sc_gen_unique_name("primate_ctrl_master")) :
            cmd((std::string(name)+"_cmd").c_str()),
            valid((std::string(name)+"_valid").c_str()),
            ready((std::string(name)+"_ready").c_str())
        {}

        template<typename CHANNEL>
        inline void bind(CHANNEL &channel) {
            valid(channel.valid);
            cmd(channel.cmd);
            ready(channel.ready);
        }

        template<typename CHANNEL>
        inline void operator () (CHANNEL &channel) {
            bind(channel);
        }

        inline bool nb_can_write() {
            return valid.read() == false || ready.read() == true;
        }

        inline bool nb_write(const cmd_t &data) {
            cmd = data;
            valid = true;
            if (ready.read() == true) {
                return true;
            } else {
                return false;
            }
        }

        inline void reset() {
            valid = false;
        }

        inline void write(const cmd_t &data) {
            cmd = data;
            valid = true;
            do { ::wait(); } while (ready.read() == false);
            valid = false;
        }

        inline const cmd_t& operator = (const cmd_t &data) {
            write(data);
            return data;
        }

    protected:
        sc_out<cmd_t> cmd;
        sc_in<bool>   ready;
        sc_out<bool>  valid;
    }; // class out

public:
    sc_signal<cmd_t> cmd;
    sc_signal<bool>  ready;
    sc_signal<bool>  valid;
}; // class primate_ctrl


#endif