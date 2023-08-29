:r buil

all: fixrom mkpal megarogue.rom run

fixrom: fixrom.cpp
	clang fixrom.cpp -lc++ -o fixrom

mkpal: mkpal.cpp
	clang -O3 -std=c++11 mkpal.cpp -lc++ -o mkpal

maze.c: md_api.h md_math.h maze.h

joypad.c: md_api.h

start.c: md_api.h md_math.h maze.h

# tga_name c_name c_sym start_x start_y cell_w cell_h cells_across cells_down palette_group_cell_width palette_group_cell_height color_bits_per_channel

char_tiles.c: mkpal
	./mkpal oryx_16bit_fantasy_creatures_trans.tga char_tiles.c char 24 24 24 24 18 22 1 2 2

bg_tiles.c: mkpal
	./mkpal oryx_16bit_fantasy_world_trans.tga bg_tiles.c bg 24 24 24 24 27 23 27 1 3

# brew install rosco-m68k/toolchain/binutils-cross-m68k
# brew install rosco-m68k/toolchain/gcc-cross-m68k@13  
# fnotree... stops the compiler from synthesizing non-existent calls to memset/memcpy

megarogue.rom: vectors.c header.c start.c joypad.c video.c char_tiles.c bg_tiles.c maze.c fixrom
	/opt/homebrew/bin/m68k-elf-gcc -DNDEBUG -Wno-multichar -march=68000 -fno-tree-loop-distribute-patterns -fomit-frame-pointer -O1 -c vectors.c header.c start.c joypad.c video.c char_tiles.c bg_tiles.c maze.c
	/opt/homebrew/bin/m68k-elf-ld -T md.script vectors.o header.o start.o video.o joypad.o char_tiles.o bg_tiles.o maze.o -o megarogue
	/opt/homebrew/bin/m68k-elf-objdump --disassemble megarogue > megarogue.txt
	/opt/homebrew/bin/m68k-elf-ld -T md.script --oformat=binary vectors.o header.o start.o video.o joypad.o char_tiles.o bg_tiles.o maze.o -o megarogue.rom
	./fixrom megarogue.rom

run: megarogue.rom
	~/Downloads/blastem-osx-0.6.2/dis megarogue.rom > mdr.txt
	~/Downloads/blastem-osx-0.6.2/blastem megarogue.rom

debug: megarogue.rom
	nm megarogue | grep " T "
	~/Downloads/blastem-osx-0.6.2/blastem -d megarogue.rom

