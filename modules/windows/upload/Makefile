CC=/usr/bin/x86_64-w64-mingw32-gcc

FLAGS=-lwinhttp -Wl,-Bstatic --static -Wl,--pic-executable,-e,main -w
OUT_NAME=/root/shad0w/modules/windows/upload/module.exe

SOURCES=main.c

x64: module

module:
	$(CC) $(SOURCES) -o $(OUT_NAME) $(FLAGS)