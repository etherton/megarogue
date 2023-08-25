struct targa_header {
	uint8_t imageIdLength; // should be zero
	uint8_t colorMapType;	// 0=no color map, 1=color map
	uint8_t imageType;	// 2=uncompressed true-color image
	uint8_t colorMap[5];	// uint16_t: index of first color map entry; uint16_t: number of entries in colormap; uint8_t: number of bits per entry
	uint16_t xOrigin;
	uint16_t yOrigin;
	uint16_t width;
	uint16_t height;
	uint8_t pixelDepth;	// 32
	uint8_t imageDescriptor; // bits 3-0 give alpha depth (8), bit 4:right-to-left if set, bit 5:top-to`-bottom if set
};

inline bool is_valid_targa_header(targa_header *h) {
	return h->imageIdLength==0 && h->colorMapType==0 && h->imageType==2 && h->pixelDepth==32 && h->imageDescriptor == 0x28;
}
