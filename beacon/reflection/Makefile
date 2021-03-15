VARIABLES=-DSECURE

WIN_CC=/usr/bin/x86_64-w64-mingw32-gcc

WIN_FLAGS=-Wl,-Bstatic -lwinhttp -Wl,-Bstatic -lwtsapi32 -Wl,-Bstatic -lsecur32 $(VARIABLES) -DREFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN -shared -DWIN_X64 --static -w

WIN_EXE_OUT=../beacon.dll

WIN_C_SOURCES=debug.c core.c beacon.c syscalls.c loader.c ReflectiveDll.c ReflectiveLoader.c

WIN_JSONLIB_DIR=../lib/json/
WIN_JSONLIB_SOURCES=$(WIN_JSONLIB_DIR)jwrite.c $(WIN_JSONLIB_DIR)jread.c

WIN_STDLIB_DIR=../lib/stdlib/
WIN_STDLIB_SOURCES=$(WIN_STDLIB_DIR)listdirs.c $(WIN_STDLIB_DIR)readfile.c \
				   $(WIN_STDLIB_DIR)makedirectory.c $(WIN_STDLIB_DIR)removefile.c \
				   $(WIN_STDLIB_DIR)getdir.c $(WIN_STDLIB_DIR)changedir.c $(WIN_STDLIB_DIR)getpid.c \
				   $(WIN_STDLIB_DIR)getps.c
WIN_STDLIB_OUT=stdlib.lib

WIN_STDLIB_FLAGS=-static

WIN_VITAL_SOURCES=base64.c

x64: win_beacon_64
x64_debug: win_beacon_64

win_stdlib_64:
	$(WIN_CC) -c $(WIN_STDLIB_SOURCES) $(WIN_STDLIB_FLAGS) $(WIN_STDLIB_FLAGS)

win_beacon_64:
	$(WIN_CC) $(WIN_STDLIB_SOURCES) $(WIN_VITAL_SOURCES) $(WIN_JSONLIB_SOURCES) $(WIN_C_SOURCES) -o $(WIN_EXE_OUT) $(WIN_LINK) $(WIN_FLAGS)