#
# exit shad0w
#

import os, signal

__description__ = "Exit shad0w C2"

def main(shad0w, args):

    # yea ik
    
    os.kill(os.getpid(), signal.SIGTERM)
