/*
 * huffman.h
 *
 *  Created on: 19 апр. 2018 г.
 *      Author: res
 */

#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cassert>

using namespace std;

bool write_stream(uint8_t * wbuf, size_t & lenwbuf, ostream & out);

#define ALPHABET_COUNT			256
#define INVALID_IDX				-1
#define MAX_DEPTH_TREE			32
#define BIT_IN_BYTE				8
#define MAGIC					"HF"

#pragma pack(push, 1)

struct enc_header_t final {
	char magic[sizeof(MAGIC) - 1] = { MAGIC[0], MAGIC[1] };
	uint64_t srclen;
	uint16_t dictlen;
	explicit enc_header_t(uint64_t sl, uint16_t dl): srclen(sl), dictlen(dl) {};
	enc_header_t() = delete;
	enc_header_t(const enc_header_t &) = default;
	enc_header_t & operator=(const enc_header_t &) = default;
};

struct dict_item_t final {
	uint8_t sym;
	uint32_t count;
	explicit dict_item_t(uint8_t s, uint32_t c) : sym(s), count(c) {};
	dict_item_t() = delete;
	dict_item_t(const dict_item_t &) = default;
	dict_item_t & operator=(const dict_item_t &) = default;
};

#pragma pack(pop)

enum node_type_e {
	NODE = 0,
	ROOT,
	LEAF
};

class node_t {
private:
	uint32_t count;
	int16_t next;					// for list - indices of next elements
	int16_t parent, left, right;	// for tree - indices of the corresponding elements of the array
	int16_t symbol;
	uint32_t weight;
	uint8_t wbitlen;
	enum node_type_e type;
public:
	explicit node_t() : count(0), next(INVALID_IDX), parent(INVALID_IDX), left(INVALID_IDX), right(INVALID_IDX),
						symbol(INVALID_IDX), weight(0), wbitlen(0), type(NODE) {};
	node_t(const node_t & node) = default;
	node_t & operator=(const node_t & node) = default;

	uint32_t get_count() const { return count; }
	uint32_t inc_count() { return ++count; }
	void set_count(uint32_t n) { count = n; }

	uint16_t get_next_idx() const { return next; }
	void set_next_idx(uint16_t n) { next = n; }

	uint16_t get_idx() const { return symbol; }
	void set_idx(uint16_t ch) { symbol = ch; }
	char get_symbol() const { return static_cast<char>(symbol); }

	enum node_type_e get_type() const { return type; }
	void set_type(enum node_type_e t) { type = t; }

	uint16_t get_parent_idx() const { return parent; }
	void set_parent_idx(uint16_t n) { parent = n; }

	uint16_t get_left_idx() const { return left; }
	void set_left_idx(uint16_t n) { left = n; }

	uint16_t get_right_idx() const { return right; }
	void set_right_idx(uint16_t n) { right = n; }

	uint32_t get_weight() const { return weight; }
	uint8_t get_wbitlen() const { return wbitlen; }
	void set_weight(uint32_t w, uint8_t wlen) { weight = w; wbitlen = wlen; }

	static void print_node(node_t & node, uint16_t level);
};

using array_node_t = array<node_t, ALPHABET_COUNT * 2>;

class list_node_t {
private:
	int16_t top, bottom;		// indices begin and end elements of list
	int16_t new_node_idx;
	uint16_t count;				// number of elements in the list
	array_node_t & anode;
	bool is_valid_idx(int16_t n) const { return n >= 0 && (size_t)n < anode.size(); }
	bool is_valid_node(node_t & n) const { return is_valid_idx(n.get_idx()); }
public:
	list_node_t() = delete;
	explicit list_node_t(array_node_t & an);
	list_node_t(const list_node_t & l) = delete;
	list_node_t & operator=(const list_node_t & l) = delete;

	// List header

	int16_t get_first_idx() const { return top; }
	node_t & get_first() const {
		assert(is_valid_idx(top) && "Top is out of range.");
		return anode[top];
	}
	int16_t get_last_idx() const { return bottom; }
	node_t & get_last() const {
		assert(is_valid_idx(bottom) && "Bottom is out of range.");
		return anode[bottom];
	}
	node_t & get_next(node_t & cur) const {
		assert(is_valid_node(cur) && "Next is out of range.");
		return anode[cur.get_next_idx()];
	}
	uint16_t get_count() const { return count; }

	// List manipulation
	void build_sorted_list();
	int16_t get_new_empty_node_idx();
	node_t & remove_first();
	void insert_sorted(node_t & new_node);

	// Node array access
	node_t & operator[](size_t idx) { return anode[idx]; }
	const node_t & operator[](size_t idx) const { return anode[idx]; }
private:
	void set_first_idx(int16_t n) { top = n; }
	void set_last_idx(int16_t n) { bottom = n; }
	void set_count(uint16_t n) { count = n; }
	void set_empty_node_idx(int16_t n) { new_node_idx = n; }
	void push_first(node_t & new_node);
	void push_back(node_t & new_node);
};

class htree_t {
private:
	array_node_t anode;
	uint16_t root_idx;
	uint64_t source_length;
	uint16_t count_symbol_in_array;
public:
	explicit htree_t() : anode(), root_idx(INVALID_IDX), source_length(0), count_symbol_in_array(0) {};
	htree_t(const htree_t & l) = delete;
	htree_t & operator=(const htree_t & l) = delete;

	// Node array access
	const node_t & operator[](size_t idx) const { return anode[idx]; }

	bool is_valid_idx(int16_t n) const { return n >= 0 && (size_t)n < anode.size(); }
	bool is_valid_tree() const { return is_valid_idx(root_idx); }

	uint16_t get_root_idx() { return root_idx; }
	node_t & get_root() {
		assert(is_valid_idx(root_idx) && "Root indices is out of range.");
		return anode[root_idx];
	}

	node_t & get_left(node_t & node) {
		assert(is_valid_idx(node.get_left_idx()) && "Left indices is out of range.");
		return anode[node.get_left_idx()];
	}

	node_t & get_right(node_t & node) {
		assert(is_valid_idx(node.get_right_idx()) && "Right indices is out of range.");
		return anode[node.get_right_idx()];
	}

	uint64_t get_source_length() const { return source_length; }
	void set_source_length(uint64_t n) { source_length = n; }

	uint16_t get_symdict_length() const { return count_symbol_in_array; }
	void set_symdict_length(uint16_t n) { count_symbol_in_array = n; }

	void htree_generate_from_source(istream & in);
	void htree_generate_from_enc(istream & in);

	using htree_traversal_func_t = void (*) (node_t & node, uint16_t level);
	void htree_traversal(htree_traversal_func_t func) { subtree_traversal(get_root(), 0, func); }

	void print_tree() {
		cout << "HTree:" << endl;
		if(is_valid_idx(root_idx))
			htree_traversal(node_t::print_node);
	}

	template<typename U>
	void array_traversal(U func) { for_each(anode.begin(), anode.end(), func); }
private:
	void htree_make(list_node_t & list);
	void subtree_traversal(node_t & node, uint16_t level, htree_traversal_func_t func);
	void htree_weight_calc() { subtree_weight_calc(get_root(), 0, 0); }
	void subtree_weight_calc(node_t & node, uint32_t w, uint8_t wlen);
};
#endif /* HUFFMAN_H_ */
