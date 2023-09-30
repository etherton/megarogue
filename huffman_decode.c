#ifndef __clang__
#include "stdint.h"
#endif

#include "huffman_decode.h"

int huffman_generate_tree(int8_t outTree[],const uint8_t *widths,int numCodes) {
	struct code_t { uint8_t width, symbol; uint16_t code; };
	assert(numCodes<=24);
	struct code_t codes[24];
	uint8_t next[24];
	uint8_t first[12] = {255,255,255,255,255,255,255,255,255,255,255,255};

	// Generate a list sorted by width (in reverse order so final list is in correct direction)
	for (int i=numCodes; i--;) {
		// assert(widths[i]<12);
		next[i] = first[widths[i]];
		first[widths[i]] = i;
	}

	int stored = 0;
	// Generate code/symbol array, leaving out unused symbols
	for (int w=1; w<12; w++) {
		int i=first[w];
		while (i != 0xFF) {
			codes[stored].width = w;
			codes[stored].symbol = i;
			codes[stored].code = 0;
			stored++;
			i = next[i];
		}
	}

	outTree[0] = outTree[1] = -128;
	int treeLength = 2;

	// Build the tree
	for (int i=0; i<stored; i++) {
		// add path at i to tree
		int base = 0;
		for (int j=codes[i].width; j--;) {
			int bit = (codes[i].code >> j) & 1;
			if (j) {
				if (outTree[base + bit]==-128) {
					outTree[base + bit] = -treeLength;
					outTree[treeLength] = outTree[treeLength+1] = -128;
					base = treeLength;
					treeLength += 2;
				}
				else
					base = -outTree[base + bit];
			}
			else
				outTree[base + bit] = codes[i].symbol;
		}
		if (i<stored-1)
			codes[i+1].code = (codes[i].code + 1) << (codes[i+1].width - codes[i].width);
	}
	return treeLength;
}

#define DECODE(tree,bitstream,bitOffset,decoded) \
	decoded = 0; \
	for(;;) { \
		int bit = (bitstream[bitOffset >> 4] >> (bitOffset & 15)) & 1; \
		bitOffset++; \
		decoded = tree[decoded + bit]; \
		if (decoded < 0) \
			decoded = -decoded; \
		else \
			break; \
	}
		

uint32_t huffman_decode_header(int8_t outTree[],const int8_t *headerTree,uint32_t headerSize,const uint16_t *bitstream) {
	// Decode the header
	assert(headerSize<=24);
	uint8_t widths[24];
	size_t bitOffset = 0;
	for (size_t i=0; i<headerSize; i++) {
		int w;
		DECODE(headerTree,bitstream,bitOffset,w);
		widths[i] = w;
	}

	// Use the header to generate the final decode tree
	huffman_generate_tree(outTree,widths,headerSize);
	return bitOffset;
}

uint32_t huffman_decode_body(const int8_t *bodyTree,const uint16_t *bitstream,uint8_t *dest,uint32_t endOffset,uint32_t progress) {
	uint16_t runLength = 0;
	uint16_t bitOffset = (uint16_t) progress;
	uint16_t destOffset = progress >> 16;
	while (destOffset < endOffset || runLength) {
		int d;
		DECODE(bodyTree,bitstream,bitOffset,d);
		if (d >= 16)
			runLength += d - 13;
		else if (runLength) {	// runLength will always be at least 3
			if (destOffset & 1)
				--runLength,dest[destOffset++>>1] |= d;
			do {
				dest[destOffset>>1] = d | (d << 4);
				destOffset += 2, runLength -= 2;
			} while (runLength > 1);
			if (runLength) {
				dest[destOffset++>>1] = d<<4;
				runLength = 0;
			}
		}
		else if (destOffset & 1)
			dest[destOffset++>>1] |= d;
		else
			dest[destOffset++>>1] = d << 4;
	}
	return (destOffset << 16) | bitOffset;
}

void huffman_decode(const int8_t *headerTree,uint32_t headerSize,const uint16_t *bitstream,uint8_t *dest,uint32_t destSize) {
	int8_t bodyTree[64];
	huffman_decode_body(bodyTree,bitstream,dest,destSize+destSize,huffman_decode_header(bodyTree,headerTree,headerSize,bitstream));
}

