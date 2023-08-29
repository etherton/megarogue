#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc,char **argv) {
	if (argc<2) { 
		fprintf(stderr,"missing filename\n"); 
		return 1; 
	}
	FILE *f = fopen(argv[1],"rb+");
	if (!f) { 
		fprintf(stderr,"open '%s' failed\n",argv[1]); 
		return 1; 
	}
	fseek(f,0,SEEK_END);
	size_t size = ftell(f);
	fseek(f,0,SEEK_SET);
	size_t rounded = 4096;
	while (rounded < size)
		rounded <<= 1;
	printf("rom size is %zu, rounded up to %zu\n",size,rounded);
	uint8_t *rom = new uint8_t[rounded];
	memset(rom,0,rounded);
	fread(rom,1,size,f);
	fseek(f,0,SEEK_SET);
	uint16_t *rom16 = (uint16_t*) rom;
	uint32_t *rom32 = (uint32_t*) rom;
	uint16_t checksum = 0;
	for (size_t i=256; i<rounded/2; i++)
		checksum += rom16[i];
	rom16[0x18E/2] = checksum;
	rom32[0x1a4/4] = (uint32_t)rounded-1;
	printf("checksum set to %u\n",rom16[0x18e/2]);
	fwrite(rom,1,rounded,f);
	fclose(f);
	return 0;
}
