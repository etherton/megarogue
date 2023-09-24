:r buil

all: fixrom mkpal megarogue.rom run

fixrom: fixrom.cpp
	clang fixrom.cpp -lc++ -o fixrom

mkpal: mkpal.cpp targa_header.h huffman_decode.h
	clang -Wall -O3 -std=c++11 mkpal.cpp -lc++ -o mkpal

mkpald:
	clang -Wall -O0 -DQUICK -g -std=c++11 mkpal.cpp -lc++ -o mkpal
	lldb mkpal tile_manifest.txt tiles 4 0

maze.o: maze.c md_api.h md_math.h maze.h

joypad.o: joypad.c md_api.h

video.o: video.c md_api.h

start.o: start.c md_api.h

main.o: main.c md_api.h md_math.h maze.h tiles.h font8x8_basic.h

tiles.c tiles.h: mkpal tile_manifest.txt
	# ./mkpal tile_manifest.txt tiles 4 10
	./mkpal tile_manifest.txt tiles 4 10

# brew install rosco-m68k/toolchain/binutils-cross-m68k
# brew install rosco-m68k/toolchain/gcc-cross-m68k@13  
# fnotree... stops the compiler from synthesizing non-existent calls to memset/memcpy

OBJS = vectors.o header.o start.o main.o joypad.o video.o tiles.o maze.o

%.o: %.c
	/opt/homebrew/bin/m68k-elf-gcc -DNDEBUG -Wno-multichar -march=68000 -fno-tree-loop-distribute-patterns -O3 -c $<

megarogue.rom: $(OBJS) fixrom
	/opt/homebrew/bin/m68k-elf-ld -T md.script $(OBJS) -o megarogue
	/opt/homebrew/bin/m68k-elf-objdump --disassemble megarogue > megarogue.txt
	/opt/homebrew/bin/m68k-elf-ld -T md.script --oformat=binary $(OBJS) -o megarogue.rom
	./fixrom megarogue.rom

run: megarogue.rom
	# ~/Downloads/blastem-osx-0.6.2/dis megarogue.rom
	~/Downloads/blastem-osx-0.6.2/blastem megarogue.rom > /dev/null

debug: megarogue.rom
	nm megarogue | grep " T "
	~/Downloads/blastem-osx-0.6.2/blastem -d megarogue.rom 

