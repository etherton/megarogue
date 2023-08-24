/* GIMP RGBA C-Source image dump (tiny_dungeon_monsters.c) */

#include <map>

#include "tdm.h"

int main(int,char**) {
	std::map<uint16_t,uint8_t> gp;
	std::vector<std::map<uint16_t,uint8_t>> palettes;
	std::map<std::pair<int,int>,size_t> palette_map;
	uint8_t gnc = 0, dups = 0;
	// each block is 64 by 16
	for (int row=0; row<512; row+=16) {
		for (int col=0; col<256; col+=64) {
			std::map<uint16_t,uint8_t> p;		
			p[0x111] = 0;
			uint8_t nc = 0;
			for (int y=0; y<16; y++) {
				for (int x=0; x<64; x++) {
					int r = pixel_data[1024*(row+y)+4*(col+x)+0];
					int g = pixel_data[1024*(row+y)+4*(col+x)+1];
					int b = pixel_data[1024*(row+y)+4*(col+x)+2];
					int a = pixel_data[1024*(row+y)+4*(col+x)+3];
					int packed;
					auto rnd = [](int c,int s) {
						if (c < 240) 
							c = c+16;
						return (c>>5) << (s + 1);
					};
					if (a < 128)
						packed=0x111;
					else
						packed = rnd(r,8) | rnd(g,4) | rnd(b,0);
					if (p.find(packed) == p.end())
						p[packed] = ++nc;
					if (gp.find(packed) == gp.end())
						gp[packed] = ++gnc;
				}
			}
			printf("block %d,%d has %d colors: ",row,col,nc);
			for (auto &a: p)
				printf("%03x ",a.first);
			printf("\n");
			uint8_t unique = 1;
			size_t existing = palettes.size();
			for (size_t i=0; i<palettes.size(); i++) {
				auto &a = palettes[i];
				unique = 0;
				for (auto &b: p)
					if (a.find(b.first) == a.end())
						++unique;
				if (!unique) {
					existing = i;
					++dups;
					break;
				}
			}
			if (unique)
				palettes.push_back(p);
			palette_map[std::pair<int,int>(row,col)] = existing;
		}
	}
	printf("total %d colors\n",gnc);
	printf("%d dupes\n",dups);
	printf("%zu total palettes\n",palettes.size());
	return 0;
}
