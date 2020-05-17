import os
import signal
import traceback
import threading

from lib import input_handler

from prompt_toolkit import PromptSession
from prompt_toolkit.history import FileHistory
from prompt_toolkit.completion import WordCompleter
from prompt_toolkit.formatted_text import HTML, ANSI
from prompt_toolkit.shortcuts import prompt, CompleteStyle

class Console(object):

    def __init__(self, shad0w):

        super(Console, self).__init__()
        # cause its kinda useful
        self.shad0w        = shad0w

        # prompts, might add a way to customize this
        self.prompt               = "shad0w ≫ "
        self.active_domain_prompt = "shad0w(\033[1;31m%s\\%s@%s\033[0m) ≫ "
        self.active_prompt        = "shad0w(\033[1;31m%s@%s\033[0m) ≫ "

        # handle all given commands
        self.cmd_handler   = input_handler.Handler(self.shad0w)

    def set_autocompletes(self):
        cmds = ["help", "beacons", "shell", "exit", "execute", "ls", "cat", "touch", "mkdir", "rm", "write", "pwd", "upload", "download", "cd", "whoami", "elevate"]
        self.autocomplete = WordCompleter(cmds, ignore_case=True)

    def bt(self):
        last_ping = self.shad0w.beacons[self.shad0w.current_beacon]["last_checkin"]
        return ANSI(f"Last Ping: {last_ping}")

    def start(self):

        # default history file
        histfile = FileHistory('.shad0w_history')

        # do what prompts do
        self.set_autocompletes()
        try:
            self.prompt_session = PromptSession(history=histfile)
        except ValueError: pass
        while True:
            try:
                # display a prompt depending on wheather we got an active beacon or not
                if self.shad0w.current_beacon is None:
                    input = self.prompt_session.prompt(ANSI(self.prompt), completer=self.autocomplete, complete_style=CompleteStyle.READLINE_LIKE)
                else:
                    # stuff to format for name
                    domain     = self.shad0w.beacons[self.shad0w.current_beacon]["domain"]
                    username   = self.shad0w.beacons[self.shad0w.current_beacon]["username"]
                    machine    = self.shad0w.beacons[self.shad0w.current_beacon]["machine"]

                    if domain != "NULL":
                        input = self.prompt_session.prompt(ANSI(self.active_domain_prompt % (username, domain, machine)), completer=self.autocomplete, complete_style=CompleteStyle.READLINE_LIKE)
                    else:
                        input = self.prompt_session.prompt(ANSI(self.active_prompt % (username, machine)), completer=self.autocomplete, complete_style=CompleteStyle.READLINE_LIKE)

                # handle the input we just recived
                try:
                    self.cmd_handler.do(input)
                except Exception as e:

                    # tell user about error
                    print("ERROR:", e)

                    # if in debug mode drop the full traceback
                    if self.shad0w.debugv:  traceback.print_exc()

                    pass
            except KeyboardInterrupt:
                break
        
        # exit, trying to make it nicely
        self.shad0w.debug.log(f"Shad0w exiting...", log=True)
        exit()
        # for i in threading.enumerate()[::-1]: i.join()
        # for _ in range(0, 50): os.kill(os.getpid(), signal.SIGTERM); exit(1)
