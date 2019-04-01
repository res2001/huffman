/*
 * htree.cpp
 *
 *  Created on: 19 апр. 2018 г.
 *      Author: res
 *  Description: Huffman tree generate.
 */
#include <cstring>
#include "huffman.h"

void htree_t::htree_make(list_node_t & list) {
	if(list.get_count() == 0)
		return;
	if(list.get_count() == 1) {
		node_t & root = list.remove_first();
		root_idx = root.get_idx();
		root.set_weight(0, 1);
		return;
	}
	while(list.get_count() > 1) {
		int32_t nidx = list.get_new_empty_node_idx();
		node_t & n = list[nidx];
		node_t & l = list.remove_first();
		node_t & r = list.remove_first();
		n.set_count(l.get_count() + r.get_count());
		n.set_left_idx(l.get_idx());
		n.set_right_idx(r.get_idx());
		l.set_parent_idx(nidx);
		r.set_parent_idx(nidx);
		list.insert_sorted(n);
	}
	node_t & root = list.remove_first();
	root.set_type(ROOT);
	root_idx = root.get_idx();
	htree_weight_calc();
}

#define BUFLEN		512
void htree_t::htree_generate_from_source(istream & in) {
	if(!in.good()) return;
	uint64_t lensrc = 0;
	{
		uint8_t bufch[BUFLEN];
		size_t read_size;
		while( in.good() ) {
			in.read( reinterpret_cast<char*>(& bufch[0]), sizeof(bufch) );
			read_size = in.gcount();
			lensrc += read_size;
			for(size_t i = 0; i < read_size; ++i)
				anode[bufch[i]].inc_count();
		}
		if(!in.eof()) {
			cerr << "Error read input file." << endl;
		}
	}
	if(lensrc) {
		set_source_length(lensrc);
		list_node_t list(anode);
		list.build_sorted_list();
		set_symdict_length(list.get_count());
		htree_make(list);
	}
}
#undef BUFLEN

void htree_t::htree_generate_from_enc(istream & in) {
	if(!in.good()) return;
	{
		// Read header of the encoded file
		enc_header_t header(0, 0);
		in.read( reinterpret_cast<char*>(&header), sizeof(header) );
		if(in.gcount() != sizeof(header)) {
			cerr << "Error read input file." << endl;
			return;
		}
		if(memcmp(&header.magic[0], MAGIC, sizeof(MAGIC) - 1) != 0) {
			cerr << "Incorrect format of the input file" << endl;
			return;
		}
		if(header.srclen == 0 || header.dictlen == 0) {
			cerr << "Invalid header in input file." << endl;
			return;
		}
		set_source_length(header.srclen);
		set_symdict_length(header.dictlen);
	}
	{
		// Read symbol dictionary
		dict_item_t dict_item(0, 0);
		const uint16_t dictlen = get_symdict_length();
		for(uint16_t i = 0; i < dictlen && in.good(); ++i) {
			in.read( reinterpret_cast<char*>(&dict_item), sizeof(dict_item) );
			if(in.gcount() != sizeof(dict_item)) {
				cerr << "Error reading dictionary from input file." << endl;
				return;
			}
			anode[dict_item.sym].set_count(dict_item.count);
		}
	}
	list_node_t list(anode);
	list.build_sorted_list();
	htree_make(list);
}

void htree_t::subtree_traversal(node_t & node, uint16_t level, htree_t::htree_traversal_func_t func) {
	func(node, level);
	++level;
	if( is_valid_idx(node.get_left_idx()) )
			subtree_traversal(anode[node.get_left_idx()], level, func);
	if( is_valid_idx(node.get_right_idx()) )
			subtree_traversal(anode[node.get_right_idx()], level, func);
}

void htree_t::subtree_weight_calc(node_t & node, uint32_t w, uint8_t wlen) {
	assert(wlen < MAX_DEPTH_TREE && "Invalid weight length.");
	node.set_weight(w, wlen);
	const uint8_t wlen_new = wlen + 1;
	if( is_valid_idx(node.get_left_idx()) )
		subtree_weight_calc(anode[node.get_left_idx()], w, wlen_new);
	if( is_valid_idx(node.get_right_idx()) )
		subtree_weight_calc(anode[node.get_right_idx()], w | (1 << wlen), wlen_new);
}
