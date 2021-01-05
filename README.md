[![Project Status](https://img.shields.io/badge/status-BETA-yellow?style=flat-square)]()

# SHAD0W

<p align="center">
  <img alt="shad0w logo" src="shad0w.png" />
</p>

SHAD0W is a modular C2 framework designed to successfully operate on mature environments.

It will use a range of methods to evade EDR and AV while allowing the operator to continue using tooling an tradecraft they are familiar with. Its powered by Python 3.8 and C, using [Donut](https://github.com/TheWover/donut) for payload generation. By using Donut along side the process injection capabilities of SHAD0W it gives the operator the ability to execute .NET assemblies, EXEs, DLLs, VBS, JS or XSLs fully inside memory. Dynamically resolved syscalls are heavily used to avoid userland API hooking, anti DLL injection to make it harder for EDR to load code into the beacons and offical microsoft mitigation methods to protect spawn processes.

Main SHAD0W C2 features:

- **Built for Docker** - It runs fully inside of Docker allowing cross platform usage
- **Live proxy and mirror** - The C2 server is able to mirror any website in real time, relaying all non C2 traffic to that site making it look less subject when viewed in a web browser
- **HTTPS C2 communication** - All traffic between beacons and the C2 are encrypted and transmitted over HTTPS
- **Modern CLI** - The CLI is built on [prompt-toolkit](https://github.com/prompt-toolkit/python-prompt-toolkit)
- **JSON based protocol** - Custom beaons are able to be built and used with an easy to implement protocol
- **Extremely modular** - Easy to create new modules to interact and task beacons

Main SHAD0W beacons features:

- **DLL, EXE, PowerShell and Shellcode** - Beacons can be generated and used in many different formats
- **Process injection** - Allowing you to `migrate`, `shinject`, `dllinject` and more
- **Bypass AV** - Payloads are frequently updated to evade common AV products
- **Highly configurable** - Custom jitters, user agents and more
- **Proxy aware** - All callbacks will use the current system proxy
- **HTTPS C2 communication** - Traffic to and from the C2 is encrypted via HTTPS

Current Modules:

- **Elevate** - Built in PrivEsc exploits
- **Ghost In The Logs** - Disable ETW & Sysmon, more info can be found [here](https://blog.dylan.codes/evading-sysmon-and-windows-event-logging/)
- **GhostPack** - With the binarys compiled nightly via an Azure pipeline. Thanks to [@Flangvik](https://twitter.com/Flangvik)
- **Mimikatz** - For all your credential theft needs
- **SharpCollection** - A ton of .NET offensive tools, more info can be found [here](https://github.com/Flangvik/SharpCollection)
- **SharpSocks** - Reverse socks proxy over HTTPS
- **StdAPI** - Common commands to interact with the file system
- **Unmanaged Powershell** - With built in AMSI bypass
- **Upload & Download** - Easy data exfiltration

## For install and usage information, see the wiki.


## Official Discord

[![Porchetta Industries](https://discordapp.com/api/guilds/736724457258745996/widget.png?style=banner3)](https://discord.gg/ycXRvcD)
