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

constexpr uint16_t TRANSP = 0x111;	// impossible color

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
	uint16_t encode() {
		return round6to3(b,9) | round6to3(g,5) | round6to3(r,1);
	}
};

class palette {
	std::set<rgb> asSet;
	std::vector<rgb> asVector;
	uint8_t nextColor = 0;
	uint8_t remapColorToEntry[64][64][64] = {};

	static uint8_t round8to6(uint8_t c) {
		return c>253? 63 : (c+2)>>2;
	}
public:
	rgb finalPalette[16];

	void tally(uint8_t r,uint8_t g,uint8_t b,uint8_t a) {
		if (a > 127) {
			// Knock off two bits to save processing time (final result is only 3 bits anyway)
			rgb e(round8to6(r),round8to6(g),round8to6(b));
			if (asSet.insert(e).second)
				asVector.push_back(e);
		}
	}
	void median_cut(int maxColors) {
		// Determine bounding box in color space; [mini,maxi) half-open interval
		rgb mini(63,63,63), maxi(0,0,0);
		for (auto &e: asVector) {
			mini.min(e);
			maxi.max(e);
		}
		maxi.r++; maxi.g++; maxi.b++;
		printf("starting median cut, %zu(%zu) colors, ranged %d,%d,%d - %d,%d,%d\n",
			asSet.size(),asVector.size(),mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b);
		median_cut(mini,maxi,0,asVector.size(),maxColors);
	}
	uint8_t remap(uint8_t r,uint8_t g,uint8_t b,uint8_t a) {
		return a > 127? remapColorToEntry[round8to6(r)][round8to6(g)][round8to6(b)] : 0;
	}
private:
	void median_cut(const rgb &mini,const rgb &maxi,size_t start,size_t end,int maxColors) {
		assert(maxColors);
		if (maxColors == 1) {
			// Assign everything in [mini,maxi) to a new color slot.
			auto &fp = finalPalette[++nextColor];
			assert(nextColor<=15);
			fp.r = (mini.r + maxi.r) >> 1;
			fp.g = (mini.g + maxi.g) >> 1;
			fp.b = (mini.b + maxi.b) >> 1;
			printf("map [%d,%d,%d] - (%d,%d,%d) -> %d (%d,%d,%d)\n",mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b,nextColor,fp.r,fp.g,fp.b);
			for (int i=mini.r; i<maxi.r; i++) {
				for (int j=mini.g; j<maxi.g; j++) {
					for (int k=mini.b; k<maxi.b; k++) {
						assert(remapColorToEntry[i][j][k]==0);
						remapColorToEntry[i][j][k] = nextColor;
					}
				}
			}
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
		printf("median_cut %d,%d,%d - %d,%d,%d - [%zu,%zu) - dom axis %d, %d colors\n",
			mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b,start,end,axis,maxColors);
		// Sort the array along the dominant axis
		std::stable_sort(asVector.begin() + start,asVector.begin() + end,
			[&axis](const rgb &a,const rgb &b) { return a.bytes[axis] < b.bytes[axis]; });
		// Compute appropriate midpoints and split the box
		size_t mid = (start + end) >> 1;
		rgb miniMid = maxi, midMaxi = mini;
		miniMid.bytes[axis] = midMaxi.bytes[axis] = asVector[mid].bytes[axis];
		// Recurse and process each half region
		median_cut(mini,miniMid,start,mid,maxColors>>1);
		median_cut(midMaxi,maxi,mid,end,maxColors-(maxColors>>1));
	}
};

int main(int argc,char** argv) {
	if (argc<12) {
		fprintf(stderr,"args: tga_name c_name c_sym start_x start_y cell_width cell_height cells_across cells_down palette_group_cell_width palette_group_cell_height\n");
		return 1;
	}
	int start_x = atoi(argv[4]), start_y = atoi(argv[5]), cell_width = atoi(argv[6]), cell_height = atoi(argv[7]), 
		cells_across = atoi(argv[8]), cells_down = atoi(argv[9]), 
		palette_group_cell_width = atoi(argv[10]), palette_group_cell_height = atoi(argv[11]);
		
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
}
