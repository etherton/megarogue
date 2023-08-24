/* GIMP RGBA C-Source image dump (tiny_dungeon_monsters.c) */

#include <map>
#include <string>
#include <stdio.h>

#include "char24.c"
#define CONVERT char24

int main(int argc,char** argv) {
	if (argc<2)
		return 1;
	FILE *cfile = fopen(argv[1],"w");
	if (!cfile)
		return 1;

	std::map<uint16_t,uint8_t> gp;
	std::vector<std::map<uint16_t,uint8_t>> palettes;
	std::map<std::pair<int,int>,size_t> palette_map;
	const uint16_t TRANSP = 0x111; // impossible color
	uint8_t gnc = 0, dups = 0;
	fprintf(cfile,"#include \"md_api.h\"\n");
	int rowstep=24, colstep=24;
	int inset=24;
	for (int row=inset; row<CONVERT.height-3*inset; row+=rowstep) {
		for (int col=inset; col<CONVERT.width-inset; col+=colstep) {
			std::map<uint16_t,uint8_t> p;		
			p[TRANSP] = 0;
			uint8_t nc = 0;
			fprintf(cfile,"const uint32_t tiles_%d_%d[] = {\n",row,col);
			uint16_t paletteArray[32] = { TRANSP };
			for (int xx=0; xx<colstep; xx+=8) {
				for (int yy=0; yy<rowstep; yy+=8) {
					for (int y=0; y<8; y++) {
						uint32_t pix = 0;
						for (int x=0; x<8; x++) {
							int r = CONVERT.pixel_data[(CONVERT.width*(row+yy+y)+(col+xx+x))*4+0];
							int g = CONVERT.pixel_data[(CONVERT.width*(row+yy+y)+(col+xx+x))*4+1];
							int b = CONVERT.pixel_data[(CONVERT.width*(row+yy+y)+(col+xx+x))*4+2];
							int a = CONVERT.pixel_data[(CONVERT.width*(row+yy+y)+(col+xx+x))*4+3];
							int packed;
							auto rnd = [](int c,int s) {
								/* if (c < 240) 
									c = c+16; */
								return s!=99? (c>>6) << (s+1) : (c>>5) << (s + 1);
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
			fprintf(cfile,"};\nconst uint16_t palette_%d_%d[16] = {",row,col);
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
