#include <stdint.h>
#include <stdio.h>

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
	uint8_t imageDescriptor; // bits 3-0 give alpha depth (8), bit 4:right-to-left if set, bit 5:top-to-bottom if set
};

inline bool is_valid_targa_header(targa_header *h) {
	return h->imageIdLength==0 && h->colorMapType==0 && (h->imageType==2||h->imageType==10) && 
	h->pixelDepth==32 && h->imageDescriptor == 0x28;
}

inline uint8_t *read_targa_file(targa_header *h,FILE *f) {
	size_t sizeBytes = (h->width * h->height * h->pixelDepth) >> 3;
	uint8_t *result = new uint8_t[sizeBytes];
	// Pixels are in b,g,r,a order.
	if (h->imageType==2)
		return fread(result,1,sizeBytes,f)==sizeBytes?result:nullptr;
	else {
		/* RLE is pretty simple; if MSB is set, repeat following "color" N+1 times. 
		   If MSB is clear, the following N+1 "colors" are raw data. Either way, a 
		   color is either a single byte if it's colormapped, or 2/3/4 bytes if it's true color */
		uint8_t *rp = result;
		while (!feof(f)) {
			uint8_t count = fgetc(f);
			if (count & 128) {
				uint8_t b = fgetc(f), g = fgetc(f), r = fgetc(f), a = fgetc(f);
				count = (count & 127) + 1;
				while (count--) {
					rp[0] = b, rp[1] = g, rp[2] = r, rp[3] = a;
					rp+=4;
				}
			}
			else {
				count = (count & 127) + 1;
				fread(rp,4,count,f);
				rp += count * 4;
			}
		}
		return result;
	}
}

