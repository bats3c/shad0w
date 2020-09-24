#
# Kill the beacon
#

__description__ = "Kill the current beacon"
__author__ = "@_batsec_"

# identify the task as shellcode execute
from lib.basecommand import BaseCommand

DIE_ID = 0x6000


class DieCommand(BaseCommand):
    def __init__(self, args):
        BaseCommand.__init__(self, "die", args)

    def parse_parameters(self):
        self.parser.add_argument("-y", "--yes", action='store_true', help="Confirm the death of the beacon")

    def get_usage(self):
        return """

Examples:

die
die -y
"""

    def run(self, shad0w):
        # check that the user has confirmed
        if not self.args.yes:
            shad0w.debug.log("Confirm you want to kill the current beacon with 'die -y'", log=True)
            return

        # tell the beacon to die
        if self.args.yes:
            shad0w.beacons[shad0w.current_beacon]["task"] = (DIE_ID, None)


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    cmd = DieCommand(args)
    if cmd.parse():
        cmd.run(shad0w)

