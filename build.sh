# brew install rosco-m68k/toolchain/binutils-cross-m68k
# brew install rosco-m68k/toolchain/gcc-cross-m68k@13  
set -e

# fnotree... stops the compiler from synthesizing non-existent calls to memset/memcpy
clang fixrom.cpp -lc++ -o fixrom
clang -O3 -std=c++11 mkpal.cpp -lc++ -o mkpal
./mkpal
/opt/homebrew/bin/m68k-elf-gcc -DNDEBUG -Wno-multichar -march=68000 -fno-tree-loop-distribute-patterns -fomit-frame-pointer -O3 -c vectors.c header.c start.c joypad.c video.c tiles.c
/opt/homebrew/bin/m68k-elf-ld -T md.script --oformat=binary vectors.o header.o start.o video.o joypad.o tiles.o -o megarogue.rom
~/Documents/personal/megarogue/fixrom megarogue.rom
/opt/homebrew/bin/m68k-elf-ld -T md.script vectors.o header.o start.o video.o joypad.o tiles.o
/opt/homebrew/bin/m68k-elf-objdump --disassemble a.out > megarogue.txt
# ~/Downloads/blastem-osx-0.6.2/dis megarogue.rom
~/Downloads/blastem-osx-0.6.2/blastem megarogue.rom

