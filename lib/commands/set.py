#
# Set internal variables
#

import argparse

__description__ = "Set the value of a variable"
__type__ = "beacon"

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
    global ERROR

    # usage examples
    usage_examples = """

Examples:

set --list
set -v MsfStageSize -d 14
"""

    parse = argparse.ArgumentParser(prog='set',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("-l", "--list", action='store_true', help="List all the variables")
    parse.add_argument("-v", "--variable", help="Variable name")
    parse.add_argument("-d", "--data", help="Data to store in the variable")

    # make sure we don't die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    if ERROR:
        ERROR = False
        print(error_list)
        parse.print_help()
        return

    if args.list:
        shad0w.debug.log(f"{len(shad0w.variables)} variables.\n", log=True)
        for key in shad0w.variables:
            print(f"{key} => {shad0w.variables[key]}")
        return

    if args.variable and args.data:
        try:
            shad0w.variables[args.variable] = args.data
        except KeyError:
            shad0w.debug.error("Key Error")
            return

    if args.variable and not args.data:
        try:
            shad0w.debug.log(f"{args.variable} => {shad0w.variables[args.variable]}", log=True)
        except KeyError:
            shad0w.debug.error("Key Error")
            return

    return
