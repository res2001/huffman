//============================================================================
// Name        : huffman.cpp
// Author      : res
// Version     :
// Copyright   :
// Description :
//============================================================================
/*
Implement Huffman compression algorithm. Two command line utilities:

1. 'encode' -- encodes any input file using Huffman algorithm:

> encode <input-file> <output-file>

2. 'decode' -- decodes input file to the original one:

> decode <input-file> <output-file>

Requirements:

* Pure C or C++ (up to C++14). No any extensions, only standard language
  features. No assembly code.
* Makefile as a build system. Don't use CMake! Executing 'make' should
  build 'encode' and 'decode' binaries.
* Only standard libraries included with the compiler, i.e. C standard library or
  C++ standard library (STL). No Boost! No Qt!
* Encoding and decoding should work as fast as possible.
* Binaries should not crash (e.g. Segmentation Fault) on any inputs.
*/
#include "huffman.h"

bool write_stream(uint8_t * wbuf, size_t & lenwbuf, ostream & out) {
	if(lenwbuf == 0) return true;
	out.write(reinterpret_cast<const char*>(wbuf), lenwbuf);
	if(!out.good()) {
		return false;
	}
	lenwbuf = 0;
	return true;
}

int main(int argc, const char *argv[]) {
	if(argc < 3)
	{
		cerr << argv[0] << " <input-file> <output-file>" << endl;
		return 1;
	}
	ifstream infile(argv[1], ios_base::binary);
	if(!infile.is_open()) {
		cerr << "Error open file: " << argv[1] << endl;
		return 2;
	}
	infile.seekg(0, ios_base::end);
	if((size_t)infile.tellg() == 0) {
		cerr << "Input file is zero length." << endl;
		return 3;
	}
	infile.seekg(0);
	ofstream outfile(argv[2], ios_base::binary | ios_base::trunc);
	if(!outfile.is_open()) {
		cerr << "File not found: " << argv[1] << endl;
		return 4;
	}
	htree_t htree;
#ifdef HUFFMANDECODE
	extern void decode(htree_t & ht, istream & in, ostream & out);
	htree.htree_generate_from_enc(infile);
	htree.print_tree();
	decode(htree, infile, outfile);
#else
	extern void encode(htree_t & ht, istream & in, ostream & out);
	htree.htree_generate_from_source(infile);
	htree.print_tree();
	infile.clear();
	infile.seekg(0);
	encode(htree, infile, outfile);
#endif
	return 0;
}

