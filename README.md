[![Project Status](https://img.shields.io/badge/status-BETA-yellow?style=flat-square)]()

# SHAD0W

<p align="center">
  <img alt="shad0w logo" src="shad0w.png" />
</p>

SHAD0W is a modular C2 framework designed to successfully operate on mature environments.

It will use a range of methods to evade EDR and AV while allowing the operator to continue using tooling an tradecraft they are familiar with. Its powered by Python 3.8 and C, using [Donut](https://github.com/TheWover/donut) for payload generation. By using Donut along side the process injection capabilities of SHAD0W it gives the operator the ability to execute .NET assemblies, EXEs, DLLs, VBS, JS or XSLs fully inside memory. Dynamically resolved syscalls are heavily used to avoid userland API hooking, anti DLL injection to make it harder for EDR to load code into the beacons and offical microsoft mitigation methods to protect spawn processes.

Main features of the SHAD0W C2 are:

- **Built For Docker** - It runs fully inside docker allowing cross platform usage
- **Live Proxy & Mirror** - The C2 server is able to mirror any website in real time, relaying all non C2 traffic to that site making it look less subject when viewed in a web browser
- **HTTPS C2 Communication** - All traffic between beacons and the C2 will be encrypted and transmitted over HTTPS
- **Modern CLI** - The CLI is built on [prompt-toolkit](https://github.com/prompt-toolkit/python-prompt-toolkit)
- **JSON Based Protocol** - Custom beaons are able to built and used easily with an easy to implement protocol
- **Extremely Modular** - Easy to create new modules to interact and task beacons

Main features of SHAD0W beacons are:

- **Shellcode, EXE, Powershell & More** - Beacons can be generated and used in many different formats
- **Process Injection** - Allowing you to `migrate`, `shinject`, `dllinject` and more
- **Bypass AV** - Payloads are frequently updated to evade common Anti-Virus products
- **Highly configurable** - Custom jitters, user agents and more
- **Proxy Aware** - All callbacks will use the current system proxy
- **HTTPS C2 Communication** - Traffic to and from the C2 is encrypted via HTTPS

Current Modules:

- **GhostPack** - With the binarys compiled nightly via an Azure pipeline. Thanks to [@Flangvik](https://twitter.com/Flangvik)
- **Unmanaged Powershell** - With built in AMSI bypass
- **Ghost In The Logs** - Disable ETW & Sysmon, more info can be found [here](https://blog.dylan.codes/evading-sysmon-and-windows-event-logging/)
- **Elevate** - Built in PrivEsc exploits
- **SharpSocks** - Reverse socks proxy over HTTPS
- **SharpCollection** - A ton of .NET offensive tools, more info can be found [here](https://github.com/Flangvik/SharpCollection)
- **Mimikatz** - For all your credential theft needs
- **Upload & Download** - Easy data exfiltration
- **StdAPI** - Common commands to interact with the file system

## Install

    $ git clone --recurse-submodules https://github.com/bats3c/shad0w.git && cd shad0w
    $ sudo ./shad0w install

## Usage

https://blog.dylan.codes/shad0w/

## Official Discord

[![Porchetta Industries](https://discordapp.com/api/guilds/736724457258745996/widget.png?style=banner3)](https://discord.gg/ycXRvcD)
