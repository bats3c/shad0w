CC=/usr/bin/x86_64-w64-mingw32-gcc

FLAGS=-L/root/shad0w/beacon/lib/json-c/.libs/ -ljson-c -Wl,-Bstatic -lwinhttp -Wl,-Bstatic -DWIN -lwtsapi32 --static -Wl,--pic-executable,-e,main -w
OUT_NAME=/root/shad0w/modules/windows/ls/module.exe

SOURCES=main.c

all: module

module:
	$(CC) $(SOURCES) -o $(OUT_NAME) $(FLAGS)