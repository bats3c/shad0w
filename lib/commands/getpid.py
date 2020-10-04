#
# Get the current pid info
#
import json

__description__ = "Get the current process info"
__author__ = "@_batsec_"

# beacon to exec command on
current_beacon = None

EXEC_ID    = 0x4000
OPCODE_PID = 0x7000

def getpid_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data)

    return ""

def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    # make the json
    data = {"op" : OPCODE_PID, "args": "null"}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[current_beacon]["callback"] = getpid_callback
    shad0w.beacons[current_beacon]["task"] = (EXEC_ID, data)