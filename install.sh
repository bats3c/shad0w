#!/bin/bash

apt-get update -y
apt-get install -y git python3.8 python3-pip python3-openssl gcc-mingw-w64 python3.8-dev
python3.8 -m pip install -r /root/shad0w/requirements.txt
# git clone https://github.com/bats3c/shad0w /root/shad0w


# x86_64-w64-mingw32-gcc test.c -o test.exe -Wl,--pic-executable,-e,main
# only mingw in kali repos
# x86_64-w64-mingw32-gcc (GCC) 9.3-win32 20200324
# 9.3-win32
# sudo docker rm -f $(sudo docker ps -a -q)
# sudo docker rmi <image>
# sudo docker run -v $(pwd):/root/shad0w -it shad0w -d
# sudo docker run -v $(pwd):/root/shad0w/bridge -it shad0w -d