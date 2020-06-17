CC=/usr/bin/x86_64-w64-mingw32-gcc

FLAGS=-ladvapi32 --static -Wl,--pic-executable,-e,main -w
OUT_NAME=/root/shad0w/modules/windows/dotnet/module.exe

SOURCES=main.c

x64: module

module:
	$(CC) $(SOURCES) -o $(OUT_NAME) $(FLAGS)