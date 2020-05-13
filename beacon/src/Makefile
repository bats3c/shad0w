# when compling the json stuffs:
# apt install autoconf automake libtool
# ./autogen.sh
# ./configure --host=x86_64-w64-mingw32 --disable-static --enable-shared LDFLAGS=-static
# make install

WIN_CC=/usr/bin/x86_64-w64-mingw32-gcc
WIN_ASM_CC=/usr/bin/nasm -f win64

WIN_LINK=-L../lib/json-c/.libs/ -L./ -ljson-c -lsyscalls64
WIN_FLAGS=-Wl,-Bstatic -lwinhttp -Wl,-Bstatic -lwtsapi32 -Wl,-Bstatic -lsecur32 -DWIN --static -Wl,--pic-executable,-e,main -w

WIN_EXE_OUT=../beacon.exe
WIN_LIB_OUT=syscalls64.lib

WIN_C_SOURCES=debug.c core.c base64.c beacon.c loader.c
WIN_ASM_SOURCES=syscalls64.asm

WIN_STDLIB_DIR=../lib/stdlib/
WIN_STDLIB_SOURCES=$(WIN_STDLIB_DIR)listdirs.c $(WIN_STDLIB_DIR)readfile.c \
				   $(WIN_STDLIB_DIR)makedirectory.c $(WIN_STDLIB_DIR)removefile.c \
				   $(WIN_STDLIB_DIR)getdir.c $(WIN_STDLIB_DIR)changedir.c $(WIN_STDLIB_DIR)whoami.c
WIN_STDLIB_FLAGS=-static -shared -w
WIN_STDLIB_OUT=stdlib.lib

all: win_beacon

win_stdlib:
	$(WIN_CC) -c $(WIN_STDLIB_SOURCES) $(WIN_STDLIB_FLAGS) $(WIN_STDLIB_FLAGS)

win_beacon:
	$(WIN_ASM_CC) $(WIN_ASM_SOURCES) -o $(WIN_LIB_OUT)
	$(WIN_CC) $(WIN_STDLIB_SOURCES) $(WIN_C_SOURCES) -o $(WIN_EXE_OUT) $(WIN_LINK) $(WIN_FLAGS)