import sys; sys.path.append("../lib/")

import libshad0w

# creds for the teamserver
USER     = sys.argv[1]
PASSWORD = sys.argv[2]

# init the shad0w object
shad0w = libshad0w.SHAD0W("localhost", 1337)

# try to login to the teamserver
if not shad0w.login(USER, PASSWORD):
    print("Login Failed")
    exit()

print("Press [ENTER] if beacon not registered")
while True:
    beacon_id = input("beacon: ")
    if len(beacon_id) != 0:
        break
    print(shad0w.poll())

while True:
    cmd = input(f"{beacon_id} >> ")

    if shad0w.run_command(beacon_id, cmd):
        print("Ran Command")

    events = ''.join(shad0w.poll()[beacon_id])
    print(events)