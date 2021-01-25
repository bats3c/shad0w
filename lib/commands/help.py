#
#   Help
#

import importlib
from lib import cmd
from prettytable import PrettyTable

__description__ = "Show help information"
__author__ = "@_batsec_"
__type__ = "system"


def usage(shad0w):
    t = PrettyTable(["Type", "Command", "Description"])

    t.align["Type"] = "l"
    t.align["Command"] = "l"
    t.align["Description"] = "l"

    num_of_modules = len(cmd.Shad0wLexer.commands)

    shad0w.debug.log(f"{num_of_modules} available commands.", log=True)
    shad0w.debug.log(f"To get more info on a command, use the -h flag.\n", log=True)

    for num, command in enumerate(cmd.Shad0wLexer.commands):
        mod = importlib.import_module("lib.commands." + command)
        if shad0w.debugv:
            importlib.reload(mod)

        try:
            type = mod.__type__
        except Exception:
            type = "No type available."

        try:
            description = mod.__description__
        except Exception:
            description = "No description available."

        t.add_row([type, command, description])
        if num != num_of_modules - 1:
            t.add_row([" ", " ", " "])

    return t


def main(shad0w, args):
    info = usage(shad0w)
    shad0w.debug.log(info, pre=False, log=True)
