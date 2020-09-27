# 
# Display the current working directory
#
import json

__description__ = "Display the current working directory of the beacon"
__author__ = "@_batsec_"

EXEC_ID    = 0x4000
OPCODE_PWD = 0x3000

# beacon to exec command on
current_beacon = None

def pwd_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data.strip())

    return ""

def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    # make the json
    data = {"op" : OPCODE_PWD, "args": "null"}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[current_beacon]["callback"] = pwd_callback
    shad0w.beacons[current_beacon]["task"] = (EXEC_ID, data)