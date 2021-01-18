#
# Kill the beacon
#

import argparse

__description__ = "Kill the current beacon"
__author__ = "@_batsec_"
__type__ = "beacon"

# identify the task as shellcode execute
DIE_ID = 0x6000

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


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    # usage examples
    usage_examples = """

Examples:

die
die -y
"""

    # init argparse
    parse = argparse.ArgumentParser(prog='die',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # set the args
    parse.add_argument("-y", "--yes", action='store_true', help="Confirm the death of the beacon")

    # make sure we don't die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # show the errors to the user
    if ERROR:
        print(error_list)
        parse.print_help()
        return

    # check that the user has confirmed
    if not args.yes:
        shad0w.debug.log("Confirm you want to kill the current beacon with 'die -y'", log=True)
        return

    # tell the beacon to die
    if args.yes:
        shad0w.beacons[shad0w.current_beacon]["task"] = (DIE_ID, None)

    return
