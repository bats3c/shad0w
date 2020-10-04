#
# Change the current directory
#

import json
import argparse

__description__ = "Change the beacons working directory on the target"
__author__ = "@_batsec_"

EXEC_ID   = 0x4000
OPCODE_CD = 0x6000

ERROR = False
error_list = ""

# beacon to exec command on
current_beacon = None

# let argparse error and exit nice
def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"

def exit(status=0, message=None):
    if message != None: print(message)
    return

def cd_callback(shad0w, data):
    shad0w.event.beacon_info(current_beacon, data)

    return ""

def main(shad0w, args, beacon):
    global current_beacon

    # make beacon global
    current_beacon = beacon

    # save the raw args
    raw_args = args

    # usage examples
    usage_examples = """

Examples:

cd C:\\
cd "C:\\Documents and Settings"
"""

    parse = argparse.ArgumentParser(prog='cd',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("dir", nargs='*', help="Location of the dir you want to change to")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # check for errors
    if ERROR:
        print(error_list)
        parse.print_help()
        return

    # make the json
    data = {"op" : OPCODE_CD, "args": ''.join(args.dir)}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[current_beacon]["callback"] = cd_callback
    shad0w.beacons[current_beacon]["task"] = (EXEC_ID, data)