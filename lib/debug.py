class Debug(object):
    def __init__(self, arg):
        super(Debug, self).__init__()
        self.debug_verbose = arg

    def log(self, text, log=False, pre=True):
        if log and pre:
            print(f"\033[1;34m[i]\033[0m {text}")
        elif log and not pre:
            print(f"{text}")
        elif self.debug_verbose:
            print(f"[DEBUG] {text}")
    
    def good(self, text):
        print("\033[1;32m[+]\033[0m", text)

    def error(self, text):
        print("\033[1;31m[!]\033[0m", text)