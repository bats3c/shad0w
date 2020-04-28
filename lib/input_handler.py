import importlib

from lib.commands import *

class Handler(object):

    def __init__(self, shad0w):

        super(Handler, self).__init__()
        self.shad0w = shad0w

    def do(self, cmd):

        # split command into name + args

        splitcommand = cmd.split(" ")
        basecmd      = splitcommand[0]
        cmd_args     = splitcommand[0:]

        # find the handle for the module in the globals list an call it with args

        if len(basecmd) > 0:

            # reimport so dont have to restart whole c2 for every file change
            # but only do this in debug mode

            cmd_func = globals()[basecmd]
            if self.shad0w.debugv: importlib.reload(cmd_func)
            cmd_func.main(self.shad0w, cmd_args)
