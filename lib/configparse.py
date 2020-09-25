import os
import yaml

class ConfigParse(object):

    def __init__(self, arg):

        super(ConfigParse, self).__init__()

        self.configfile = arg

    def init(self):
        """
        Load the config file
        """

        with open(self.configfile) as file:
            self.data = yaml.load(file, Loader=yaml.FullLoader)

    def parse(self, teamsrv):
        """
        Extract the values out of the config file
        """

        # pull out the values
        teamserver_config = self.data['teamserver']
        ssl_config        = self.data['ssl']
        c2_config         = self.data['c2']

        # config for the teamserver
        teamsrv.addr = teamserver_config['host']
        teamsrv.port = teamserver_config['port']

        # set the ssl stuff
        teamsrv.key = "certs/" + ssl_config['key']
        teamsrv.cert = "certs/" + ssl_config['cert']

        # c2 stuff
        teamsrv.c2_addr     = c2_config['address']
        teamsrv.c2_port     = c2_config['port']
        teamsrv.c2_endpoint = c2_config['endpoint']