#include <map>
#include <string>
#include <stdio.h>

#include "targa_header.h"

int main(int argc,char** argv) {
	if (argc<13) {
		fprintf(stderr,"args: tga_name c_name c_sym start_x start_y cell_width cell_height cells_across cells_down palette_group_cell_width palette_group_cell_height color_bits_per_channel\n");
		return 1;
	}
	int start_x = atoi(argv[4]), start_y = atoi(argv[5]), cell_width = atoi(argv[6]), cell_height = atoi(argv[7]), 
		cells_across = atoi(argv[8]), cells_down = atoi(argv[9]), 
		palette_group_cell_width = atoi(argv[10]), palette_group_cell_height = atoi(argv[11]),
		color_bits_per_channel = atoi(argv[12]);
		
	FILE *tga = fopen(argv[1],"rb"); 
	if (!tga) {
		fprintf(stderr,"unable to open tga '%s'\n",argv[1]);
		return 1;
	}

	targa_header hdr;
	fread(&hdr,sizeof(hdr),1,tga);
	if (!is_valid_targa_header(&hdr)) {
		fprintf(stderr,"file '%s' must be uncompressed (no RLE) 32bpp with top left origin\n",argv[1]);
		return 1;
	}
	uint32_t sizeBytes = (hdr.width * hdr.height * hdr.pixelDepth) >> 3;
	uint8_t *pixel_data = new uint8_t[sizeBytes];
	if (fread(pixel_data,1,sizeBytes,tga) != sizeBytes) {
		fprintf(stderr,"short read on tga file\n");
		return 1;
	}
	fclose(tga);
	if (hdr.width < start_x + cells_across * cell_width) {
		fprintf(stderr,"image is only %d wide but needs to be at least %d\n",hdr.width,start_x + cells_across * cell_width);
		return 1;
	}
	if (hdr.height < start_y + cells_down * cell_height) {
		fprintf(stderr,"image is only %d tall but needs to be at least %d\n",hdr.height,start_y + cells_down * cell_height);
		return 1;
	}
	FILE *cfile = fopen(argv[2],"w");
	if (!cfile)
		return 1;
	const char *c_sym = argv[3];

	std::map<uint16_t,uint8_t> gp;
	std::vector<std::map<uint16_t,uint8_t>> palettes;
	std::map<std::pair<int,int>,size_t> palette_map;
	const uint16_t TRANSP = 0x111; // impossible color
	int width=hdr.width, height=hdr.height;
	uint8_t gnc = 0, dups = 0;
	fprintf(cfile,"#include \"md_api.h\"\n");
	for (int row=start_y; row<start_y+cell_height*cells_down; row+=cell_height) {
		for (int col=start_x; col<start_x+cell_width*cells_across; col+=cell_width) {
			std::map<uint16_t,uint8_t> p;		
			p[TRANSP] = 0;
			uint8_t nc = 0;
			fprintf(cfile,"const uint32_t %s_tiles_%d_%d[] = {\n",c_sym,row,col);
			uint16_t paletteArray[32] = { TRANSP };
			for (int xx=0; xx<cell_width; xx+=8) {
				for (int yy=0; yy<cell_height; yy+=8) {
					for (int y=0; y<8; y++) {
						uint32_t pix = 0;
						for (int x=0; x<8; x++) {
							int r = pixel_data[(width*(row+yy+y)+(col+xx+x))*4+2];
							int g = pixel_data[(width*(row+yy+y)+(col+xx+x))*4+1];
							int b = pixel_data[(width*(row+yy+y)+(col+xx+x))*4+0];
							int a = pixel_data[(width*(row+yy+y)+(col+xx+x))*4+3];
							int packed;
							auto rnd = [&](int c,int s) {
								return (c>>(8-color_bits_per_channel)) << (s + 4 - color_bits_per_channel);
							};
							if (a < 128)
								packed=TRANSP;
							else
								packed = rnd(b,8) | rnd(g,4) | rnd(r,0);
							if (p.find(packed) == p.end()) {
								paletteArray[++nc] = packed;
								p[packed] = nc;
							}
							pix <<= 4;
							pix |= (p[packed]);
							if (gp.find(packed) == gp.end())
								gp[packed] = ++gnc;
						}
						fprintf(cfile,"0x%08x,",pix);
					}
					fprintf(cfile,"\n");
				}
			}
			fprintf(cfile,"};\nconst uint16_t %s_palette_%d_%d[16] = {",c_sym,row,col);
			for (int i=0; i<16; i++) fprintf(cfile,"0x%03x,",paletteArray[i]);
			fprintf(cfile,"};\n\n");
			printf("block %d,%d has %d colors: ",row,col,nc);
			for (auto &a: p)
				printf("%03x ",a.first);
			printf("\n");
		}
	}
	printf("total %d colors\n",gnc);
	printf("%d dupes\n",dups);

	fprintf(cfile,"const struct directory { const uint32_t *tilePtr; const uint16_t *palPtr; } %s_directory[] = {\n",c_sym);
	for (int row=start_y; row<start_y+cells_down*cell_height; row+=cell_height) {
		for (int col=start_x; col<start_x+cells_across*cell_width; col+=cell_width) {
			fprintf(cfile,"\t{%s_tiles_%d_%d, %s_palette_%d_%d },\n",c_sym,row,col,c_sym,row,col);
		}
	}
	fprintf(cfile,"};\nconst uint16_t %s_directory_count = sizeof(%s_directory) / sizeof(%s_directory[0]);\n",c_sym,c_sym,c_sym);
	fclose(cfile);

#if 0
	size_t it=0, stop=1ULL << palettes.size();
	// try to fit everything into two palettes by brute force
	int lastPct = 0;
	uint8_t best[2] = {0,0};
	while (it != stop) {
		int pct = (int)(it * 100 / stop);
		if (pct != lastPct) {
			printf("%d%% complete %d/%d best\r",lastPct=pct,best[0],best[1]);
			fflush(stdout);
		}
		std::map<uint16_t,uint8_t> pals[2];
		pals[0][TRANSP] = 0;
		pals[1][TRANSP] = 0;
		uint8_t nc[2] = { 0,0 };

		auto isClose = [](uint16_t a,uint16_t b) {
			if (a==b) return true;
			return (abs((a >> 9) - (b >> 9)) + abs(((a >> 5) & 7) - ((b >> 5) & 7)) + abs(((a >> 1) & 7) - ((b >> 1) & 7)) <= 2);
		};
		for (size_t i=0; i<palettes.size(); i++) {
			size_t which = (it >> i) & 1;
			for (auto &a: palettes[i]) {
				bool found = false;
				for (auto &b: pals[which]) {
					if (isClose(a.first,b.first)) {
						found = true;
						break;
					}
				}
				if (!found)
					pals[which][a.first] = ++nc[which];
			}
		}
		if (nc[0]<=15 && nc[1]<=15) {
			printf("got a fit in %zx\n",it);
			break;
		}
		if (nc[0]*nc[1]>best[0]*best[1]) best[0]=nc[0],best[1]=nc[1];
		++it;
	}
#endif
	return 0;

}
