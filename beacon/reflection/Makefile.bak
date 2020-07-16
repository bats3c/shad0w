#CC=i686-w64-mingw32-gcc
CC=x86_64-w64-mingw32-gcc

INJ_DIR=inject/
DLL_DIR=dll/

#ARCH=WIN_x86
ARCH=WIN_X64

all: $(DLL_DIR)ReflectiveLoader $(INJ_DIR)Inject

$(DLL_DIR)ReflectiveLoader:
	$(CC) $(DLL_DIR)ReflectiveDll.c $(DLL_DIR)ReflectiveLoader.c -o reflective_$(ARCH).dll -DREFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN -D$(ARCH) -shared

setmem:
	$(shell xxd -i reflective_$(ARCH).dll > $(INJ_DIR)memory.h)
	$(shell mv reflective_$(ARCH).dll $(DLL_DIR))

$(INJ_DIR)Inject:
	$(CC) $(INJ_DIR)GetProcAddressR.c $(INJ_DIR)LoadLibraryR.c $(INJ_DIR)Inject.c -o reflct_dll_inject.exe -O2 -D$(ARCH) -w
