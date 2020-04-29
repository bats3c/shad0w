# shad0w

Shad0w is a covert post exploitation framework

### Install

    git clone https://github.com/bats3c/shad0w.git
    sudo docker build -t shad0w .

I recommend adding this alias to your bashrc

    alias shad0w='sudo docker run -v $(pwd):/root/shad0w/.bridge -p 443:443 -it shad0w'

### Usage

    shad0w listen -a <ip>
    shad0w beacon -a <ip> -f <format> -o <name>

### Run as dev

    sudo docker run -v $(pwd):/root/shad0w -p 443:443 -it shad0w etc...