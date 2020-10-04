#
# Create a directory
#

import json
import argparse

__description__ = "Create a new directory on the target"
__author__ = "@_batsec_"

EXEC_ID      = 0x4000
OPCODE_MKDIR = 0x5000

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

def mkdir_callback(shad0w, data):
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
Example:

mkdir "C:\\Users\\thejoker\\hello\\"
"""

    parse = argparse.ArgumentParser(prog='mkdir',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("name", nargs='*', help="Name of the directory you want to create")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    if not args.name:
        parse.print_help()
        return

    # clean it up
    del_file = ' '.join(args.name).replace('\\', "\\\\").replace('"', '')

    # make the json
    data = {"op" : OPCODE_MKDIR, "args": del_file}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[current_beacon]["callback"] = mkdir_callback
    shad0w.beacons[current_beacon]["task"] = (EXEC_ID, data)