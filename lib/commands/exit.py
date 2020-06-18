#
# exit shad0w
#

import os, signal

__description__ = "Exit shad0w C2"
__author__ = "@_batsec_"

def main(shad0w, args):

    # yea ik
    
    os.kill(os.getpid(), signal.SIGTERM)
