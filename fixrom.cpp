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
	uint16_t checksum = 0;
	for (size_t i=512; i<rounded; i+=2)
		checksum += (rom[i]<<8) + rom[i+1];
	rom[0x18E] = checksum >> 8;
	rom[0x18F] = checksum;
	rom[0x1a4] = ((rounded-1)>>24);
	rom[0x1a5] = ((rounded-1)>>16);
	rom[0x1a6] = ((rounded-1)>>8);
	rom[0x1a7] = (rounded-1);
	printf("checksum set to 0x%04x\n",checksum);
	fwrite(rom,1,rounded,f);
	fclose(f);
	return 0;
}
