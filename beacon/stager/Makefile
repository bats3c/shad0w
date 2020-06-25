VARIABLES=

WIN_CC=/usr/bin/x86_64-w64-mingw32-gcc

WIN_HIDE_FLAGS=-Wl,-subsystem,windows
WIN_FLAGS=-lwinhttp -Wl,-Bstatic -lwtsapi32 -Wl,-Bstatic $(VARIABLES) --static -Wl,--pic-executable,-e,main -w

WIN_EXE_OUT=../beacon.exe

WIN_C_SOURCES=debug.c callback.c syscalls.c loader.c main.c

WIN_SECURE_DIR=../lib/secure/
WIN_SECURE_SOURCES=$(WIN_SECURE_DIR)main.c

WIN_VITAL_SOURCES=base64.c crypt.c

x64: win_beacon_x64
x64_debug: win_beacon_x64_debug

win_beacon_x64_debug:
	$(WIN_CC) $(WIN_VITAL_SOURCES) $(WIN_SECURE_SOURCES) $(WIN_C_SOURCES) -o $(WIN_EXE_OUT) $(WIN_LINK) $(WIN_FLAGS)

win_beacon_x64:
	$(WIN_CC) $(WIN_VITAL_SOURCES) $(WIN_SECURE_SOURCES) $(WIN_C_SOURCES) -o $(WIN_EXE_OUT) $(WIN_LINK) $(WIN_FLAGS) $(WIN_HIDE_FLAGS)