"""
Compile the stuff
"""

import os


class Binary(object):
    def __init__(self):
        super(Binary, self).__init__()

    def compile(self, path, outfile):
        pwd = os.getcwd()
        os.chdir(path)
        # os.system(f"make")
        os.system(f"make > /dev/null")
        os.chdir(pwd)
        return
