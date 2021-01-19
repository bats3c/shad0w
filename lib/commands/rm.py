#
# Delete something
#

import json
import argparse

__description__ = "Delete a file on a target"
__author__ = "@_batsec_"
__type__ = "file system"

EXEC_ID = 0x4000
OPCODE_RM = 0x4000

ERROR = False
error_list = ""


def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"


def exit(status=0, message=None): 
    if message is not None:
        print(message)
    return


def rm_callback(shad0w, data):
    shad0w.debug.log(data, log=True, pre=False)

    return ""


def main(shad0w, args):

    # save the raw args
    raw_args = args
    
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon.")
        return

    # usage examples
    usage_examples = """
Example:

rm "C:\\Users\\thejoker\\deleteme.txt"
"""
    
    parse = argparse.ArgumentParser(prog='rm',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("name", nargs='*', help="Name of what you want to delete")

    # make sure we don't die from weird args
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
    data = {"op": OPCODE_RM, "args": del_file}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["callback"] = rm_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, data)
