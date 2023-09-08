#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <assert.h>
#include <stdio.h>

#include "targa_header.h"

/*
	First pass: Build up individual tile bitmaps and local palettes.
	Randomly assign each tile to a palette.
	Generate each final palette, convert each tile to its matching palette,
	and track the accumulated error.
	Place each tile into a priority queue based on accumulated error.
	For the UNMARKED tile nearest the head of the queue:
		1. Try changing to another palette
		2. Rebuild that palette from scratch
		3. Is the accumulated error for this image better than it used to be?
	For now: 
		Randomly assign the first 
*/

int debug;

struct rgb { 
	rgb() { key=0; }
	rgb(uint8_t r_,uint8_t g_,uint8_t b_) {
		assert(r_<64); assert(g_<64); assert(b_<64);
		r = r_; g = g_; b = b_; pad = 0;
	}
	union {
		struct { uint8_t r, g, b, pad; };
		uint8_t bytes[4];
		uint32_t key;
	};
	void min(const rgb &o) {
		if (o.r < r) r = o.r;
		if (o.g < g) g = o.g;
		if (o.b < b) b = o.b;
	}
	void max(const rgb &o) {
		if (o.r > r) r = o.r;
		if (o.g > g) g = o.g;
		if (o.b > b) b = o.b;
	}
	bool operator< (const rgb &that) const { 
		return key < that.key;
	}
	static uint16_t round6to3(uint8_t ch,int sh) {
		// Knock six bits down to three
		return ch<59? ((ch+4) >> 3) << sh : 7 << sh;
	}
	static uint16_t round6to2(uint8_t ch,int sh) {
		// Knock six bits down to two
		return ch<55? ((ch+8) >> 4) << sh : 3 << sh;
	}
	uint16_t encode() const {
		return round6to3(b,9) | round6to3(g,5) | round6to3(r,1);
	}
	uint8_t mask_bit_index() const {
		return round6to2(r,4) | round6to2(g,2) | round6to2(b,0);
	}
};

class palette {
	std::map<rgb,uint8_t> asMap;
	std::vector<rgb> asVector;

	static uint8_t round8to6(uint8_t c) {
		return c>253? 63 : (c+2)>>2;
	}
public:
	rgb finalPalette[16];
	uint64_t occupancy = 0;
	uint8_t nextColor = 0;

	void tally(uint8_t r,uint8_t g,uint8_t b,uint8_t a) {
		if (a > 127) {
			// Knock off two bits to save processing time (final result is only 3 bits anyway)
			rgb e(round8to6(r),round8to6(g),round8to6(b));
			if (asMap.insert(std::pair<rgb,uint8_t>(e,255)).second)
				asVector.push_back(e);
		}
	}
	void median_cut(int maxColors) {
		if (!asVector.size())
			return; // no colors!
		// Determine bounding box in color space; [mini,maxi) half-open interval
		rgb mini(63,63,63), maxi(0,0,0);
		for (auto &e: asVector) {
			mini.min(e);
			maxi.max(e);
		}
		// Make upper bound exclusive
		maxi.r++; maxi.g++; maxi.b++;
		if (debug) printf("starting median cut, %zu(%zu) colors, ranged %d,%d,%d - %d,%d,%d\n",
			asMap.size(),asVector.size(),mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b);
		median_cut(mini,maxi,0,asVector.size(),maxColors);
		if (debug>1) printf("...final occupancy %llx\n",occupancy);
	}
	uint8_t remap(uint8_t r,uint8_t g,uint8_t b,uint8_t a) {
		if (a < 128)
			return 0;
		rgb e(round8to6(r),round8to6(g),round8to6(b));
		return asMap.find(e)->second;
	}
private:
	void median_cut(const rgb &mini,const rgb &maxi,size_t start,size_t end,int maxColors) {
		assert(maxColors);
		if (maxColors == 1) {
			// Assign everything in [mini,maxi) to a new color slot.
			auto &fp = finalPalette[++nextColor];
			assert(nextColor<=15);
			assert(end>start);
			int rSum = 0, gSum = 0, bSum = 0, found = 0;
			for (size_t i=start; i<end; i++) {
				asMap[asVector[i]] = nextColor;
				rSum += asVector[i].r;
				gSum += asVector[i].g;
				bSum += asVector[i].b;
			}
			int delta = (int)(end - start);
			fp.r = rSum / delta;
			fp.g = gSum / delta;
			fp.b = bSum / delta;
			if (debug>1) printf("map [%d,%d,%d] - (%d,%d,%d) -> %d (%d,%d,%d)\n",mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b,nextColor,fp.r,fp.g,fp.b);
			occupancy |= 1ULL << fp.mask_bit_index();
			return;
		}
		/* identify largest axis and split that one */
		uint8_t dR = maxi.r - mini.r;
		uint8_t dG = maxi.g - mini.g;
		uint8_t dB = maxi.b - mini.b;
		uint8_t axis = 2;
		// Slightly favor green since human eye is more perceptive to it
		if (dG >= dR && dG >= dB)
			axis = 1;
		else if (dR >= dG && dR >= dB)
			axis = 0;
		if (debug>1) printf("median_cut %d,%d,%d - %d,%d,%d - [%zu,%zu) - dom axis %d, %d colors\n",
			mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b,start,end,axis,maxColors);
		// Sort the array along the dominant axis
		std::stable_sort(asVector.begin() + start,asVector.begin() + end,
			[&axis](const rgb &a,const rgb &b) { return a.bytes[axis] < b.bytes[axis]; });
		// Compute appropriate midpoints and split the box
		size_t mid = (start + end) >> 1;
		rgb miniMid = maxi, midMaxi = mini;
		miniMid.bytes[axis] = midMaxi.bytes[axis] = asVector[mid].bytes[axis];
		// Recurse and process each half region, but only if it isn't empty
		if (mid>start)
			median_cut(mini,miniMid,start,mid,maxColors>>1);
		if (end>mid)
			median_cut(midMaxi,maxi,mid,end,maxColors-(maxColors>>1));
	}
};

int main(int argc,char** argv) {
	if (argc<2) {
		fprintf(stderr,"args: manifest_name\n");
		return 1;
	}
	FILE *m = fopen(argv[1],"r");
	if (!m) {
		fprintf(stderr,"unable to open %s\n",argv[1]);
		return 1;
	}

	char lineBuf[1024];
	int lineNumber = 0;
	struct tile {
		palette p;
		size_t o;
	};
	struct work_item {
		char cfile_name[64];
		char c_sym[64];
		uint8_t *pixel_data;
		int image_width;
		int tileWidth, tileHeight, firstCol, firstRow, numCols, numRows;
		std::vector<tile> tiles;
	};
	std::vector<work_item> work_items;
	struct popcmp {
		bool operator()(const uint64_t &a,const uint64_t &b) const {	
			int pa = __builtin_popcountll(a);
			int pb = __builtin_popcountll(b);
			return (pa==pb)? a < b : pa<pb;
		}
	};
	// Items are sorted by population count
	std::map<uint64_t,std::vector<uint32_t>,popcmp> palette_groups;
	uint32_t tile_count = 0;
	while (fgets(lineBuf,sizeof(lineBuf),m)) {
		++lineNumber;
		if (lineBuf[0]=='#')
			continue;
		work_item i;
		char image_name[64];
		puts(lineBuf);
		if (sscanf(lineBuf,"%s %s %s %d %d %d %d %d %d",image_name,i.cfile_name,i.c_sym,
			&i.tileWidth,&i.tileHeight,&i.firstCol,&i.firstRow,&i.numCols,&i.numRows) != 9) {
			fprintf(stderr,"bad data on line %d\n",lineNumber);
			return 1;
		}

		FILE *t = fopen(image_name,"rb");
		if (!t) {
			fprintf(stderr,"unable to open tga '%s'\n",image_name);
			return 1;
		}
		targa_header hdr;
		fread(&hdr,sizeof(hdr),1,t);
		if (!is_valid_targa_header(&hdr)) {
			fprintf(stderr,"file '%s' must be 32bpp (RLE or uncompressed) with top left origin\n",image_name);
			return 1;
		}
		if (hdr.width < (i.firstCol + i.numCols) * i.tileWidth) {
			fprintf(stderr,"image %s is only %d wide but needs to be at least %d\n",
				image_name,hdr.width,(i.firstCol + i.numCols) * i.tileWidth);
			return 1;
		}
		if (hdr.height < (i.firstRow + i.numRows) * i.tileHeight) {
			fprintf(stderr,"image %s is only %d tall but needs to be at least %d\n",
				image_name,hdr.height,(i.firstRow + i.numRows) * i.tileHeight);
			return 1;
		} 
	
		i.pixel_data = read_targa_file(&hdr,t);
		i.image_width = hdr.width;
		fclose(t);
		if (!i.pixel_data) {
			fprintf(stderr,"error reading image '%s'\n",image_name);
			return 1;
		}
		printf("processing %s...\n",image_name);
		for (int r=0; r<i.numRows; r++) {
			for (int c=0; c<i.numCols; c++) {
				tile t;
				for (int y=0; y<i.tileHeight; y++) {
					uint32_t o = (i.image_width * (y + i.tileHeight * (i.firstRow + r)) + i.tileWidth * (i.firstCol + c)) * 4;
					t.o = o;
					for (int x=0; x<i.tileWidth; x++, o+=4)
						t.p.tally(i.pixel_data[o+2],i.pixel_data[o+1],i.pixel_data[o+0],i.pixel_data[o+3]);
				}
				t.p.median_cut(15);
				if (t.p.nextColor) {
					palette_groups[t.p.occupancy].push_back((work_items.size() << 24) | i.tiles.size());
					++tile_count;
					i.tiles.push_back(t);
					if (debug>1) printf("tile offset %zu has palette occupancy %llx, %u colors\n",t.o,t.p.occupancy,t.p.nextColor);
				}
			}
		}
		work_items.push_back(i);
	}
	fclose(m);

	printf("%u total tiles, %zu unique palettes\n",tile_count,palette_groups.size());
	for (auto &p: palette_groups)
		printf("%016llx ",p.first);
	printf("\n");

	// Now try to find palettes which are subsets of others; 
	int merged = 0;
	std::map<uint64_t,std::vector<uint32_t>>::iterator p, q;
	for (p=palette_groups.begin(); std::next(p) != palette_groups.end(); ++p) {
		for (q=std::next(p); q != palette_groups.end();) {
			if ((p->first & q->first) == q->first) { // q is a subset of p
				// copy everything in q over to p
				do {
					p->second.push_back(q->second.back());
					q->second.pop_back();
				} while (q->second.size());
				q = palette_groups.erase(q);
				++merged;
			}
			else
				++q;
		}
	}
	printf("%u merged, %zu palettes remain\n",merged,palette_groups.size());

#if 0
	FILE *cfile = fopen(argv[2],"w");
	if (!cfile)
		return 1;
	const char *c_sym = argv[3];

	fprintf(cfile,"#include \"md_api.h\"\n");

	// First pass over the data, assign each palette group to a different palette for now.
	palette *palettes[4] = { new palette, new palette, new palette, new palette };
	for (int row=0; row<cells_down; row+=palette_group_cell_height) {
		for (int col=0; col<cells_across; col+=palette_group_cell_width) {
			palette &p = *palettes[(row+col) & 3];
			for (int y=0; y<cell_height * palette_group_cell_height; y++) {
				uint32_t offs = (hdr.width * (row*cell_height + start_y+y) + (col*cell_width + start_x)) * 4;
				for (int x=0; x<cell_width * palette_group_cell_width; x++, offs+=4) 
					p.tally(pixel_data[offs+2],pixel_data[offs+1],pixel_data[offs+0],pixel_data[offs+3]);
			}
		}
	}

	// Generate all the palettes based on the sampled data
	for (int i=0; i<4; i++) {
		palettes[i]->median_cut(15);
		fprintf(cfile,"const uint16_t %s_palette_%u[16] = { ",c_sym,i);
		for (int j=0; j<16; j++)
			fprintf(cfile,"0x%03x, ",palettes[i]->finalPalette[j].encode());
		fprintf(cfile,"};\n");
	}

	std::string dirString;

	// Next pass over the data is to generate the actual tiles by mapping them through the associated palette
	// This loop is more intricate sprite tiles are 8x8 columns, then rows.
	for (int row=0; row<cells_down; row+=palette_group_cell_height) {
		for (int col=0; col<cells_across; col+=palette_group_cell_width) {
			palette &p = *palettes[(row+col) & 3];
			for (int pgRow=0; pgRow<palette_group_cell_height; pgRow++) {
				for (int pgCol=0; pgCol<palette_group_cell_width; pgCol++) {
					fprintf(cfile,"const uint32_t %s_%d_%d[] = {\n",c_sym,col+pgCol,row+pgRow);
					dirString += std::string("{ ") + c_sym + "_" + std::to_string(col+pgCol) + "_" + std::to_string(row+pgRow) + ", " + c_sym + "_palette_" + std::to_string((row+col)&3) + " },\n";
					for (int spCol=0; spCol<cell_width; spCol+=8) {
						for (int spRow=0; spRow<cell_height; spRow+=8) {
							fprintf(cfile,"\t");
							for (int y=0; y<8; y++) {
								uint32_t pix = 0;
								uint32_t offs = (hdr.width * ((row+pgRow)*cell_height + start_y+spRow+y) + ((col+pgCol)*cell_width + start_x+spCol)) * 4;
								for (int x=0; x<8; x++,offs+=4) {
									pix <<= 4;
									pix |= p.remap(pixel_data[offs+2],pixel_data[offs+1],pixel_data[offs+0],pixel_data[offs+3]);
								}
								fprintf(cfile,"0x%08x,",pix);
							}
							fprintf(cfile,"\n");
						}
					}
					fprintf(cfile,"};\n\n");
				}
			}
		}
	}

	fprintf(cfile,"const struct { const uint32_t *tile; const uint16_t *palette; } %s_directory[] = {\n",c_sym);
	fprintf(cfile,"%s",dirString.c_str());
	fprintf(cfile,"};\n\nconst uint16_t %s_directory_count = sizeof(%s_directory) / sizeof(%s_directory[0]);\n",c_sym,c_sym,c_sym);
	fclose(cfile);
#endif
}
