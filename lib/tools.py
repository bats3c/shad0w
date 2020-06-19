import json
import hashlib
import os
from datetime import datetime

# list all command scripts in the commands dir, append to list of commands
def get_commands():
    commandList = []
    for r, d, f in os.walk("/root/shad0w/lib/commands/"):
        for file in f:
            if file.endswith(".py") and "_" not in file and :
                commandList.append(file.replace(".py",""))
    return commandList

def generate_beacon_id():
    # get md5 hash of the current date + time
    return hashlib.md5(str(datetime.now()).encode()).hexdigest()

def get_data_from_json(jdata):
    # get the data from data

    id     = ""
    opcode = 0
    data   = ""

    # if we get any errors, just return the above values
    # and this req will then be ignored
    try:
        id = jdata['id']
        if jdata['opcode']:
            opcode = jdata['opcode']
            if jdata['data']:
                data = jdata['data']
    except KeyError:
        pass

    # print("reting: ", (id, opcode, data))

    return id, opcode, data
