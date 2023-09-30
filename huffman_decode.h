
extern int huffman_generate_tree(int8_t outTree[],const uint8_t *widths,int numCodes);

extern void huffman_decode(const int8_t *headerTree,uint32_t headerSize,const uint16_t *bitstream,uint8_t *dest,uint32_t destSize);

extern uint32_t huffman_decode_header(int8_t outTree[],const int8_t *headerTree,uint32_t headerSize,const uint16_t *bitstream);
extern uint32_t huffman_decode_body(const int8_t *bodyTree,const uint16_t *bitstream,uint8_t *dest,uint32_t endOffset,uint32_t progress);
