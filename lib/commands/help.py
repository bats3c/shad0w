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
    t = PrettyTable(["Command", "Description"])

    t.align["Command"] = "l"
    t.align["Description"] = "l"

    num_of_modules = len(cmd.Shad0wLexer.commands)

    shad0w.debug.log(f"{num_of_modules} available commands.", log=True)
    shad0w.debug.log(f"To get more info on a command, use the -h flag.\n", log=True)

    tDict = dict()
    for num, command in enumerate(cmd.Shad0wLexer.commands):
        mod = importlib.import_module("lib.commands." + command)
        if shad0w.debugv:
            importlib.reload(mod)

        try:
            description = mod.__description__
        except Exception:
            description = "No description available."

        tDict[command] = ([command, description], num)

    for tup in (sorted(tDict.items())):
        t.add_row(tup[1][0])
        if tup[1][1] != num_of_modules - 1:
            t.add_row([" ", " "])
    return t


def main(shad0w, args):
    info = usage(shad0w)
    shad0w.debug.log(info, pre=False, log=True)