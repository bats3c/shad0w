# Format the created beacons

import tempfile

from lib import buildtools

from lib.templates import exe
from lib.templates import dll
from lib.templates import powershell

formats = ('raw', 'exe', 'psh', 'dll')

def get_size(filename):
    # get the bytes of the exe
    with open(filename, 'rb') as file:
        return len(file.read())

def format_raw(builder, length=True, code=False):
    # extract the shellcode from the new beacon
    rcode = buildtools.extract_shellcode()

    # write the shellcode
    buildtools.write_and_bridge(builder.outfile, rcode)

    if length:
        return get_size(builder.outfile)

    if code:
        return rcode

def format_exe(builder, length=True, code=False):
    # get the bytes of the exe
    # with open("/root/shad0w/beacon/beacon.exe", 'rb') as file:
    #     rcode = file.read()

    # then give them the exe and bridge it
    # length = buildtools.write_and_bridge(builder.outfile, rcode)

    # get the the beacon shellcode
    rcode = format_raw(builder, length=False, code=True)

    # create an exe from the shellcode
    ecode = exe.generate(rcode, debug=builder.debugv)

    length = buildtools.write_and_bridge(builder.outfile, ecode)

    # shrink the finally binary
    if not builder.no_shrink:
        length = buildtools.shrink_exe(builder.outfile)

    if length:
        return length

    if code:
        with open(builder.outfile, 'rb') as file:
            return file.read()

def format_powershell(builder, length=True, code=False):

    outfile = builder.outfile

    with tempfile.NamedTemporaryFile(dir='/tmp', delete=False) as tmpfile:
        temp_file_name = tmpfile.name

    builder.outfile = temp_file_name
    rcode = format_raw(builder, length=False, code=True)

    # insert into the powershell template
    pcode = powershell.generate(rcode)

    with open(outfile, "w") as file:
        file.write(pcode)

    builder.outfile = outfile

    if length:
        return len(pcode)

    if code:
        return pcode

def format_dll(builder, length=True, code=False):

    # get the the beacon shellcode
    rcode = format_raw(builder, length=False, code=True)

    # create an exe from the shellcode
    ecode = dll.generate(rcode, debug=builder.debugv)

    length = buildtools.write_and_bridge(builder.outfile, ecode)

    if length:
        return length

    if code:
        with open(builder.outfile, 'rb') as file:
            return file.read()

def create(builder):
    if builder.format == "raw":
        return format_raw(builder)

    if builder.format == "exe":
        return format_exe(builder)

    if builder.format == "dll":

        if builder.static == None:
            print("[!] Staged dll payloads are not currently supported")
            return False

        return format_dll(builder)

    if builder.format == "psh":

        if builder.static == None:
            print("[!] Staged powershell payloads are not currently supported")
            return False

        return format_powershell(builder)