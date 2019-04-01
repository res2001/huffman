/*
 * decode.cpp
 *
 *  Created on: 23 апр. 2018 г.
 *      Author: res
 */

#include "huffman.h"

#define BUFLEN		512

void decode(htree_t & ht, istream & in, ostream & out) {
	if(!in.good() || !out.good() || !ht.is_valid_tree())
		return;
	uint8_t rbuf[BUFLEN];
	uint8_t wbuf[BUFLEN];
	const uint64_t readsym_count_max = ht.get_source_length();
	uint64_t readsym_count = 0;
	size_t read_size;
	size_t write_size = 0;
	uint8_t curch = 0;
	uint16_t node_idx = ht.get_root_idx();
	while(in.good()) {
		in.read( reinterpret_cast<char*>(&rbuf[0]), sizeof(rbuf) );
		read_size = in.gcount();
		for(size_t i = 0; i < read_size; ++i) {
			curch = rbuf[i];
			for(uint16_t j = 1; j < UINT8_MAX && readsym_count < readsym_count_max; j <<= 1) {
				{
					const node_t & node = ht[node_idx];
					if(node.get_type() != LEAF) {
						if( (curch & j) ) {
							// current bit equals 1
							node_idx = node.get_right_idx();
						} else {
							// current bit equals 0
							node_idx = node.get_left_idx();
						}
					}
				}
				{
					const node_t & node = ht[node_idx];
					if(node.get_type() == LEAF) {
						wbuf[write_size++] = node.get_symbol();
						++readsym_count;
						node_idx = ht.get_root_idx();
						if(write_size == sizeof(wbuf)) {
							if(!write_stream(& wbuf[0], write_size, out)) {
								cerr << "Dencoder write error." << endl;
								return;
							}
						}
					}
				}
			}
		}
	}
	if(write_size > 0)
		if(!write_stream(& wbuf[0], write_size, out))
			cerr << "Dencoder write error." << endl;
}
