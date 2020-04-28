WIN_CC=/usr/bin/x86_64-w64-mingw32-gcc

# when compling the json stuffs:
# apt install autoconf automake libtool
# ./autogen.sh
# ./configure --host=x86_64-w64-mingw32 --disable-static --enable-shared LDFLAGS=-static
# make install

WIN_FLAGS=-L../lib/json-c/.libs/ -ljson-c -Wl,-Bstatic -lwinhttp -Wl,-Bstatic -DWIN -lwtsapi32 --static -Wl,--pic-executable,-e,main -w
WIN_OUTNAME=../beacon.exe

SOURCES=debug.c core.c base64.c beacon.c loader.c

all: clean beacon

clean:
	$(shell rm $(WIN_OUTNAME))

beacon:
	$(WIN_CC) $(SOURCES) -o $(WIN_OUTNAME) $(WIN_FLAGS)