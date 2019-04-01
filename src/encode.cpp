/*
 * encode.cpp
 *
 *  Created on: 19 апр. 2018 г.
 *      Author: res
 */
#include <cstring>
#include "huffman.h"

#define BUFLEN		512

static bool write_enc_buf(uint8_t * wbuf, size_t & lenwbuf, ostream & out, uint32_t & encbyte, uint8_t & enclen) {
	while(enclen >= BIT_IN_BYTE) {
		wbuf[lenwbuf] = static_cast<uint8_t>(encbyte);
		encbyte >>= BIT_IN_BYTE;
		enclen -= BIT_IN_BYTE;
		++lenwbuf;
		if(lenwbuf == BUFLEN) {
			if(! write_stream(wbuf, lenwbuf, out)) {
				cerr << "Encoder write error." << endl;
				return false;
			}
			lenwbuf = 0;
		}
	}
	return true;
}

void encode(htree_t & ht, istream & in, ostream & out) {
	if(!in.good() || !out.good() || !ht.is_valid_tree())
		return;
	{
		// write header
		enc_header_t header(ht.get_source_length(), ht.get_symdict_length());
		out.write(reinterpret_cast<const char*>(&header), sizeof(header));
	}
	// write symbol dictionary
	ht.array_traversal([&out](const node_t & node) {
		dict_item_t item(node.get_symbol(), node.get_count());
		if(node.get_idx() < ALPHABET_COUNT && item.count > 0) {
			out.write( reinterpret_cast<const char*>(&item), sizeof(item) );
		}
	});
	// write encode data
	uint8_t rbuf[BUFLEN];
	uint8_t wbuf[BUFLEN];
	size_t read_size;
	size_t write_size = 0;
	uint32_t encbyte = 0;
	uint8_t enclen = 0;
	while( in.good() ) {
		in.read( reinterpret_cast<char*>(& rbuf[0]), sizeof(rbuf) );
		read_size = in.gcount();
		for(size_t i = 0; i < read_size; ++i) {
			const node_t & node = ht[rbuf[i]];
			encbyte |= node.get_weight() << enclen;
			enclen += node.get_wbitlen();
			if(enclen >= BIT_IN_BYTE) {
				if(!write_enc_buf(& wbuf[0], write_size, out, encbyte, enclen)) {
					return;
				}
			}
		}
	}
	// write the rest encode data
	if(enclen > 0) {
		enclen = 8;
		write_enc_buf(& wbuf[0], write_size, out, encbyte, enclen);
	}
	if(write_size > 0)
		if(!write_stream(& wbuf[0], write_size, out))
			cerr << "Encoder write error." << endl;
}
#undef BUFLEN
