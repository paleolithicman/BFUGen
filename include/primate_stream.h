#ifndef _PRIMATE_STREAM_
#define _PRIMATE_STREAM_

#include <systemc.h>
#include <string>

template<typename Cfg>
class primate_stream
{
public:
    enum {
        DATA_WIDTH = Cfg::dataWidth,
        NUM_THREADS_LG = Cfg::numThreadsLG,
        EMPTY_WIDTH = (DATA_WIDTH >> 3),
    };
    typedef typename Cfg::Data_t Data;

    struct payload_t {
        sc_uint<NUM_THREADS_LG>  tag;
        Data                     data;
        sc_uint<EMPTY_WIDTH>     empty;
        bool                     last;

        payload_t(Data data = 0, sc_uint<NUM_THREADS_LG> tag = 0, sc_uint<EMPTY_WIDTH> empty = 0, bool last = false) : 
            data(data), tag(tag), empty(empty), last(last) {}

        inline bool is_last() {
            return last;
        }

        payload_t& operator= (const payload_t& val) {
            data = val.data;
            tag = val.tag;
            empty = val.empty;
            last = val.last;
            return (*this);
        }

        bool operator== (const payload_t& val) const {
            return ((data == val.data) && (tag == val.tag) &&
                (empty == val.empty) && (last == val.last));
        }

        inline friend std::ostream& operator<<(std::ostream& os, const payload_t& val) {
            os << hex << "data = " << val.data << dec << "; tag = " << val.tag << 
                "; empty = " << val.empty << "; last = " << val.last << std::endl;
            return os;
        }

        inline friend void sc_trace(sc_trace_file*& f, const payload_t& val, std::string name) {
            sc_trace(f, val.data, name + ".data");
            sc_trace(f, val.tag, name + ".tag");
            sc_trace(f, val.empty, name + ".empty");
            sc_trace(f, val.last, name + ".last");
        }
    };

    class in {
    public:
        in(const char* name=sc_gen_unique_name("primate_stream_in")) :
            payload((std::string(name)+"_payload").c_str()),
            valid((std::string(name)+"_valid").c_str()),
            ready((std::string(name)+"_ready").c_str())
        {}

        template<typename CHANNEL>
        inline void bind(CHANNEL &channel) {
            valid(channel.valid);
            payload(channel.payload);
            ready(channel.ready);
        }

        template<typename CHANNEL>
        inline void operator () (CHANNEL &channel) {
            bind(channel);
        }

        inline bool nb_can_read() {
            return valid.read();
        }

        inline bool nb_read(payload_t &data) {
            data = payload;
            // ready = true;
            if (valid.read()) {
                return true;
            } else {
                return false;
            }
        }

        inline void reset() {
            ready = false;
        }

        inline payload_t read() {
            ready = true;
            do { ::wait(); } while (valid.read() == false);
            ready = false;
            return payload.read();
        }

        inline operator payload_t () {
            return read();
        }


    public:
        sc_in<payload_t> payload;
        sc_out<bool>     ready;
        sc_in<bool>      valid;
    }; // class in

    class out {
    public:
        out(const char* name=sc_gen_unique_name("primate_stream_out")) :
            payload((std::string(name)+"_payload").c_str()),
            valid((std::string(name)+"_valid").c_str()),
            ready((std::string(name)+"_ready").c_str())
        {}

        template<typename CHANNEL>
        inline void bind(CHANNEL &channel) {
            valid(channel.valid);
            payload(channel.payload);
            ready(channel.ready);
        }

        template<typename CHANNEL>
        inline void operator () (CHANNEL &channel) {
            bind(channel);
        }

        inline bool nb_can_write() {
            return valid.read() == false || ready.read() == true;
        }

        inline bool nb_write(const payload_t &data) {
            payload = data;
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

        inline void write(const payload_t &data) {
            payload = data;
            valid = true;
            do { ::wait(); } while (ready.read() == false);
            valid = false;
        }

        inline const payload_t& operator = (const payload_t &data) {
            write(data);
            return data;
        }

    public:
        sc_out<payload_t> payload;
        sc_in<bool>       ready;
        sc_out<bool>      valid;
    }; // class out

public:
    sc_signal<payload_t> payload;
    sc_signal<bool>      ready;
    sc_signal<bool>      valid;
}; // class primate_stream

template <int DW, int NTLG>
struct cfg_biguint {

  typedef sc_biguint<DW> Data_t;

  enum {
    dataWidth = DW,
    numThreadsLG = NTLG,
  };

};
typedef primate_stream<cfg_biguint<512, 4>> primate_stream_512_4;


#endif