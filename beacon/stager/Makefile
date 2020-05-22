VARIABLES=

WIN_CC=/usr/bin/x86_64-w64-mingw32-gcc
WIN_ASM_CC=/usr/bin/nasm -f win64

WIN_HIDE_FLAGS=-Wl,-subsystem,windows
WIN_FLAGS=-lwinhttp -Wl,-Bstatic -lwtsapi32 -Wl,-Bstatic $(VARIABLES) --static -Wl,--pic-executable,-e,main -w

WIN_EXE_OUT=../beacon.exe
WIN_LIB_OUT=syscalls64.lib

WIN_LINK=-L./ -lsyscalls64

WIN_ASM_SOURCES=syscalls64.asm

WIN_C_SOURCES=debug.c base64.c callback.c loader.c main.c

WIN_SECURE_DIR=../lib/secure/
WIN_SECURE_SOURCES=$(WIN_SECURE_DIR)main.c

x64: win_beacon_x64
x64_debug: win_beacon_x64_debug

win_beacon_x64_debug:
	$(WIN_ASM_CC) $(WIN_ASM_SOURCES) -o $(WIN_LIB_OUT)
	$(WIN_CC) $(WIN_SECURE_SOURCES) $(WIN_C_SOURCES) -o $(WIN_EXE_OUT) $(WIN_LINK) $(WIN_FLAGS)

win_beacon_x64:
	$(WIN_ASM_CC) $(WIN_ASM_SOURCES) -o $(WIN_LIB_OUT)
	$(WIN_CC) $(WIN_SECURE_SOURCES) $(WIN_C_SOURCES) -o $(WIN_EXE_OUT) $(WIN_LINK) $(WIN_FLAGS) $(WIN_HIDE_FLAGS)