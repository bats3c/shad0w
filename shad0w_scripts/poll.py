import sys; sys.path.append("../lib/")

import time
import libshad0w

# creds for the teamserver
USER     = "admin"
PASSWORD = "shad0w"

# init the shad0w object
shad0w = libshad0w.SHAD0W("localhost", 1337)

# try to login to the teamserver
if not shad0w.login(USER, PASSWORD):
    print("Login Failed")
    exit()

while True:
    # try poll the team server
    events = shad0w.poll()

    print(events)

    time.sleep(1)