[![Project Status](https://img.shields.io/badge/status-BETA-yellow?style=flat-square)]()

# SHAD0W

<p align="center">
  <img alt="shad0w logo" src="shad0w.png" />
</p>

SHAD0W is a modular C2 framework designed to successfully operate on mature environments.

It uses a range of methods to evade EDR and AV while allowing the operator to continue using tooling and tradecraft they are familiar with. Its powered by Python 3.8 and C, and uses [Donut](https://github.com/TheWover/donut) for payload generation. By using Donut along with the process injection capabilities of SHAD0W, it provides the operator the ability to execute .NET assemblies, DLLs, EXEs, JS, VBS or XSLs fully inside memory. Dynamically resolved syscalls are heavily used to avoid userland API hooking, anti DLL injection to make it harder for EDR to load code into the beacons and offical Microsoft mitigation methods to protect spawn processes.

See the wiki for installation and usage instructions.

Main features of SHAD0W C2:
- **Built for Docker** - Runs fully inside of Docker allowing cross platform usage
- **Extremely modular** - Easy to create new modules to interact and task beacons
- **HTTPS C2 communication** - All traffic between beacons and the C2 are encrypted and transmitted over HTTPS
- **JSON based protocol** - Custom beaons are able to be built and used with an easy to implement protocol
- **Live proxy and mirror** - The C2 server is able to mirror any website in real time, relaying all non C2 traffic to that site, making it look less subject when viewed in a web browser
- **Modern CLI** - The CLI is built on [prompt-toolkit](https://github.com/prompt-toolkit/python-prompt-toolkit)

Main features of SHAD0W beacons:
- **EXE, PowerShell, shellcode and more** - Beacons can be generated and used in many different formats
- **Process injection** - Allows the operator to `dllinject`, `migrate`, `shinject` and more
- **Bypass AV** - Payloads are frequently updated to evade common Anti-Virus products
- **Highly configurable** - Custom jitters, user agents and more
- **HTTPS C2 communication** - Traffic to and from the C2 is encrypted via HTTPS
- **Proxy aware** - All callbacks can use the current system proxy

Current Modules:
- **Elevate** - Built in PrivEsc exploits
- **Ghost in the Logs** - Disable ETW & Sysmon, more info can be found [here](https://blog.dylan.codes/evading-sysmon-and-windows-event-logging/)
- **GhostPack** - Binaries compiled nightly via an Azure pipeline. Thanks to [@Flangvik](https://twitter.com/Flangvik)
- **Mimikatz** - For all your credential theft needs
- **SharpCollection** - A ton of .NET offensive tools, more info can be found [here](https://github.com/Flangvik/SharpCollection)
- **SharpSocks** - Reverse SOCKS proxy over HTTPS
- **StdAPI** - Common commands to interact with the file system
- **Unmanaged PowerShell** - Contains built in AMSI bypass
- **Upload and Download** - Easy data exfiltration

## Official Discord

[![Porchetta Industries](https://discordapp.com/api/guilds/736724457258745996/widget.png?style=banner3)](https://discord.gg/ycXRvcD)
