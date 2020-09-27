import json
import base64
import urllib3
import requests

# disable warnings
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

class LoginError(Exception):
    pass

class PollingError(Exception):
    pass

class SHAD0W(object):
    """
    Main class for interacting with the SHAD0W team server.
    """

    def __init__(self, host, port):
        super(SHAD0W, self).__init__()

        self.teamserver_host = host
        self.teamserver_port = port

        self.login_path = "/login"
        self.poll_path  = "/poll"
        self.comnd_path = "/cmd"

        self.http_session = requests.Session()

    def _make_url(self, path):
        """
        Make a url from a path.
        """

        return "https://" + self.teamserver_host + ":" + str(self.teamserver_port) + path

    def login(self, user, password):
        """
        Try to login to the team server.
        """

        # make the url for where to login
        login_url = self._make_url(self.login_path)

        # format the post data
        login_data = {
            "username": user,
            "password": password
        }

        # turn the dict into json
        login_data = json.dumps(login_data)

        # try to login
        req = self.http_session.post(login_url, login_data, verify=False)

        # check the responce is valid json and convert it to dict
        try:
            res = json.loads(req.text.strip())
        except json.decoder.JSONDecodeError:
            raise LoginError

        # check if it was successful
        if res["success"] == True:
            return True

        return False

    def poll(self):
        """
        Poll the teamserver for new events
        """

        # make the polling url
        poll_url = self._make_url(self.poll_path)

        # get the events
        req = self.http_session.get(poll_url)

        # check the responce is valid json and convert it to dict
        try:
            res = json.loads(req.text.strip())
        except json.decoder.JSONDecodeError:
            raise PollingError

        # return the dict with the events
        return res

    def run_command(self, beacon_id, command):
        """
        Run a command on a beacon
        """

        # make the command url
        cmd_url = self._make_url(self.comnd_path)

        # setup the arguments
        data_args = {
            "cmd": base64.b64encode(command.encode()).decode(),
            "beacon_id": beacon_id
        }

        # make it json
        data_args = json.dumps(data_args)

        # run the command
        req = self.http_session.post(cmd_url, data_args)

        # did we run it successfully
        data = json.loads(req.text)

        # give the result back
        try:
            return data["success"]
        except:
            return False