# ../llvm-project/.build/bin/clang -target m68k-apple-macosx13.0.0 -c vectors.c header.c start.c joypad.c video.c
#../llvm-project/.build/bin/clang -DNDEBUG -target m68k-apple-macosx13.0.0 -fomit-frame-pointer -O3 -c vectors.c header.c start.c joypad.c video.c
# ../llvm-project/.build/bin/ld.lld -T md.script --oformat=binary vectors.o header.o start.o video.o joypad.o

/opt/homebrew/bin/m68k-elf-gcc -DNDEBUG -Wno-multichar -march=68000 -fomit-frame-pointer -Os -c vectors.c header.c start.c joypad.c video.c && \
/opt/homebrew/bin/m68k-elf-ld -T md.script --oformat=binary vectors.o header.o start.o video.o joypad.o -o megarogue.rom && \
/opt/homebrew/bin/m68k-elf-ld -T md.script vectors.o header.o start.o video.o joypad.o && \
/opt/homebrew/bin/m68k-elf-objdump --disassemble a.out
~/Downloads/blastem-osx-0.6.2/dis megarogue.rom
~/Downloads/blastem-osx-0.6.2/blastem megarogue.rom > /dev/null

