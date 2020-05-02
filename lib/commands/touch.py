# 
# Create a file
# 

import argparse

from lib import buildtools

EXEC_ID = 0x3000

ERROR = False
error_list = ""

# let argparse error and exit nice
def error(message):
    global ERROR, error_list
    ERROR = True
    error_list += f"\033[0;31m{message}\033[0m\n"

def exit(status=0, message=None): 
    if message != None: print(message)
    return

def main(shad0w, args):

    # save the raw args
    raw_args = args
    
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return

    # usage examples
    usage_examples = """
Example:

touch "C:\\Users\\thejoker\\hello.txt"
"""
    
    parse = argparse.ArgumentParser(prog='touch',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)
    
    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("name", nargs='*', help="Name of the file you want to create")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/touch/", builddir="/root/shad0w/modules/windows/touch/build")

    # set the correct settings
    template = "LPCSTR szFileName = \"%s\";" % (' '.join(args.name).replace('"', '').replace('\\', '\\\\'))

    buildtools.update_settings_file(None, custom_template=template, custom_path="/root/shad0w/modules/windows/touch/build/settings.h")

    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/touch/build", modlocation="/root/shad0w/modules/windows/touch/module.exe")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/touch/module.exe", want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)