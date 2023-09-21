#include <string>
#include <algorithm>
#include <queue>
#include <set>
#include <assert.h>
#include <stdio.h>

#include "targa_header.h"

int debug = 0;

// map sqrt(i) to fixed point 4.4
const uint8_t sqrt_tab[49+49+49+1] = {
	0,16,23,28,32,36,39,42,45,48,51,53,55,58,60,62,64,66,68,70,72,73,75,77,78,80,82,83,85,86,88,89,91,92,93,95,96,
	97,99,100,101,102,104,105,106,107,109,110,111,112,113,114,115,116,118,119,120,121,122,123,124,125,126,127,128,
	129,130,131,132,133,134,135,136,137,138,139,139,140,141,142,143,144,145,146,147,148,148,149,150,151,152,153,
	153,154,155,156,157,158,158,159,160,161,162,162,163,164,165,166,166,167,168,169,169,170,171,172,172,173,174,175,
	175,176,177,177,178,179,180,180,181,182,182,183,184,185,185,186,187,187,188,189,189,190,191,191,192,193,193,194,
};

uint16_t bitrev(uint16_t x) {
	static unsigned char sn[] = { 
		0b0000, 0b1000, 0b0100, 0b1100, 
		0b0010, 0b1010, 0b0110, 0b1110,
		0b0001, 0b1001, 0b0101, 0b1101, 
		0b0011, 0b1011, 0b0111, 0b1111,
	};
	return (sn[x&15] << 12) | (sn[(x>>4)&15]<<8) | (sn[(x>>8)&15]<<4) | sn[x>>12];
}

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
	uint16_t encode() const {
		assert(r<8&&g<8&&b<8);
		return ((uint16_t)b << 9) | ((uint16_t)g << 5) | ((uint16_t)r << 1);
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
	static uint16_t round6to3(uint8_t ch) {
		// Knock six bits down to three
		return ch<59? ((ch+4) >> 3) : 7;
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
		if (debug) {
			printf("starting median cut, %zu(%zu) colors, ranged %d,%d,%d - %d,%d,%d\n",
				asSet.size(),asVector.size(),mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b);
			for (auto &e: asVector)
				printf("    color: %d,%d,%d (%d,%d,%d)\n",e.r,e.g,e.b,round6to3(e.r),round6to3(e.g),round6to3(e.b));
		}
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
			assert(e.r<8&&e.g<8&&e.b<8);
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
			const rgb &e = finalPalette[c];
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
		
		// sanity check the results
		for (uint8_t c=1; c<=nextColor; c++) {
			const rgb &e = finalPalette[c];
			// printf("c: %d rgb %d,%d,%d nI=%d nD=%f\n",c,e.r,e.g,e.b,nearestIndex[e.r][e.g][e.b],nearestDist[e.r][e.g][e.b]/16.0f);
			assert(nearestIndex[e.r][e.g][e.b] == c);
			assert(nearestDist[e.r][e.g][e.b] == 0);
		}
	}
	// return value is number of colors we used (might be zero if we match another entry due to low precision)
	int median_cut(const rgb &mini,const rgb &maxi,size_t start,size_t end,int maxColors) {
		// printf("median_cut %d,%d,%d-%d,%d,%d, [%zu,%zu) %d\n",mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b,start,end,maxColors);
		assert(maxColors);
		if (end-start==1 || maxColors == 1) {
			// Assign everything in [mini,maxi) to a new color slot.
			auto &fp = finalPalette[nextColor+1];
			assert(nextColor<=15);
			assert(end>start);
			int rSum = 0, gSum = 0, bSum = 0;
			for (size_t i=start; i<end; i++) {
				rSum += asVector[i].r;
				gSum += asVector[i].g;
				bSum += asVector[i].b;
			}
			int delta = (int)(end - start);
			// Round the final color down now so that distance computations are coarser.
			fp.r = round6to3(rSum / delta);
			fp.g = round6to3(gSum / delta);
			fp.b = round6to3(bSum / delta);
			// Make sure we didn't duplicate an existing palette slot
			for (int i=1; i<=nextColor; i++)
				if (fp.key==finalPalette[i].key)
					return 0;
			++nextColor;
			if (debug>1) printf("map [%d,%d,%d] - (%d,%d,%d) -> %d (%d,%d,%d)\n",mini.r,mini.g,mini.b,maxi.r,maxi.g,maxi.b,nextColor,fp.r,fp.g,fp.b);
			return 1;
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
		int used = 0;
		if (mid>start)
			used = median_cut(mini,miniMid,start,mid,maxColors>>1);
		if (end>mid)
			used += median_cut(midMaxi,maxi,mid,end,maxColors-used);
		return used;
	}
};

static size_t accum_in, accum_rle, accum_out;

class compressor {
	uint8_t *input;
	uint32_t *tally;
	uint8_t *index;
	size_t cursor,inputSize;
	uint8_t inputRange, symbolRange;	// also the value of the RLE symbol
	uint8_t maxRun;		// max run length; must either zero to disable RLE, or at least 3
	uint8_t prev, prevPrev;
	uint8_t runLength;
	static uint32_t headerTally[16];
public:
	compressor(size_t inputSize_,uint8_t inputRange_,uint8_t maxRun_) : inputSize(inputSize_), inputRange(inputRange_), maxRun(maxRun_) {
		assert(maxRun==0 || maxRun>=3);
		symbolRange = inputRange + (maxRun? maxRun-2 : 0);
		input = new uint8_t[inputSize];
		index = new uint8_t[symbolRange];
		std::fill(index,index+symbolRange,0);
		tally = new uint32_t[symbolRange];
		std::fill(tally,tally+symbolRange,0);
		cursor = 0;
		runLength = 0;
		prev = prevPrev = 0xFF;
	}
	void closeRun() {
		input[cursor++] = runLength-3+inputRange;
		input[cursor++] = prev;
		prevPrev = prev = 0xFF;
		runLength = 0;
	}
	void add(uint8_t s) {
		assert(cursor<inputSize);
		assert(s<inputRange);
		if (runLength) {
			if (prev!=s || runLength==maxRun) {
				closeRun();
				input[cursor++] = s;
			}
			else
				++runLength;
		}
		else if (prev==prevPrev && prev==s && maxRun) {
			runLength=3;
			cursor-=2;
		}
		else
			input[cursor++] = s;
		prevPrev = prev;
		prev = s;
	}

	struct tree_t {
		struct node_t {
			uint32_t popularity;
			uint8_t depth;
			uint8_t zeroChild, oneChild;	// if oneChild is 0xFF, it's a leaf node containing symbol zeroChild
		};
		std::vector<node_t> v;
		uint8_t newLeaf(uint32_t pop,uint8_t d,uint8_t z,uint8_t o) {
			// printf("node %zu popularity %u depth=%d %d/%d\n",v.size(),pop,d,z,o);
			v.emplace_back(node_t { pop, d, z, o });
			return v.size()-1;
		}
		uint8_t addLeaf(uint32_t pop,uint8_t sym) {
			return newLeaf(pop,0,sym,0xFF);
		}
		void raise(uint8_t n) {
			v[n].depth++;
			if (v[n].oneChild != 0xFF) {
				raise(v[n].zeroChild);
				raise(v[n].oneChild);
			}
		}
		uint8_t addNode(uint8_t c1,uint8_t c2) {
			assert(c2 != 0xFF);
			raise(c1);
			raise(c2);
			return newLeaf(v[c1].popularity + v[c2].popularity,
				std::max(v[c1].depth,v[c2].depth), c1, c2);
		}
	};
	
	// the best this is going to do is 4:1 compression without rle
	void emit(FILE *f) {
		if (runLength)
			closeRun();
		// tally all symbols, including the RLE lengths
		for (size_t i=0; i<cursor; i++)
			tally[input[i]]++;

		tree_t t;
		auto cmp = [&t](const uint8_t a,const uint8_t b) { 
			return t.v[a].popularity > t.v[b].popularity;
		};
		std::priority_queue<uint8_t,std::vector<uint8_t>,decltype(cmp)> q(cmp);

		// construct a min prio q and remember where each leaf node is.
		for (uint32_t i=0; i<symbolRange; i++)
			if (tally[i]) {
				q.push(index[i] = t.addLeaf(tally[i],1));
			}
		assert(q.size());
		// If there is only one node (possible only without RLE) then set its depth properly
		if (q.size()==1)
			t.v[0].depth = 1;
		else {
			while (q.size()>1) {
				auto a = q.top(); q.pop();
				auto b = q.top(); q.pop();
				q.push(t.addNode(a,b));
			}
		}
		// once we've built the tree, we only need to remember the code lengths.
		// the lengths themselves can be huffman encoded; zero in particular
		// is a common length.
		uint8_t headerLen = 0;
		uint32_t compressedBits = 0;
		//fprintf(f,"header[");
		static const uint8_t depthWidths[16] = { 1, 5, 4, 4, 4, 4, 4, 4, 5, 5, 5, 255, 255, 255, 255, 255 };
		static const uint8_t depthCodes[16] = { 0, 0b10000, 0b1010, 0b1011, 0b1100, 0b1101, 0b1110, 0b1111,
				0b10001, 0b10010, 0b10011 };
		struct code { uint8_t symbol; uint8_t width; uint16_t code; };
		std::vector<code> codes;
		for (uint8_t i=0; i<symbolRange; i++) {
			if (tally[i]) {
				// printf("entry %u (node %u) tally %u depth %u\n",i,index[i],tally[i],t.v[index[i]].depth);
				compressedBits += tally[i] * t.v[index[i]].depth;
				codes.emplace_back(code { i, t.v[index[i]].depth, 0});
				// assert(t.v[index[i]].depth < 7);
				headerLen += depthWidths[t.v[index[i]].depth];
				// fprintf(f,"%x",t.v[index[i]].depth);
				headerTally[t.v[index[i]].depth]++;
			}
			else {
				headerTally[0]++;
				codes.emplace_back(code { i, 0, 0 });
				// fprintf(f,"0");
				headerLen++;
			}
			/*if (i+1==inputRange && maxRun)
				fprintf(f,"|"); */
		}
		auto huff_string = [](uint32_t width,uint32_t code) {
			static char buf[33];
			assert(width && width<=32);
			buf[width] = 0;
			do {
				buf[--width] = '0' + (code & 1);
				code >>= 1;
			} while (width);
			return buf;
		};
		// Sort by width, then by symbol (all empty codes sort to the front)
		std::sort(codes.begin(),codes.end(),[](const code &a,const code &b) { 
			return a.width < b.width || (a.width == b.width && a.symbol < b.symbol); });
		for (int i=0; i<codes.size()-1; i++) {
			if (codes[i].width) {
				codes[i+1].code = (codes[i].code + 1) << (codes[i+1].width - codes[i].width);
				// fprintf(f,"// symbol %d is %s\n",codes[i].symbol,huff_string(codes[i].width,codes[i].code));
			}
		}
		// Re-sort by symbol (original symbol order)
		std::sort(codes.begin(),codes.end(),[](const code &a,const code &b) { return a.symbol < b.symbol; });

		uint16_t bitBuffer = 0, currentShift = 16, counter = 0;
		auto emit_bits = [&](uint8_t width,uint16_t code) {
			assert(width);
			if (currentShift > width) {
				currentShift -= width;
				bitBuffer |= code << currentShift;
			}
			else {
				bitBuffer |= code >> (width - currentShift);
				fprintf(f,"%s0x%04x,%s",(counter&15)==0?"\t":"",bitrev(bitBuffer),(counter&15)==15?"\n":" ");
				++counter;
				currentShift = currentShift + 16 - width;
				bitBuffer = currentShift==16? 0 : code << currentShift;
			}
		};
		// Emit the header (a run of code widths that themselves are huffman encoded)
		//for (uint8_t i=0; i<symbolRange; i++)
		//	emit_bits(depthWidths[codes[i].width],depthCodes[codes[i].width]);

		// Emit the actual data
		for (uint32_t i=0; i<cursor; i++) {
			assert(codes[input[i]].symbol == input[i]);
			emit_bits(codes[input[i]].width,codes[input[i]].code);
		}

		// Flush the bit buffer
		emit_bits(15,0);

		if ((counter&15)) 
			fputc('\n',f);
		fprintf(f,"\t// %zu bits compressed to %zu bits via RLE, then %u+%u=%u bits\n",inputSize*4,cursor*4,headerLen,compressedBits,headerLen+compressedBits);	
		accum_in += inputSize * 4;
		accum_rle += cursor * 4;
		accum_out += headerLen + compressedBits;
	}
	static void finalize() {
		tree_t t;
		auto cmp = [&t](const uint8_t a,const uint8_t b) { 
			return t.v[a].popularity > t.v[b].popularity;
		};
		std::priority_queue<uint8_t,std::vector<uint8_t>,decltype(cmp)> q(cmp);

		// construct a min prio q and remember where each leaf node is.
		uint8_t index[16];
		for (uint32_t i=0; i<16; i++)
			if (headerTally[i])
				q.push(index[i] = t.addLeaf(headerTally[i],i));
		assert(q.size());
		while (q.size()>1) {
			auto a = q.top(); q.pop();
			auto b = q.top(); q.pop();
			q.push(t.addNode(a,b));
		}
		for (uint32_t i=0; i<16; i++)
			if (headerTally[i])
				printf("depth %d encodes in %d bits\n",i,t.v[index[i]].depth);
	}
};
uint32_t compressor::headerTally[16];
/*
depth 0 encodes in 1 bits
depth 1 encodes in 6 bits
depth 2 encodes in 4 bits
depth 3 encodes in 4 bits
depth 4 encodes in 4 bits
depth 5 encodes in 3 bits
depth 6 encodes in 4 bits
depth 7 encodes in 4 bits
depth 8 encodes in 5 bits
depth 9 encodes in 7 bits
depth 10 encodes in 7 bits

0 -> 0
2/3/4/5/6/7 -> 1010,1011,1100,1101,1110,1111
1/8/9/10 -> 10000,10001,10010,10011
*/

int main(int argc,char** argv) {
	if (argc<4) {
		fprintf(stderr,"args: manifest_name c_file palette_count max_rle_run\n");
		return 1;
	}
	FILE *m = fopen(argv[1],"r");
	int nPal = atoi(argv[3]);
	int maxRun = atoi(argv[4]);
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
		uint8_t workItem;
		uint8_t row, col;
		char name[32-15];
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
		// puts(lineBuf);
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
		if (debug)printf("processing %s...\n",image_name);
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
					snprintf(t.name,sizeof(t.name),"%s_%d_%d",i.c_sym,r,c);
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

#if 1
	auto validate = [](int) { };
#else
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
#endif

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
			auto &p = *sharedPalettes[bestI];
			p.reset();
			for (auto ti: p.clients) {
				auto &t = tiles[ti];
				auto &i = workItems[t.workItem];
				for (int r=0; r<i.numRows; r++) {
					for (int c=0; c<i.numCols; c++) {
						for (int y=0; y<i.tileHeight; y++) {
							uint32_t o = t.offset + 4 * y * i.imageWidth;
							for (int x=0; x<i.tileWidth; x++, o+=4)
								p.tally(i.pixelData[o+2],i.pixelData[o+1],i.pixelData[o+0],i.pixelData[o+3]);
						}
					}
				}
			}
			p.median_cut(15);
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
	}

	bool doCompress = strcmp(argv[4],"none");
	for (auto &t: tiles) {
		auto &p = *t.sp;
		workItem &w = workItems[t.workItem];
		fprintf(cfile,"const uint16_t %s[] = {\n",t.name);
		compressor k(w.tileWidth * w.tileHeight,16,maxRun);
		for (int c=0; c<w.tileWidth; c+=8) {
			for (int r=0; r<w.tileHeight; r+=8) {
				if (!doCompress)
					fprintf(cfile,"\t");
				for (int y=0; y<8; y++) {
					uint32_t pix = 0;
					uint32_t o = t.offset + 4 * (w.imageWidth*(r+y) + c);
					for (int x=0; x<8; x++,o+=4) {
						uint8_t thisPix = p.remap(w.pixelData[o+2],w.pixelData[o+1],w.pixelData[o+0],w.pixelData[o+3]);
						pix = (pix << 4) | thisPix;
						if (doCompress)
							k.add(thisPix);
					}
					if (!doCompress)
						fprintf(cfile,"0x%04x,0x%04x, ",pix>>16,(uint16_t)pix);
				}
				if (!doCompress)
					fprintf(cfile,"\n");
			}
		}
		if (doCompress)
			k.emit(cfile);
		fprintf(cfile,"};\n");
		fprintf(hfile,"extern const uint16_t %s[];\n",t.name);
	}

	if (doCompress)
		compressor::finalize();

	fprintf(hfile,"enum %s_enum {\n",argv[2]);
	fprintf(cfile,"\nconst uint32_t %s_directory[] = {\n",argv[2]);
	for (auto &t: tiles) {
		fprintf(hfile,"\t%s_%s,\n",argv[2],t.name);
		fprintf(cfile,"\t(uint32_t)%s + (%d<<29),\n",t.name,t.sp->selfIndex);
	}
	fprintf(cfile,"};\nconst uint16_t %s_directory_size = %zd;\n",argv[2],tiles.size());
	fprintf(hfile,"};\n");

	fprintf(hfile,"extern const uint32_t %s_directory[];\n",argv[2]);
	fprintf(hfile,"extern const uint16_t %s_directory_size;\n",argv[2]);

	fclose(cfile);
	fclose(hfile);

	printf("total in %zu bits, %zu bits after RLE, %zu total output, %zu%% of orig size\n",accum_in,accum_rle,accum_out,
		(accum_out*100)/accum_in);
	return 0;
}
