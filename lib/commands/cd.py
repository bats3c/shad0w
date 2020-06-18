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

# let argparse error and exit nice
def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"

def exit(status=0, message=None): 
    if message != None: print(message)
    return

def get_list_directory(rargs, args):
    # resolve the directory we need to list

    # if we got no other args but 'ls' then drop the current dir
    if ''.join(rargs) == 'ls':
        return "." 

    elif type(args.dir) == list:
        return ' '.join(args.dir).replace('"', '')
    
    elif args.dir is not None:
        return args.dir

    return None


def cd_callback(shad0w, data):
    shad0w.debug.log(data, log=True, pre=False)

    return ""

def main(shad0w, args):

    # save the raw args
    raw_args = args
    
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return

    # usage examples
    usage_examples = """

Examples:

cd C:\\
cd "C:\\Documents and Settings"
"""
    
    parse = argparse.ArgumentParser(prog='ls',
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

    # the user might have just run 'ls' but if not lets fail
    if ERROR:
        print(error_list)
        parse.print_help()
        return
    
    # find the dir we want to list
    dir = get_list_directory(raw_args, args)

    # make the json
    data = {"op" : OPCODE_CD, "args": dir}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["callback"] = cd_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, data)