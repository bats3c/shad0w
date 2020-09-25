import sys

"""
┬
└
├
─
┴

┬
├─ Started Team Server [ ]
|  └ Config: 
├─ Started C2 Server [ ]
|  └ SSL Key: 
|  └ SSL Cert: 
├─ Building Payloads
|  └ Staged [ ]
|  └ Static [ ]
|     └ ShellCode [ ]
├─ Adding Users
|  └ *username* [ ]
┴

[i] Team Server is ready on localhost:31337
[i] C2 is ready on localhost:443

"""

def print_start():
    print("┬")

def print_infomation(text):
    sys.stdout.write(f"├─ {text}\n")

def print_pending(text):
    sys.stdout.write(f"├─ {text} [...]")

def print_done(text):
    sys.stdout.write(f"\r├─ {text} [✓]               \n")

def print_sub_straight(text):
    sys.stdout.write(f"|  └ {text} \n")

def print_sub_pending(text):
    sys.stdout.write(f"|  └ {text} [...]")

def print_sub_done(text):
    sys.stdout.write(f"\r|  └ {text} [✓]            \n")

def print_end():
    print("┴")