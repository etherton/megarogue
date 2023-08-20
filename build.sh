# brew install rosco-m68k/toolchain/binutils-cross-m68k
# brew install rosco-m68k/toolchain/gcc-cross-m68k@13  

/opt/homebrew/bin/m68k-elf-gcc -DNDEBUG -Wno-multichar -march=68000 -fomit-frame-pointer -Os -c vectors.c header.c start.c joypad.c video.c && \
/opt/homebrew/bin/m68k-elf-ld -T md.script --oformat=binary vectors.o header.o start.o video.o joypad.o -o megarogue.rom && \
/opt/homebrew/bin/m68k-elf-ld -T md.script vectors.o header.o start.o video.o joypad.o && \
/opt/homebrew/bin/m68k-elf-objdump --disassemble a.out
~/Downloads/blastem-osx-0.6.2/dis megarogue.rom
~/Downloads/blastem-osx-0.6.2/blastem megarogue.rom > /dev/null

