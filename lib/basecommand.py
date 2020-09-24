import argparse
from abc import ABC, abstractmethod


class BaseCommand:
    def __init__(self, command_name, args):
        self.__commandName = command_name
        self.__ERROR = False
        self.__error_list = ""

        self.args = args

        # init argparse
        self.parser = argparse.ArgumentParser(prog=self.__commandName,
                                              formatter_class=argparse.RawDescriptionHelpFormatter,
                                              epilog=self.get_usage())

        # keep it behaving nice
        self.parser.exit = self.exit
        self.parser.error = self.error

        self.parse_parameters()

    def error(self, message):
        self.__ERROR = True
        self.__error_list += f"\033[0;31m{message}\033[0m\n"

    @staticmethod
    def exit(status=0, message=None):
        if message is not None:
            print(message)
        return

    def parse(self):
        # make sure we dont die from weird args
        try:
            self.args = self.parser.parse_args(self.args[1:])
        except:
            pass

        # show the errors to the user
        if self.__ERROR:
            print(self.__error_list)
            self.parser.print_help()
            return False

        return True

    @abstractmethod
    def run(self, shad0w):
        pass

    @abstractmethod
    def parse_parameters(self):
        pass

    @abstractmethod
    def get_usage(self):
        pass
