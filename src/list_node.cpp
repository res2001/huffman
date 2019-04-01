/*
 * list_node.cpp
 *
 *  Created on: 21 апр. 2018 г.
 *      Author: res
 */
#include "huffman.h"

void node_t::print_node(node_t & node, uint16_t level) {
	for(; level > 0; --level) {
		cout << "|  ";
	}
	switch(node.get_type())
	{
	case NODE:
		cout << "-N ";
		break;
	case ROOT:
		cout << "-R ";
		break;
	case LEAF:
		cout << "-L ";
		break;
	}
	cout << node.get_idx();
	if(node.get_idx() < ALPHABET_COUNT && isprint(node.get_symbol()))
		cout << "('" << node.get_symbol() << "')";
	cout << " = " << node.get_count();
	if(node.get_type() != ROOT) {
		cout << " p:"  << node.get_parent_idx();
	}
	if(node.get_type() != LEAF) {
		cout << " l:" << node.get_left_idx() << " r:" << node.get_right_idx();
	}
	const ios_base::fmtflags ff = cout.flags();
	cout.setf(cout.hex, cout.basefield);
	cout.setf(cout.showbase);
	cout << " w:" << node.get_weight();
	cout.flags(ff);
	cout << " wlen:" << static_cast<uint16_t>(node.get_wbitlen());
	cout << endl;
}

list_node_t::list_node_t(array_node_t & an) :
	top(INVALID_IDX), bottom(INVALID_IDX), new_node_idx(ALPHABET_COUNT), count(0), anode(an)
{
	uint16_t idx = 0;
	for_each(anode.begin(), anode.begin() + anode.size() / 2, [&idx](node_t & node) {
		node.set_idx(idx);
		node.set_type(LEAF);
		++idx;
	});
	for_each(anode.begin() + anode.size() / 2, anode.end(), [&idx](node_t & node) {
		node.set_idx(idx);
		++idx;
	});
//	for_each(anode.begin(), anode.begin() + anode.size() / 2,
//				[](const node_t & node){ if(node.get_count() > 0) cout << node.get_symbol() << ":\t" << node.get_count() << endl; });
}

int16_t list_node_t::get_new_empty_node_idx() {
	assert(new_node_idx >= 0 && new_node_idx < ALPHABET_COUNT * 2 && "There are no empty nodes");
	return new_node_idx++;
}

void list_node_t::build_sorted_list() {
	for_each(anode.begin(), anode.begin() + anode.size() / 2,
				[this](node_t & node) { if(node.get_count() > 0) this->insert_sorted(node);	});
	// debug print
	cout << "top = " << top << "\tbottom = " << bottom << endl;
	cout << "ch\tcode\tnext\tcount" << endl;
	for_each(anode.begin(), anode.begin() + anode.size() / 2,
				[](const node_t & node){ if(node.get_count() > 0) cout << node.get_symbol() << "\t" << node.get_idx() << "\t" << node.get_next_idx() << "\t" << node.get_count() << endl; });
}

node_t & list_node_t::remove_first() {
	assert(is_valid_idx(top) && "Top is out of range.");
	node_t & node = anode[top];
	top = node.get_next_idx();
	--count;
	if(!count) bottom = INVALID_IDX;
	node.set_next_idx(INVALID_IDX);
	return node;
}

void list_node_t::push_first(node_t & new_node) {
	assert(is_valid_node(new_node) && "New node idx is out of range.");
	new_node.set_next_idx(top);
	top = new_node.get_idx();
	if(!count) bottom = top;
	++count;
}

void list_node_t::push_back(node_t & new_node) {
	assert(is_valid_node(new_node) && "New node idx is out of range.");
	new_node.set_next_idx(INVALID_IDX);
	anode[bottom].set_next_idx(new_node.get_idx());
	bottom = new_node.get_idx();
	if(!count) top = bottom;
	++count;
}

void list_node_t::insert_sorted(node_t & new_node) {
	assert(is_valid_node(new_node) && "New node idx is out of range.");
	if(count == 0 || anode[top].get_count() >= new_node.get_count())
	{
		push_first(new_node);
		return;
	}
	int16_t before_idx = anode[top].get_next_idx();
	int16_t prev_idx = top;
	while(is_valid_idx(before_idx))
	{
		if(anode[before_idx].get_count() >= new_node.get_count())
		{
			new_node.set_next_idx(before_idx);
			anode[prev_idx].set_next_idx(new_node.get_idx());
			++count;
			return;
		}
		prev_idx = before_idx;
		before_idx = anode[before_idx].get_next_idx();
	}
	if(!is_valid_idx(before_idx)) {
		push_back(new_node);
		return;
	}
}
