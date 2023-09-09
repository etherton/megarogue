#include <set>
#include <string>
#include <algorithm>
#include <assert.h>
#include <stdio.h>

#include "targa_header.h"

int debug = 0;

// map sqrt(i) to fixed point 4.4
const uint8_t sqrt_tab[49+49+49+1] = {
	0,16,23,28,32,36,39,42,45,48,51,53,55,58,60,62,64,66,68,70,72,73,75,77,78,80,82,83,85,86,88,89,91,92,93,95,96,
	97,99,100,101,102,104,105,106,107,109,110,111,112,113,114,115,116,118,119,120,121,122,123,124,125,126,127,128,
	129,130,131, 132,133,134,135,136,137,138,139,139,140,141,142,143,144,145,146,147,148,148,149,150,151,152,153,
	153,154,155,156,157,158,158,159,160,161,162,162,163,164,165,166,166,167,168,169,169,170,171,172,172,173,174,175,
	175,176,177,177,178,179, 180,180,181,182,182,183,184,185,185,186,187,187,188,189,189,190,191,191,192,193,193,194,
};

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
	static uint16_t round6to3(uint8_t ch) {
		// Knock six bits down to three
		return ch<59? ((ch+4) >> 3) : 7;
	}
	uint16_t encode() const {
		return (b << 9) | (g << 5) | (b << 1);
	}
};

class palette {
	std::set<rgb> asSet;
	std::vector<rgb> asVector;
	uint8_t nearestDist[8][8][8]; // fixed point distance to nearest palette. (max dist is 12.12 so this is a 4.4 fixed point)
	uint8_t nearestIndex[8][8][8];

	static uint8_t round8to6(uint8_t c) {
		return c>253? 63 : (c+2)>>2;
	}
	static uint8_t round8to3(uint8_t c) {
		return c>239? 7 : (c+16)>>5;
	}
public:
	palette() {
		reset();
	}
	void reset() {
		asSet.clear();
		asVector.clear();
		nextColor = 0;
	}
	rgb finalPalette[16];
	uint8_t nextColor;

	void tally(uint8_t r,uint8_t g,uint8_t b,uint8_t a) {
		if (a > 127) {
			// Knock off two bits to save processing time (final result is only 3 bits anyway)
			rgb e(round8to6(r),round8to6(g),round8to6(b));
			if (asSet.insert(e).second)
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
			asSet.size(),asVector.size(),mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b);
		median_cut(mini,maxi,0,asVector.size(),maxColors);
		compute_distance_field();
	}
	uint8_t remap(uint8_t r,uint8_t g,uint8_t b,uint8_t a) {
		if (a < 128)
			return 0;
		else
			return nearestIndex[round8to3(r)][round8to3(g)][round8to3(b)];
	}
	uint32_t computeDistanceTo(const palette &that) {
		uint32_t t = 0;
		// For each color in our palette look up the distance to any nearest color
		for (int i=1; i<=nextColor; i++) {
			auto &e = finalPalette[i];
			t += that.nearestDist[e.r][e.g][e.b];
		}
		return t;
	}
private:
	void compute_distance_field() {
		// Set everything to maximum distance
		memset(nearestDist,0xFF,sizeof(nearestDist));
		// And invalid (transparent) palette index
		memset(nearestIndex,0,sizeof(nearestIndex));
		auto sqr = [](int x) { return x * x; };
		// Could parallelize this; each slice is independent of any other
		for (uint8_t c=1; c<=nextColor; c++) {
			rgb &e = finalPalette[c];
			assert(e.r < 8 && e.g < 8 && e.b < 8);
			for (uint8_t i=0; i<8; i++) {
				for (uint8_t j=0; j<8; j++) {
					for (uint8_t k=0; k<8; k++) {
						uint32_t d = sqr(e.r-i) + sqr(e.g-j) + sqr(e.b-k);
						assert(d<sizeof(sqrt_tab));
						uint8_t x = sqrt_tab[d];
						if (x < nearestDist[i][j][k]) {
							nearestDist[i][j][k] = x;
							nearestIndex[i][j][k] = c;
						}
					}
				}
			}
		}
	}
	void median_cut(const rgb &mini,const rgb &maxi,size_t start,size_t end,int maxColors) {
		assert(maxColors);
		if (maxColors == 1) {
			// Assign everything in [mini,maxi) to a new color slot.
			auto &fp = finalPalette[++nextColor];
			assert(nextColor<=15);
			assert(end>start);
			int rSum = 0, gSum = 0, bSum = 0, found = 0;
			for (size_t i=start; i<end; i++) {
				rSum += asVector[i].r;
				gSum += asVector[i].g;
				bSum += asVector[i].b;
			}
			int delta = (int)(end - start);
			// Round the final color down now so that distance computations are coarser.
			fp.r = rgb::round6to3(rSum / delta);
			fp.g = rgb::round6to3(gSum / delta);
			fp.b = rgb::round6to3(bSum / delta);
			if (debug>1) printf("map [%d,%d,%d] - (%d,%d,%d) -> %d (%d,%d,%d)\n",mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b,nextColor,fp.r,fp.g,fp.b);
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
	if (argc<4) {
		fprintf(stderr,"args: manifest_name c_file palette_count\n");
		return 1;
	}
	FILE *m = fopen(argv[1],"r");
	int nPal = atoi(argv[3]);
	if (!m) {
		fprintf(stderr,"unable to open %s\n",argv[1]);
		return 1;
	}

	char lineBuf[1024];
	int lineNumber = 0;
	struct tile;
	struct sharedPalette: public palette {
		std::vector<uint32_t> clients; // List of clients using this palette
		uint8_t selfIndex = 0;
	};
	std::vector<sharedPalette*> sharedPalettes;
	struct tile {
		sharedPalette *sp;
		uint32_t offset;
		uint32_t workItem;
		uint16_t row, col;
	};
	struct workItem {
		char cfile_name[64];
		char c_sym[64];
		uint8_t *pixelData;
		int imageWidth;
		int tileWidth, tileHeight, firstCol, firstRow, numCols, numRows;
	};
	std::vector<tile> tiles;
	std::vector<workItem> workItems;
	while (fgets(lineBuf,sizeof(lineBuf),m)) {
		++lineNumber;
		if (lineBuf[0]=='#')
			continue;
		workItem i;
		char image_name[64];
		puts(lineBuf);
		if (sscanf(lineBuf,"%s %s %d %d %d %d %d %d",image_name,i.c_sym,
			&i.tileWidth,&i.tileHeight,&i.firstCol,&i.firstRow,&i.numCols,&i.numRows) != 8) {
			fprintf(stderr,"bad data on line %d\n",lineNumber);
			return 1;
		}

		FILE *f = fopen(image_name,"rb");
		if (!f) {
			fprintf(stderr,"unable to open tga '%s'\n",image_name);
			return 1;
		}
		targa_header hdr;
		fread(&hdr,sizeof(hdr),1,f);
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
	
		i.pixelData = read_targa_file(&hdr,f);
		fclose(f);
		if (!i.pixelData) {
			fprintf(stderr,"error reading image '%s'\n",image_name);
			return 1;
		}
		i.imageWidth = hdr.width;
		printf("processing %s...\n",image_name);
		for (int r=0; r<i.numRows; r++) {
			for (int c=0; c<i.numCols; c++) {
				sharedPalette p;
				uint32_t base = (i.imageWidth * (i.tileHeight * (i.firstRow + r)) + i.tileWidth * (i.firstCol + c)) * 4;
				for (int y=0; y<i.tileHeight; y++) {
					uint32_t o = base + 4 * y * i.imageWidth;
					for (int x=0; x<i.tileWidth; x++, o+=4)
						p.tally(i.pixelData[o+2],i.pixelData[o+1],i.pixelData[o+0],i.pixelData[o+3]);
				}
				p.median_cut(15);
				if (p.nextColor) {
					tile t;
					p.clients.push_back(tiles.size());
					t.sp = new sharedPalette(std::move(p));
					t.offset = base;
					t.workItem = workItems.size();
					t.row = r; t.col = c;
					sharedPalettes.push_back(t.sp);
					tiles.push_back(std::move(t));
				}
			}
		}
		workItems.push_back(i);
	}
	fclose(m);

	// Compute the "distance" between every palette. It is not transitive; the distance from A to B isn't necessarily from B to A.
	// This is because if A is a subset of B, its "distance" will be zero (or nearly so) while B's distance to A will be larger.
	// We just use a dumb algorithm here because any palette will never have more than 15 items, so even a dumb N-squared algorithm
	// is probably good enough -- famous last words.

	// First we look for all palettes with a distance of zero to another palette. These are easiest because we can just switch this
	// palette to use the other palette straight away. Once we cannot find any palettes of distance zero, we then look for the next
	// shortest distance. 

	printf("%zu non-empty tiles found across %zu files\n",tiles.size(),workItems.size());

	auto validate = [&](int line) {
		bool *b = (bool*)alloca(tiles.size());
		memset(b,0,tiles.size());
		for (auto &sp: sharedPalettes) {
			for (auto &t: sp->clients) {
				assert(t < tiles.size());
				assert(b[t] == false);
				assert(tiles[t].sp == sp);
				b[t] = true;
			}
		}
		bool *c = (bool*)memchr(b,false,tiles.size());
		if (c) {
			printf("tile %zu orphaned? (line %d)\n",c-b,line);
			assert(false);
		}
	};

	validate(__LINE__);

	// Initial pass to get our working set size down; look only for palettes that are subsets
	for (size_t i=0; i<sharedPalettes.size(); i++) {
		for (size_t j=0; j<sharedPalettes.size();) {
			if (i!=j && sharedPalettes[j]->computeDistanceTo(*sharedPalettes[i]) == 0) {
				if (debug) printf("palette %zu is a subset of palette %zu\n",j,i);
				// Transfer all clients from j to i
				validate(__LINE__);
				while (sharedPalettes[j]->clients.size()) {
					auto t = sharedPalettes[j]->clients.back();
					sharedPalettes[i]->clients.push_back(t);
					assert(tiles[t].sp == sharedPalettes[j]);
					tiles[t].sp = sharedPalettes[i];
					sharedPalettes[j]->clients.pop_back();
				}
				delete sharedPalettes[j];
				// Remove the palette and adjust outer loop index if necessary
				sharedPalettes.erase(sharedPalettes.begin() + j);
				if (i > j)
					--i;
				validate(__LINE__);
			}
			else
				++j;
		}
		validate(__LINE__);
	}
	printf("%zu palettes remain after initial pass\n",sharedPalettes.size());

	// Now enter the general version of the algorithm
	while (sharedPalettes.size() > nPal) {
		size_t bestI = ~0U, bestJ = ~0U;
		uint32_t bestDist = ~0U;
		for (size_t i=0; bestDist && i<sharedPalettes.size(); i++) {
			for (size_t j=0; bestDist && j<sharedPalettes.size(); j++) {
				if (i!=j) {
					uint32_t j2i = sharedPalettes[j]->computeDistanceTo(*sharedPalettes[i]);
					if (j2i < bestDist)
						bestI = i, bestJ = j, bestDist = j2i;
				}
			}
		}
		if (debug>1) printf("merging palette %zu into palette %zu\n",bestJ,bestI);
		// Transfer all clients from bestJ to bestI 
		while (sharedPalettes[bestJ]->clients.size()) {
			auto t = sharedPalettes[bestJ]->clients.back();
			sharedPalettes[bestI]->clients.push_back(t);
			assert(tiles[t].sp == sharedPalettes[bestJ]);
			tiles[t].sp = sharedPalettes[bestI];
			sharedPalettes[bestJ]->clients.pop_back();
		}
		
		delete sharedPalettes[bestJ];
		sharedPalettes.erase(sharedPalettes.begin() + bestJ);

		validate(__LINE__);

		// If it wasn't a proper subset, we need to (for best results!) regenerate the entire shared palette
		if (bestDist) {
		}
	}
	nPal = sharedPalettes.size();
	printf("reduced to %zu palettes\n",sharedPalettes.size());

	char buf[64];
	snprintf(buf,sizeof(buf),"%s.c",argv[2]);
	FILE *cfile = fopen(buf,"w");
	if (!cfile) {
		fprintf(stderr,"cannot create output file %s\n",buf);
		return 1;
	}
	snprintf(buf,sizeof(buf),"%s.h",argv[2]);
	FILE *hfile = fopen(buf,"w");
	if (!hfile) {
		fprintf(stderr,"cannot create output file %s\n",buf);
		return 1;
	}

	fprintf(cfile,"#include \"md_api.h\"\n");
	fprintf(cfile,"#include \"%s.h\"\n",argv[2]);

	for (int i=0; i<nPal; i++) {
		auto &p = *sharedPalettes[i];
		p.selfIndex = i;
		fprintf(cfile,"const uint16_t %s_palette_%d[16] = { ",argv[2],i);
		for (int j=0; j<16; j++)
			fprintf(cfile,"0x%03x,",p.finalPalette[j].encode());
		fprintf(cfile,"};\n");
		fprintf(hfile,"extern const uint16_t %s_palette_%d[];\n",argv[2],i);

		for (auto ti: p.clients) {
			auto &t = tiles[ti];
			workItem &w = workItems[t.workItem];
			std::string thisOne = std::string(w.c_sym) + "_" + std::to_string(t.col) + "_" + std::to_string(t.row);
			fprintf(cfile,"const uint32_t %s[] = {\n",thisOne.c_str());
			for (int c=0; c<w.tileWidth; c+=8) {
				for (int r=0; r<w.tileHeight; r+=8) {
					fprintf(cfile,"\t");
					for (int y=0; y<8; y++) {
						uint32_t pix = 0;
						uint32_t o = t.offset + 4 * (w.imageWidth*(r+y) + c);
						for (int x=0; x<8; x++,o+=4) {
							pix <<= 4;
							pix |= p.remap(w.pixelData[o+2],w.pixelData[o+1],w.pixelData[o+0],w.pixelData[o+3]);
						}
						fprintf(cfile,"0x%08x,",pix);
					}
					fprintf(cfile,"\n");
				}
			}
			fprintf(cfile,"};\n\n");
			fprintf(hfile,"extern const uint32_t %s[];\n",thisOne.c_str());
		}
	}
	fclose(cfile);
	fclose(hfile);
	return 0;
}
