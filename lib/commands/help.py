#
#   Help    -   Kinda self explanatory what this does...
#

import importlib

from lib import cmd
from prettytable import PrettyTable

__description__ = "Show shad0ws help infomation"
__author__ = "@_batsec_"

def usage(shad0w):
    t = PrettyTable(["Command", "Description", "Author"])

    t.align["Command"] = "l"
    t.align["Description"] = "l"
    t.align["Author"] = "l"

    num_of_modules = len(cmd.Shad0wLexer.commands)

    shad0w.debug.log(f"{num_of_modules} avalible commands.", log=True)
    shad0w.debug.log(f"To get more info on the usage of the command use the flags -h/--help on it.\n", log=True)

    for num, command in enumerate(cmd.Shad0wLexer.commands):
        mod = importlib.import_module("lib.commands." + command)
        if shad0w.debugv: importlib.reload(mod)

        try:
            description = mod.__description__
        except:
            description = "No description avalible"

        try:
            author = mod.__author__
        except:
            author = "No author avalible"

        t.add_row([command, description, author])
        if num != num_of_modules - 1:
            t.add_row([" ", " ", " "])


    return t

def main(shad0w, args):
    info = usage(shad0w)
    shad0w.debug.log(info, pre=False, log=True)