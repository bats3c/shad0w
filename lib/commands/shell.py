# OPSEC Unsafe

#
# spawn cmd.exe with arguments given
#

__description__ = "Execute shell commands via cmd.exe on target"
__author__ = "@_batsec_"


# identify task as cmd.exe spawn

_SHELL_ID = 0x1000

def main(shad0w, args):

    # check we have an active beacon

    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    # set a task for the current beacon to do

    shad0w.beacons[shad0w.current_beacon]["task"] = (_SHELL_ID, ' '.join(args[1:]))

    # inform the user of the change

    shad0w.debug.log(f"Tasked beacon ({shad0w.current_beacon})", log=True)
