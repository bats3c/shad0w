#!/bin/bash

# install stuff we need
apt-get update -y
apt-get install -y python3.8 python3-pip python3-openssl gcc-mingw-w64 python3.8-dev nasm upx binutils wget apt-transport-https procps mono-complete less metasploit-framework
python3.8 -m pip install -r /root/shad0w/requirements.txt

# install dotnet
wget https://packages.microsoft.com/config/ubuntu/16.04/packages-microsoft-prod.deb -O /tmp/packages-microsoft-prod.deb
dpkg -i /tmp/packages-microsoft-prod.deb
apt update -y
apt install dotnet-sdk-2.2 -y


# x86_64-w64-mingw32-gcc test.c -o test.exe -Wl,--pic-executable,-e,main
# only mingw in kali repos
# x86_64-w64-mingw32-gcc (GCC) 9.3-win32 20200324
# 9.3-win32
# sudo docker rm -f $(sudo docker ps -a -q)
# sudo docker rmi <image>
# sudo docker run -v $(pwd):/root/shad0w -it shad0w -d
# sudo docker run -v $(pwd):/root/shad0w/bridge -it shad0w -d

