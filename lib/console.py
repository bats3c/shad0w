import os
import signal
import datetime
import traceback
import threading

from lib import input_handler, cmd

from prompt_toolkit import PromptSession
from pygments.lexers.shell import PowerShellLexer
from prompt_toolkit.history import FileHistory
from prompt_toolkit.lexers import PygmentsLexer
from prompt_toolkit.completion import WordCompleter
from prompt_toolkit.formatted_text import HTML, ANSI
from prompt_toolkit.patch_stdout import patch_stdout
from prompt_toolkit.auto_suggest import AutoSuggestFromHistory
from prompt_toolkit.shortcuts import prompt, CompleteStyle, PromptSession


class Console(object):

    def __init__(self, shad0w):

        super(Console, self).__init__()
        # cause its kind of useful
        self.shad0w        = shad0w

        # prompts, might add a way to customize this
        self.prompt               = "shad0w ≫ "
        self.active_domain_prompt = "shad0w(\033[1;31m%s\\%s@%s\033[0m) ≫ "
        self.active_prompt        = "shad0w(\033[1;31m%s@%s\033[0m) ≫ "

        # handle all given commands
        self.cmd_handler   = input_handler.Handler(self.shad0w)

    def set_autocompletes(self):
        self.autocomplete = WordCompleter(cmd.Shad0wLexer.commands, ignore_case=True)

    def beacon_toolbar(self):
        if self.shad0w.current_beacon:
            checkin_diff = self.shad0w.beacons[self.shad0w.current_beacon]["last_checkin_raw"]
            ping_diff = datetime.datetime.now() - checkin_diff
            last_ping = f'<style bg="black">{ping_diff.seconds + 1}s</style>'

            secure_val = self.shad0w.beacons[self.shad0w.current_beacon]["secure"]
            if secure_val == False:
                secure = '<style bg="ansired">No</style>'
            if secure_val != False:
                secure = '<style bg="green">Yes</style>'
            
            username = self.shad0w.beacons[self.shad0w.current_beacon]["username"]
            username = f'<style bg="black">{username}</style>'

            computer = self.shad0w.beacons[self.shad0w.current_beacon]["machine"]
            computer = f'<style bg="black">{computer}</style>'

            arch = self.shad0w.beacons[self.shad0w.current_beacon]["arch"]
            arch = f'<style bg="black">{arch}</style>'

            version = self.shad0w.beacons[self.shad0w.current_beacon]["os"]
            version = f'<style bg="black">{version} ({arch})</style>'

            return HTML(f'User: {username} | Computer: {computer} | OS: {version} | Secure: {secure} | Ping: {last_ping}')
        else:
            return HTML(f'<b><style bg="ansired">No Active Beacon</style></b>')

    async def start(self):

        # default history file
        histfile = FileHistory('.shad0w_history')

        # do what prompts do
        self.set_autocompletes()
        try:
            with patch_stdout():
                self.prompt_session = PromptSession(bottom_toolbar=self.beacon_toolbar, history=histfile, lexer=PygmentsLexer(cmd.Shad0wLexer), style=cmd.Shad0wLexer.lex_style, auto_suggest=AutoSuggestFromHistory())
        except ValueError: pass
        while True:
            try:
                # display a prompt depending on whether we got an active beacon or not
                if self.shad0w.current_beacon is None:
                    input = await self.prompt_session.prompt_async(ANSI(self.prompt), completer=self.autocomplete, complete_style=CompleteStyle.READLINE_LIKE)
                else:
                    # stuff to format for name
                    domain     = self.shad0w.beacons[self.shad0w.current_beacon]["domain"]
                    username   = self.shad0w.beacons[self.shad0w.current_beacon]["username"]
                    machine    = self.shad0w.beacons[self.shad0w.current_beacon]["machine"]

                    if domain != "NULL":
                        with patch_stdout():
                            input = await self.prompt_session.prompt_async(ANSI(self.active_domain_prompt % (username, domain, machine)), completer=self.autocomplete, complete_style=CompleteStyle.READLINE_LIKE, refresh_interval=0.5)
                    else:
                        with patch_stdout():
                            input = await self.prompt_session.prompt_async(ANSI(self.active_prompt % (username, machine)), completer=self.autocomplete, complete_style=CompleteStyle.READLINE_LIKE, refresh_interval=0.5)

                # handle the input we just received
                try:
                    with patch_stdout():
                        await self.cmd_handler.do(input)
                except Exception as e:

                    # tell user about error
                    print("ERROR:", e)

                    # if in debug mode drop the full traceback
                    if self.shad0w.debugv:
                        traceback.print_exc()

                    pass
            except KeyboardInterrupt:
                break
        
        # exit, trying to make it nicely
        self.shad0w.debug.log(f"Shad0w exiting...", log=True)
        exit()
        # for i in threading.enumerate()[::-1]: i.join()
        # for _ in range(0, 50): os.kill(os.getpid(), signal.SIGTERM); exit(1)
