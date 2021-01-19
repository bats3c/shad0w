# 
# Display the current working directory
#
import json

__description__ = "Show the current working directory on a target"
__author__ = "@_batsec_"
__type__ = "file system"

EXEC_ID = 0x4000
OPCODE_PWD = 0x3000


def pwd_callback(shad0w, data):
    shad0w.debug.log(data, log=True, pre=False)

    return ""


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon.")
        return

    # make the json
    data = {"op": OPCODE_PWD, "args": "null"}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["callback"] = pwd_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, data)
