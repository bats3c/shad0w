import json


class Builder(object):

    def __init__(self, shad0w):

        super(Builder, self).__init__()
        self.shad0w = shad0w

        # for invalid requests
        self.IGNORE_CONTENT = ""

    def build(self, blank=False, beacon_id=None, **resp):

        # build a response to the beacon

        if blank is True:
            return self.IGNORE_CONTENT

        try:
            # we have a correct beacon id
            resp["alive"] = self.shad0w.beacons[beacon_id]["stay_alive"]

            # if we got a 'null' task set it to 0x1000
            # this will throw a key error when a beacon is registering. hence the try, except.

            try:
                if resp["task"] == None:
                    resp["task"] = 0x1000
            except KeyError:
                pass

            # now return the dict in json format

            return json.dumps(resp)

        except KeyError:
            # we got an invalid beacon id
            return self.IGNORE_CONTENT
