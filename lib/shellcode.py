import os
import donut
import base64
import tempfile

from lib.ShellcodeRDI import *

def generate_srdi(file, flags):
    """ generate shellcode from a reflective dll using sRDI """

    hfunc = HashFunctionName("Entry")

    with open(file, "rb") as file:
        dll_data = file.read()

    return ConvertToShellcode(dll_data, hfunc, b"None", flags)

def parse_donut_error(data, filename):
    if "Error : File not found." in data:
        print(f"Unable to find '{filename}'")
    else:
        print(f"Failed to execute '{filename}'")

def generate(file, args, params, parse=True):
    """ generate shellcode from a pe using donut """

    """
    # build args, kinda messy but idk how else to do it
    call_donut = "donut.create(file=file"
    if args.param:
        call_donut += f", params='{params}'"
    if args.cls:
        call_donut += f", cls='{args.cls}'"
    if args.method:
        call_donut += f", method='{args.method}'"
    if args.runtime:
        call_donut += f", runtime='{args.runtime}'"
    if args.appdomain:
        call_donut += f", appdomain='{args.appdomain}'"
    call_donut += f")"

    # get the shellcode from the file with <3 from donut
    shellcode_bytes = eval(call_donut)

    # shellcode_bytes = donut.create(file=file, params='arg1 arg2')

    # encode the shellcode and return it
    return base64.b64encode(shellcode_bytes).decode()
    """

    # using the python bindings for donut doesn't seem to let them
    # build shellcode from a pe with any arguments? Using this as
    # a dirty work around untill this is fixed

    # if we need to parse the args an build it from them then
    if parse:
        # key variables we are going to use
        donut = f"/root/shad0w/bin/donut.elf"
        cmdline = ""

        # create a temp file
        temp = tempfile.NamedTemporaryFile()

        # build the command line args if we need to
        if args.param:
            cmdline += f" -p \'{params}\'"
        if args.cls:
            cmdline += f" -c {args.cls}"
        if args.method:
            cmdline += f" -m {args.method}"
        if args.runtime:
            cmdline += f" -r {args.runtime}"
        if args.appdomain:
            cmdline += f" -d \"{args.appdomain}\""

        # execute the command to build the shellcode
        cmd = f"{donut} {file} {cmdline} -o {temp.name}"
        cmd_out = os.popen(cmd).read()

        # not a good way to check but it will work for now
        if temp.name not in cmd_out:
            parse_donut_error(cmd_out, file)
            return None

        # base64 encode the file and return it
        with open(temp.name, "rb") as file:
            data = file.read()
            return base64.b64encode(data).decode()

    elif parse == False:
        # location of our donut binary
        donut = f"/root/shad0w/bin/donut.elf"

        # create a temp file
        temp = tempfile.NamedTemporaryFile()

        # build the command to generate our shellcode
        if params:
            cmd = f"{donut} {file} -p \'{params}\' -o {temp.name}"
        else:
            cmd = f"{donut} {file} -o {temp.name}"

        # execute the command and get the output
        cmd_out = os.popen(cmd).read()

        # not a good way to check but it will work for now
        if temp.name not in cmd_out:
            parse_donut_error(cmd_out, file)
            return None

        # return the raw shellcode
        with open(temp.name, "rb") as file:
            return file.read()