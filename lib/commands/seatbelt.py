#
# Execute seatbelt on a session
#

import argparse

from lib import shellcode

__description__ = "Perform a number of security oriented host-survey \"safety checks\""
__author__ = "@_batsec_, @harmj0y"
__type__ = "enumeration"

# identify the task as shellcode execute
USERCD_EXEC_ID = 0x3000

# location of seatbelt binary
SEATBELT_BIN = "/root/shad0w/bin/SharpCollection/NetFramework_4.5_x86/Seatbelt.exe"


class DummyClass(object):
    # little hack but lets us pass the args to Donut
    def __init__(self):
        pass


def seatbelt_callback(shad0w, data):
    print(data)

    return ""


def usage():
    help_stuff = """
                            %&&@@@&&
                            &&&&&&&%%%,                       #&&@@@@@@%%%%%%###############%
                            &%&   %&%%                        &////(((&%%%%%#%################//((((###%%%%%%%%%%%%%%%
    %%%%%%%%%%%######%%%#%%####%  &%%**#                      @////(((&%%%%%%######################(((((((((((((((((((
    #%#%%%%%%%#######%#%%#######  %&%,,,,,,,,,,,,,,,,         @////(((&%%%%%#%#####################(((((((((((((((((((
    #%#%%%%%%#####%%#%#%%#######  %%%,,,,,,  ,,.   ,,         @////(((&%%%%%%%######################(#(((#(#((((((((((
    #####%%%####################  &%%......  ...   ..         @////(((&%%%%%%%###############%######((#(#(####((((((((
    #######%##########%#########  %%%......  ...   ..         @////(((&%%%%%#########################(#(#######((#####
    ###%##%%####################  &%%...............          @////(((&%%%%%%%%##############%#######(#########((#####
    #####%######################  %%%..                       @////(((&%%%%%%%################
                            &%&   %%%%%      Seatbelt         %////(((&%%%%%%%%#############*
                            &%%&&&%%%%%        v1.0.0         ,(((&%%%%%%%%%%%%%%%%%,
                             #%%%%##,


    Available commands (+ means remote usage is supported):

        + AMSIProviders          - Providers registered for AMSI
        + AntiVirus              - Registered antivirus (via WMI)
          AppLocker              - AppLocker settings, if installed
          ARPTable               - Lists the current ARP table and adapter information (equivalent to arp -a)
          AuditPolicies          - Enumerates classic and advanced audit policy settings
        + AuditPolicyRegistry    - Audit settings via the registry
        + AutoRuns               - Auto run executables/scripts/programs
          ChromeBookmarks        - Parses any found Chrome bookmark files
          ChromeHistory          - Parses any found Chrome history files
          ChromePresence         - Checks if interesting Google Chrome files exist
          CloudCredentials       - AWS/Google/Azure cloud credential files
          CredEnum               - Enumerates the current user's saved credentials using CredEnumerate()
          CredGuard              - CredentialGuard configuration
          dir                    - Lists files/folders. By default, lists users' downloads, documents, and desktop folders (arguments == [directory] [depth] [regex] [boolIgnoreErrors]
        + DNSCache               - DNS cache entries (via WMI)
        + DotNet                 - DotNet versions
          DpapiMasterKeys        - List DPAPI master keys
          EnvironmentPath        - Current environment %PATH$ folders and SDDL information
          EnvironmentVariables   - Current user environment variables
          ExplicitLogonEvents    - Explicit Logon events (Event ID 4648) from the security event log. Default of 7 days, argument == last X days.
          ExplorerMRUs           - Explorer most recently used files (last 7 days, argument == last X days)
        + ExplorerRunCommands    - Recent Explorer "run" commands
          FileInfo               - Information about a file (version information, timestamps, basic PE info, etc. argument(s) == file path(s)
          FirefoxHistory         - Parses any found FireFox history files
          FirefoxPresence        - Checks if interesting Firefox files exist
          IdleTime               - Returns the number of seconds since the current user's last input.
          IEFavorites            - Internet Explorer favorites
          IETabs                 - Open Internet Explorer tabs
          IEUrls                 - Internet Explorer typed URLs (last 7 days, argument == last X days)
          InstalledProducts      - Installed products via the registry
          InterestingFiles       - "Interesting" files matching various patterns in the user's folder. Note: takes non-trivial time.
        + InterestingProcesses   - "Interesting" processes - defensive products and admin tools
          InternetSettings       - Internet settings including proxy configs
        + LAPS                   - LAPS settings, if installed
        + LastShutdown           - Returns the DateTime of the last system shutdown (via the registry).
          LocalGPOs              - Local Group Policy settings applied to the machine/local users
        + LocalGroups            - Non-empty local groups, "-full" displays all groups (argument == computername to enumerate)
        + LocalUsers             - Local users, whether they're active/disabled, and pwd last set (argument == computername to enumerate)
          LogonEvents            - Logon events (Event ID 4624) from the security event log. Default of 10 days, argument == last X days.
        + LogonSessions          - Windows logon sessions
        + LSASettings            - LSA settings (including auth packages)
        + MappedDrives           - Users' mapped drives (via WMI)
          NamedPipes             - Named pipe names and any readable ACL information.
        + NetworkProfiles        - Windows network profiles
        + NetworkShares          - Network shares exposed by the machine (via WMI)
        + NTLMSettings           - NTLM authentication settings
          OfficeMRUs             - Office most recently used file list (last 7 days)
          OSInfo                 - Basic OS info (i.e. architecture, OS version, etc.)
          OutlookDownloads       - List files downloaded by Outlook
          PoweredOnEvents        - Reboot and sleep schedule based on the System event log EIDs 1, 12, 13, 42, and 6008. Default of 7 days, argument == last X days.
        + PowerShell             - PowerShell versions and security settings
          PowerShellEvents       - PowerShell script block logs (4104) with sensitive data.
          Printers               - Installed Printers (via WMI)
          ProcessCreationEvents  - Process creation logs (4688) with sensitive data.
          Processes              - Running processes with file info company names that don't contain 'Microsoft', "-full" enumerates all processes
        + ProcessOwners          - Running non-session 0 process list with owners. For remote use.
        + PSSessionSettings      - Enumerates PS Session Settings from the registry
        + PuttyHostKeys          - Saved Putty SSH host keys
        + PuttySessions          - Saved Putty configuration (interesting fields) and SSH host keys
          RDCManFiles            - Windows Remote Desktop Connection Manager settings files
        + RDPSavedConnections    - Saved RDP connections stored in the registry
        + RDPSessions            - Current incoming RDP sessions (argument == computername to enumerate)
          RecycleBin             - Items in the Recycle Bin deleted in the last 30 days - only works from a user context!
          reg                    - Registry key values (HKLM\\Software by default) argument == [Path] [intDepth] [Regex] [boolIgnoreErrors]
          RPCMappedEndpoints     - Current RPC endpoints mapped
        + SCCM                   - System Center Configuration Manager (SCCM) settings, if applicable
        + ScheduledTasks         - Scheduled tasks (via WMI) that aren't authored by 'Microsoft', "-full" dumps all Scheduled tasks
          SearchIndex            - Query results from the Windows Search Index, default term of 'password'. (argument(s) == <search path> <pattern1,pattern2,...>
          SecurityPackages       - Enumerates the security packages currently available using EnumerateSecurityPackagesA()
          Services               - Services with file info company names that don't contain 'Microsoft', "-full" dumps all processes
          SlackDownloads         - Parses any found 'slack-downloads' files
          SlackPresence          - Checks if interesting Slack files exist
          SlackWorkspaces        - Parses any found 'slack-workspaces' files
        + Sysmon                 - Sysmon configuration from the registry
          SysmonEvents           - Sysmon process creation logs (1) with sensitive data.
          TcpConnections         - Current TCP connections and their associated processes and services
          TokenGroups            - The current token's local and domain groups
          TokenPrivileges        - Currently enabled token privileges (e.g. SeDebugPrivilege/etc.)
        + UAC                    - UAC system policies via the registry
          UdpConnections         - Current UDP connections and associated processes and services
          UserRightAssignments   - Configured User Right Assignments (e.g. SeDenyNetworkLogonRight, SeShutdownPrivilege, etc.) argument == computername to enumerate
        + WindowsAutoLogon       - Registry autologon information
          WindowsCredentialFiles - Windows credential DPAPI blobs
        + WindowsDefender        - Windows Defender settings (including exclusion locations)
        + WindowsEventForwarding - Windows Event Forwarding (WEF) settings via the registry
        + WindowsFirewall        - Non-standard firewall rules, "-full" dumps all (arguments == allow/deny/tcp/udp/in/out/domain/private/public)
          WindowsVault           - Credentials saved in the Windows Vault (i.e. logins from Internet Explorer and Edge).
          WMIEventConsumer       - Lists WMI Event Consumers
          WMIEventFilter         - Lists WMI Event Filters
          WMIFilterBinding       - Lists WMI Filter to Consumer Bindings
        + WSUS                   - Windows Server Update Services (WSUS) settings, if applicable


    Seatbelt has the following command groups: All, User, System, Slack, Chrome, Remote, Misc

        You can invoke command groups with "Seatbelt.exe <group>"

       "Seatbelt.exe -group=all" runs all commands

       "Seatbelt.exe -group=user" runs the following commands:

            ChromePresence, CloudCredentials, CredEnum, dir, DpapiMasterKeys,
            ExplorerMRUs, ExplorerRunCommands, FirefoxPresence, IdleTime,
            IEFavorites, IETabs, IEUrls, MappedDrives,
            OfficeMRUs, PuttyHostKeys, PuttySessions, RDCManFiles,
            RDPSavedConnections, SlackDownloads, SlackPresence, SlackWorkspaces,
            TokenGroups, WindowsCredentialFiles, WindowsVault

       "Seatbelt.exe -group=system" runs the following commands:

            AMSIProviders, AntiVirus, AppLocker, ARPTable, AuditPolicies,
            AuditPolicyRegistry, AutoRuns, CredGuard, DNSCache,
            DotNet, EnvironmentPath, EnvironmentVariables, InterestingProcesses,
            InternetSettings, LAPS, LastShutdown, LocalGPOs,
            LocalGroups, LocalUsers, LogonSessions, LSASettings,
            NamedPipes, NetworkProfiles, NetworkShares, NTLMSettings,
            OSInfo, PoweredOnEvents, PowerShell, Printers,
            Processes, PSSessionSettings, RDPSessions, SCCM,
            Services, Sysmon, TcpConnections, TokenPrivileges,
            UAC, UdpConnections, UserRightAssignments, WindowsAutoLogon,
            WindowsDefender, WindowsEventForwarding, WindowsFirewall, WMIEventConsumer,
            WMIEventFilter, WMIFilterBinding, WSUS

       "Seatbelt.exe -group=slack" runs the following commands:

            SlackDownloads, SlackPresence, SlackWorkspaces

       "Seatbelt.exe -group=chrome" runs the following commands:

            ChromeBookmarks, ChromeHistory, ChromePresence

       "Seatbelt.exe -group=remote" runs the following commands:

            AMSIProviders, AntiVirus, DotNet, ExplorerRunCommands, InterestingProcesses,
            LastShutdown, LogonSessions, LSASettings, MappedDrives,
            NetworkProfiles, NetworkShares, NTLMSettings, PowerShell,
            ProcessOwners, PuttyHostKeys, PuttySessions, RDPSavedConnections,
            RDPSessions, Sysmon, WindowsDefender, WindowsEventForwarding,
            WindowsFirewall

       "Seatbelt.exe -group=misc" runs the following commands:

            ChromeBookmarks, ChromeHistory, ExplicitLogonEvents, FileInfo, FirefoxHistory,
            InstalledProducts, InterestingFiles, LogonEvents, OutlookDownloads,
            PowerShellEvents, ProcessCreationEvents, ProcessOwners, RecycleBin,
            reg, RPCMappedEndpoints, ScheduledTasks, SearchIndex,
            SecurityPackages, SysmonEvents


    Examples:
        'Seatbelt.exe <Command> [Command2] ...' will run one or more specified checks only
        'Seatbelt.exe <Command> -full' will return complete results for a command without any filtering.
        'Seatbelt.exe "<Command> [argument]"' will pass an argument to a command that supports it (note the quotes).
        'Seatbelt.exe -group=all' will run ALL enumeration checks, can be combined with "-full".
        'Seatbelt.exe <Command> -computername=COMPUTER.DOMAIN.COM [-username=DOMAIN\\USER -password=PASSWORD]' will run an applicable check remotely
        'Seatbelt.exe -group=remote -computername=COMPUTER.DOMAIN.COM [-username=DOMAIN\\USER -password=PASSWORD]' will run remote specific checks
        'Seatbelt.exe -group=system -outputfile="C:\\Temp\\out.txt"' will run system checks and output to a .txt file.
        'Seatbelt.exe -group=user -q -outputfile="C:\\Temp\\out.json"' will run in quiet mode with user checks and output to a .json file.
"""
    print(help_stuff)


def main(shad0w, args):

    # check we actually have a beacon
    if shad0w.current_beacon is None:
        shad0w.debug.log("ERROR: No active beacon.", log=True)
        return

    seatbelt_args = ' '.join(args[1:])

    # kind of a hack to make sure we integrate nice with the shellcode generator
    args = DummyClass()

    if len(seatbelt_args) != 0:
        args.param = seatbelt_args
    else:
        usage()
        return

    args.cls = False
    args.method = False
    args.runtime = False
    args.appdomain = False

    b64_comp_data = shellcode.generate(SEATBELT_BIN, args, seatbelt_args)

    shad0w.beacons[shad0w.current_beacon]["task"] = (USERCD_EXEC_ID, b64_comp_data)
    shad0w.beacons[shad0w.current_beacon]["callback"] = seatbelt_callback
