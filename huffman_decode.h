
int huffman_generate_tree(int8_t outTree[],const uint8_t *widths,int numCodes) {
	struct code_t { uint8_t width, symbol; uint16_t code; };
	assert(numCodes<=24);
	code_t codes[24];
	uint8_t next[24];
	uint8_t first[12];

	// Generate a list sorted by width (in reverse order so final list is in correct direction)
	*(unsigned*)(first+0) = ~0U;
	*(unsigned*)(first+4) = ~0U;
	*(unsigned*)(first+8) = ~0U;
	for (int i=numCodes; i--;) {
		assert(widths[i]<12);
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
