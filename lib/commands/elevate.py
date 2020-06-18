# 
# Elevate the current users privs
#

import os
import sys
import argparse
import importlib
import threading

from prettytable import PrettyTable

__description__ = "Attempt to elevate the beacons privilages on the target"
__author__ = "@_batsec_"

ERROR = False
error_list = ""
RAN_COMMAND = False

# let argparse error and exit nice
def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"

def exit(status=0, message=None): 
    if message != None: print(message)
    return

def list_exploits(shad0w):
    sys.path.append("/root/shad0w/exploits/")
    all_exploits = importlib.import_module("__init__").__all__

    shad0w.debug.log(f"{len(all_exploits)} avalible exploits:\n", log=True)
    t = PrettyTable(['Exploit', 'Description'])
    
    for exploit in all_exploits:
        exploit_name = os.path.basename(exploit)
        description = importlib.import_module(exploit.replace("/", ".")).__description__["Details"]
        t.add_row([exploit_name, description])
    
    print(t)

def show_details(shad0w, name):
    sys.path.append("/root/shad0w/exploits/")
    all_exploits = importlib.import_module("__init__").__all__

    for exploit in all_exploits:
        exploit_name = os.path.basename(exploit)
        if name == exploit_name:
            author = importlib.import_module(exploit.replace("/", ".")).__description__["Author"]
            cve = importlib.import_module(exploit.replace("/", ".")).__description__["CVE"]
            target = importlib.import_module(exploit.replace("/", ".")).__description__["Target"]
            versions = importlib.import_module(exploit.replace("/", ".")).__description__["Versions"]
            arch = importlib.import_module(exploit.replace("/", ".")).__description__["Arch"]

            secure = importlib.import_module(exploit.replace("/", ".")).__description__["Secure"]
            if secure: secure = "Yes"
            else: secure = "No"

            shad0w.debug.log("Exploit details:\n", log=True)
            shad0w.debug.log(f"Name: {exploit_name}", log=True, pre=False)
            shad0w.debug.log(f"Author: {author}", log=True, pre=False)
            shad0w.debug.log(f"CVE: {cve}", log=True, pre=False)
            shad0w.debug.log(f"Target: {target}", log=True, pre=False)
            shad0w.debug.log(f"Versions: {versions}", log=True, pre=False)
            shad0w.debug.log(f"Arch: {arch}", log=True, pre=False)
            shad0w.debug.log(f"Supports Secure: {secure}", log=True, pre=False)

def check_exploit(shad0w, name, arch):
    # run the exploit in check mode

    sys.path.append("/root/shad0w/exploits/")
    all_exploits = importlib.import_module("__init__").__all__

    for exploit in all_exploits:
        exploit_name = os.path.basename(exploit)
        if name == exploit_name:
            importlib.import_module(exploit.replace("/", ".")).check(shad0w, arch)

def use_exploit(shad0w, name, arch):
    # run the exploit in exploit mode

    sys.path.append("/root/shad0w/exploits/")
    all_exploits = importlib.import_module("__init__").__all__

    for exploit in all_exploits:
        exploit_name = os.path.basename(exploit)
        if name == exploit_name:
            exploit_mod = importlib.import_module(exploit.replace("/", "."))
            threading.Thread(target=exploit_mod.exploit, args=(shad0w, arch)).start()

def main(shad0w, args):
    global RAN_COMMAND
    
    # check we got a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return
    
    # usage examples
    usage_examples = """

Examples:

elevate --list
elevate --details system_printspoofer
elevate --check system_printspoofer
elevate --use system_printspoofer
elevate --smart
"""

    # init the parser
    parse = argparse.ArgumentParser(prog='elevate',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("-l", "--list", required=False, action='store_true', help="List the exploits avalible for the current session")
    parse.add_argument("-d", "--details", required=False, help="Display the details of an exploit")
    parse.add_argument("-c", "--check", required=False, help="Check if the exploit will work")
    parse.add_argument("-u", "--use", required=False, help="Use the exploit to attempt to elevate the session")
    parse.add_argument("-s", "--smart", required=False, action='store_true', help="Attempt to auto elevate by letting shad0w decide what exploits to use")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass
    
    # we need a file to read so if we dont then fail
    if ERROR:
        print(error_list) 
        parse.print_help()
        return

    # list the avalible exploits
    if args.list:
        RAN_COMMAND = True
        list_exploits(shad0w)
    
    # show the details for an exploit
    if args.details:
        RAN_COMMAND = True
        show_details(shad0w, args.details)

    # check if an exploit will work
    if args.check:
        RAN_COMMAND = True
        session_arch = shad0w.beacons[shad0w.current_beacon]["arch"]
        check_exploit(shad0w, args.check, session_arch)
    
    # check if an exploit will work
    if args.use:
        RAN_COMMAND = True
        session_arch = shad0w.beacons[shad0w.current_beacon]["arch"]
        use_exploit(shad0w, args.use, session_arch)
    
    if RAN_COMMAND == False:
        parse.print_help()
        return