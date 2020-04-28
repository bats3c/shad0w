#!/bin/bash

apt-get update -y
apt-get install -y git python3.8 python3-pip python3-openssl gcc-mingw-w64 python3.8-dev
python3.8 -m pip install -r /root/shad0w/requirements.txt