#include "outputUnit.h"

void outputUnit::outputUnit_cmd() {
    sc_uint<1> state;
	primate_ctrl_ou::cmd_t cmd;

	// initialize handshake
	cmd_in.reset();
    state = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
    	hdr_done[i].write(0);
    	hdr_mode[i].write(0);
    }
    
#pragma hls_pipeline_init_interval 1
#pragma hls_pipeline_stall_mode flush
	wait();

	while (true) {
		if (state == 0) {
			state = 1;
			if (done) {
				hdr_done[done_tag.read()].write(0);
			}
			cmd = cmd_in.read();
		} else if (state == 1) {
			hdr_mode[cmd.ar_tag].write(cmd.ar_bits);
			if (done) {
				hdr_done[done_tag.read()].write(0);
			}
			hdr_done[cmd.ar_tag].write(1);
			state = 0;
			wait();
		}
	}
}

void outputUnit::outputUnit_req() {
	sc_uint<4> state;
	sc_uint<8> num_hdr;
	sc_uint<NUM_THREADS_LG> tag;
	primate_stream_512_4::payload_t pkt_in;

	bfu_rdreq.reset();
	state = 0;
	int count = 0;
	done = 0;
	done_tag = 0;
#pragma hls_pipeline_init_interval 1
#pragma hls_pipeline_stall_mode flush

	wait();

	while (true) {
		if (state != 0)
			cout << sc_time_stamp() << ": req state" << state << endl;
		if (state == 0) {
			done = false;
			if (pkt_buf_in.nb_read(pkt_in)) {
				if (hdr_done[pkt_in.tag].read() == 1) {
					state = 1;
				}
			}
			wait();
		} else if (state == 1) {
			tag = pkt_in.tag;
			num_hdr = hdr_mode[pkt_in.tag];
			if (num_hdr > 1) {
				state = 2;
			} else {
				state = 0;
			}
			done = true;
			done_tag = pkt_in.tag;
			bfu_rdreq.write(primate_bfu_req_t(tag, 1, 2));
		} else if (state == 2) {
			if (num_hdr < 5) {
				state = 0;
			} else {
				state = 3;
			}
			bfu_rdreq.write(primate_bfu_req_t(tag, 3, 4));
		} else if (state == 3) {
			if (num_hdr < 7) {
				state = 0;
			} else {
				state = 4;
			}
			bfu_rdreq.write(primate_bfu_req_t(tag, 4, 5));
		} else {
			state = 0;
			bfu_rdreq.write(primate_bfu_req_t(tag, 6, 7));
		}
	}

}

void outputUnit::outputUnit_rsp() {
	sc_uint<4> state;
	sc_uint<8> num_hdr;
	sc_uint<NUM_THREADS_LG> tag;
	sc_biguint<512> out_buf;
	sc_biguint<REG_WIDTH> rd_rsp_buf;

	primate_stream_512_4::payload_t pkt_in;
	primate_bfu_rsp_t hdr_data;

	bfu_rdrsp.reset();
	stream_out.reset();
	pkt_buf_in.reset();
	bfu_out.reset();
	state = 15;
#pragma hls_pipeline_init_interval 1
#pragma hls_pipeline_stall_mode flush

	wait();

	while (true) {
		cout << sc_time_stamp() << ": rsp state" << state << endl;
		if (state == 15) {
			if (pkt_buf_in.nb_read(pkt_in)) {
				state = 0;
			}
			wait();
		} else if (state == 0) {
			num_hdr = hdr_mode[pkt_in.tag];
			tag = pkt_in.tag;
			hdr_data = bfu_rdrsp.read();
			out_buf = (hdr_data.data1.range(159, 0), hdr_data.data0.range(111, 0));
			if (num_hdr < 2) {
				state = 11;
				stream_out.write(primate_io_payload_t(out_buf, tag, 50, false));
			} else {
				state = 1;
				wait();
			}
		} else if (state == 1) {
			hdr_data = bfu_rdrsp.read();
			out_buf = (hdr_data.data1.range(47, 0), hdr_data.data0.range(191, 0), out_buf.range(271, 0));
			rd_rsp_buf = hdr_data.data1;
			if (num_hdr < 3) {
				state = 11;
				stream_out.write(primate_io_payload_t(out_buf, tag, 6, false));
			} else if (num_hdr < 5) {
				state = 4;
				stream_out.write(primate_io_payload_t(out_buf, tag, 0, false));
			} else {
				state = 2;
				stream_out.write(primate_io_payload_t(out_buf, tag, 0, false));
			}
		} else if (state == 2) {
			hdr_data = bfu_rdrsp.read();
			out_buf = (hdr_data.data1.range(127, 0), hdr_data.data0.range(127, 48));
			if (num_hdr < 6) {
				state = 11;
				stream_out.write(primate_io_payload_t(out_buf, tag, 46, false));
			} else if (num_hdr < 7) {
				state = 11;
				stream_out.write(primate_io_payload_t(out_buf, tag, 38, false));
			} else {
				state = 3;
				wait();
			}
		} else if (state == 3) {
			hdr_data = bfu_rdrsp.read();
			out_buf = (hdr_data.data1.range(127, 0), hdr_data.data0.range(127, 0), out_buf.range(207, 0));
				state = 11;
			if (num_hdr < 8) {
				stream_out.write(primate_io_payload_t(out_buf, tag, 30, false));
			} else if (num_hdr < 9) {
				stream_out.write(primate_io_payload_t(out_buf, tag, 22, false));
			} else if (num_hdr < 10) {
				stream_out.write(primate_io_payload_t(out_buf, tag, 14, false));
			} else {
				stream_out.write(primate_io_payload_t(out_buf, tag, 6, false));
			}
		} else if (state == 4) {
			out_buf = rd_rsp_buf;
			state = 11;
			if (num_hdr < 4) {
				stream_out.write(primate_io_payload_t(out_buf, tag, 62, false));
			} else {
				stream_out.write(primate_io_payload_t(out_buf, tag, 54, false));
			}
		} else if (state == 11) {
			pkt_in = pkt_buf_in.read();
			if (pkt_in.last) {
				state = 12;
			}
			stream_out.write(pkt_in);
		} else {
			state = 15;
			bfu_out.write(tag, 0);
		}
	}

}