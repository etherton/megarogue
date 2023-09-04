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
		
	FILE *f = fopen(argv[1],"rb"); 
	if (!f) {
		fprintf(stderr,"unable to open tga '%s'\n",argv[1]);
		return 1;
	}

	targa_header hdr;
	fread(&hdr,sizeof(hdr),1,f);
	if (!is_valid_targa_header(&hdr)) {
		fprintf(stderr,"file '%s' must be 32bpp (RLE or uncompressed) with top left origin\n",argv[1]);
		return 1;
	}
	uint8_t *pixel_data = read_targa_file(&hdr,f);
	if (!pixel_data) {
		fprintf(stderr,"short read on tga file\n");
		return 1;
	}
	fclose(f);
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
	std::vector<std::string> directory_entries;
	for (int row=0; row<cells_down; row+=palette_group_cell_height) {
		for (int col=0; col<cells_across; col+=palette_group_cell_width) {
			std::map<uint16_t,uint8_t> p;		
			std::string palette_name = std::string(c_sym) + "_palette_" + std::to_string(row) + "_" + std::to_string(col);
			uint16_t paletteArray[32] = { TRANSP };
			p[TRANSP] = 0;
			uint8_t nc = 0;
			for (int sub_row=0; sub_row<palette_group_cell_height; sub_row++) {
				for (int sub_col=0; sub_col<palette_group_cell_width; sub_col++) {
					int base_col = start_x + (col+sub_col) * cell_width, base_row = start_y + (row+sub_row) * cell_height;
					std::string tile_name = std::string(c_sym) + "_tiles_" + std::to_string(row+sub_row) + "_" + std::to_string(col+sub_col);
					fprintf(cfile,"const uint32_t %s[] = {\n",tile_name.c_str());
					directory_entries.push_back(std::string("{ ") + tile_name + ", " + palette_name	+ "},");
					for (int xx=0; xx<cell_width; xx+=8) {
						for (int yy=0; yy<cell_height; yy+=8) {
							for (int y=0; y<8; y++) {
								uint32_t pix = 0;
								for (int x=0; x<8; x++) {
									uint32_t o = (width * (base_row+yy+y) + (base_col+xx+x)) * 4;
									int r = pixel_data[o+2];
									int g = pixel_data[o+1];
									int b = pixel_data[o+0];
									int a = pixel_data[o+3];
									int packed;
									auto rnd = [&](int c,int s) {
										if (c < 248) c += 15;
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
					fprintf(cfile,"};\n");
				}
			}
			fprintf(cfile,"const uint16_t %s[16] = {",palette_name.c_str());
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
	for (auto &d: directory_entries)
		fprintf(cfile,"%s\n",d.c_str());
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
