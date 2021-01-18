import os
import importlib

from prompt_toolkit.patch_stdout import patch_stdout

from lib.commands import *


class Handler(object):

    def __init__(self, shad0w):

        super(Handler, self).__init__()
        self.shad0w = shad0w

    async def do(self, cmd):

        # split command into name + args

        splitcommand = cmd.split(" ")
        basecmd = splitcommand[0]
        cmd_args = splitcommand[0:]

        # see if we need to execute a local command
        try:
            if cmd[0][0] == "!":
                # lets execute the commands in the bridge
                cwd = os.getcwd()
                os.chdir("/root/shad0w/.bridge")

                # execute the command
                self.shad0w.debug.log(f"Executing: {''.join(cmd[1:])}", log=True)
                os.system(''.join(cmd[1:]))

                # move back to the old dir
                os.chdir(cwd)

                return
        except IndexError:
            pass

        # find the handle for the module in the globals list an call it with args

        if len(basecmd) > 0:

            # reimport so don't have to restart whole c2 for every file change
            # but only do this in debug mode

            cmd_func = globals()[basecmd]
            if self.shad0w.debugv: importlib.reload(cmd_func)

            if basecmd == "whoami":
                await cmd_func.main(self.shad0w, cmd_args)
            else:
                cmd_func.main(self.shad0w, cmd_args)
