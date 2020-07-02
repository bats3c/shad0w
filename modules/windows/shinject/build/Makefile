# x86_64-w64-mingw32-gcc ReflectiveDll.c ReflectiveLoader.c -o msgbox.dll -DREFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN -DREFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR -DWIN_X64 -shared -static

CC=/usr/bin/x86_64-w64-mingw32-gcc

FLAGS=-DREFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN -DREFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR -DWIN_X64 -shared -static
OUT_NAME=/root/shad0w/modules/windows/shinject/module.dll

SOURCES=ReflectiveDll.c ReflectiveLoader.c

x64: module

module:
	$(CC) $(SOURCES) -o $(OUT_NAME) $(FLAGS)
