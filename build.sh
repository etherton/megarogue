# brew install rosco-m68k/toolchain/binutils-cross-m68k
# brew install rosco-m68k/toolchain/gcc-cross-m68k@13  
set -e

# fnotree... stops the compiler from synthesizing non-existent calls to memset/memcpy
/opt/homebrew/bin/m68k-elf-gcc -DNDEBUG -Wno-multichar -march=68000 -fno-tree-loop-distribute-patterns -fomit-frame-pointer -O3 -c vectors.c header.c start.c joypad.c video.c
/opt/homebrew/bin/m68k-elf-ld -T md.script --oformat=binary vectors.o header.o start.o video.o joypad.o -o megarogue.rom
/opt/homebrew/bin/m68k-elf-ld -T md.script vectors.o header.o start.o video.o joypad.o
/opt/homebrew/bin/m68k-elf-objdump --disassemble a.out > megarogue.txt
# ~/Downloads/blastem-osx-0.6.2/dis megarogue.rom
~/Downloads/blastem-osx-0.6.2/blastem megarogue.rom > /dev/null

