#!/bin/bash

# install stuff we need
apt-get update -y
apt-get install -y python3 python3-pip python3-openssl gcc-mingw-w64 nasm upx binutils wget apt-transport-https procps mono-complete less metasploit-framework
#Needs to be in shadow dir to install correctly. Assumes the install.sh script is already in shadow dir
python3 -m pip install -r requirements.txt

# install dotnet
wget https://packages.microsoft.com/config/ubuntu/16.04/packages-microsoft-prod.deb -O /tmp/packages-microsoft-prod.deb
dpkg -i /tmp/packages-microsoft-prod.deb
apt update -y
apt install dotnet-sdk-2.2 -y
