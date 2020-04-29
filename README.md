# shad0w

blah

## Setup

build...

    docker build -t shad0w .

run...

    docker run -v $(pwd):/root/shad0w/bridge -it shad0w

dev...

    sudo docker run -v $(pwd):/root/shad0w -it shad0w -d
    sudo docker run -v $(pwd):/root/shad0w/bridge -it shad0w -d

notes:

    x86_64-w64-mingw32-gcc test.c -o test.exe -Wl,--pic-executable,-e,main
    only mingw in kali repos
    x86_64-w64-mingw32-gcc (GCC) 9.3-win32 20200324
    9.3-win32

shad0w listen -a dylan.codes -p 443
shad0w beacon -a dylan.codes -p 443 -j 1 -f raw -o beacon.bin

sudo docker rm -f $(sudo docker ps -a -q)
sudo docker rmi <image>