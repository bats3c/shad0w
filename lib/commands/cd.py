# 
# Change the current directory
#

import json

__description__ = "Change the beacons working directory on the target"
__author__ = "@_batsec_"

from lib.basecommand import BaseCommand

EXEC_ID   = 0x4000
OPCODE_CD = 0x6000


def get_list_directory(rargs, args):
    # resolve the directory we need to list

    # if we got no other args but 'cd' then drop the current dir
    if ''.join(rargs) == 'cd':
        return "."

    elif type(args.dir) == list:
        return ' '.join(args.dir).replace('"', '')
    
    elif args.dir is not None:
        return args.dir

    return None


class CdCommand(BaseCommand):
    def __init__(self, args):
        BaseCommand.__init__(self, "cd", args)

    def parse_parameters(self):
        self.parser.add_argument("dir", nargs='*', help="Location of the dir you want to change to")

    def get_usage(self):
        return """

Examples:

cd C:\\
cd "C:\\Documents and Settings"
"""

    def run(self, shad0w):
        # find the dir we want to list
        _dir = get_list_directory(self.raw_args, self.args)

        # make the json
        data = {"op": OPCODE_CD, "args": _dir}
        data = json.dumps(data)

        # set a task for the current beacon to do
        shad0w.beacons[shad0w.current_beacon]["callback"] = cd_callback
        shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, data)


def cd_callback(shad0w, data):
    shad0w.debug.log(data, log=True, pre=False)

    return ""


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return

    cmd = CdCommand(args)
    if cmd.parse():
        cmd.run(shad0w)
