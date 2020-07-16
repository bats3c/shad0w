import sys
import time
import threading

class Debug(object):
    def __init__(self, arg):
        super(Debug, self).__init__()
        self.debug_verbose = arg

        self.stop_spinner = False

    def log(self, text, log=False, pre=True, new=False, end='\n'):
        if log and pre:
            if new: print(f"\n\033[1;34m[i]\033[0m {text}", end='\n')
            if not new: print(f"\033[1;34m[i]\033[0m {text}", end='\n')
        elif log and not pre:
            print(f"{text}")
        elif self.debug_verbose:
            print(f"[DEBUG] {text}")
    
    def good(self, text):
        print("\033[1;32m[+]\033[0m", text)

    def error(self, text):
        print("\033[1;31m[!]\033[0m", text)
    
    def do_spinner(self, text):
        spin = ["|", "/", "-", "\\"]

        self.stop_spinner = False
        
        while self.stop_spinner == False:
            for s in spin:
                sys.stdout.write(f"\r\033[1;34m[i]\033[0m {text} {s}                                                         \r")
                time.sleep(0.1)
        
        sys.stdout.write("\r")

    
    def spinner(self, text):
        spin_thread = threading.Thread(target=self.do_spinner, args=(text,))
        spin_thread.daemon = False
        spin_thread.start()