#
# exit shad0w
#

import os, signal

def main(shad0w, args):

    # yea ik
    
    os.kill(os.getpid(), signal.SIGTERM)
