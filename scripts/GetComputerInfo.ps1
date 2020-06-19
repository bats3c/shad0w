Function Get-ComputerInfo
{
    <#
            .SYNOPSIS
            This function will collect various data elements from a local or remote computer.
            .DESCRIPTION
            This function was inspired by Get-ServerInfo a custom function written by Jason Walker 
            and the PSInfo Sysinternals Tool written by Mark Russinovich.  It will collect a plethora
            of data elements that are important to a Microsoft Windows System Administrator.  The 
            function will run locally, run without the -ComputerName Parameter, or connect remotely 
            via the -ComputerName Parameter.  This function will return objects that you can interact 
            with, however, due to the fact that multiple custom objects are returned, when piping the 
            function to Get-Member, it will only display the first object, unless you run the following; 
            "Get-ComputerInfo | Foreach-Object {$_ | Get-Member}".  This function is currently in beta.  
            Also remember that you have to dot source the ".ps1" file in order to load it into your 
            current PowerShell console: ". .\Get-ComputerInfo.ps1"  Then it can be run as a "cmdlet" 
            aka "function".  Reminder: In it's current state, this function's output is intended for 
            the console, in other words the data does not export very well, unless the Foreach-Object 
            technique is used above.  This is something that may come in a future release or a simplied 
            version.
            .PARAMETER ComputerName
            A single Computer or an array of computer names.  The default is localhost ($env:COMPUTERNAME).
            .EXAMPLE
            PS D:\> Get-ComputerInfo -ComputerName WIN-7-01

            Computer            : WIN-7-01
            Domain              : INQU1S1T0R.LOCAL
            OperatingSystem     : Microsoft Windows 7 Professional 
            OSArchitecture      : 32-bit
            BuildNumber         : 7601
            ServicePack         : 1
            Manufacturer        : VMware, Inc.
            Model               : VMware Virtual Platform
            SerialNumber        : VMware-56 4d 0e 63 66 87 22 81-48 df af 02 e5 08 7f 7d
            Processor           : Intel(R) Core(TM) i7-6500U CPU @ 2.50GHz
            LogicalProcessors   : 2
            PhysicalMemory      : 1024
            OSReportedMemory    : 1023
            PAEEnabled          : True
            InstallDate         : 14/11/2015 21:38:36
            LastBootUpTime      : 14/11/2016 22:08:04
            UpTime              : 2.18:20:55.8121611
            RebootPending       : True
            RebootPendingKey    : False
            CBSRebootPending    : True
            WinUpdRebootPending : True
            LogonServer         : \\INQU1S1T0R-DC

            Network Adaptors 

            NICName         : Intel(R) PRO/1000 MT Network Connection
            NICManufacturer : Intel
            DHCPEnabled     : True
            MACAddress      : 00:0C:29:08:7F:7D
            IPAddress       : {192.168.0.119, fe80::31e1:f129:1265:9ec2}
            IPSubnetMask    : {255.255.255.0, 64}
            DefaultGateway  : {192.168.0.1}
            DNSServerOrder  : {192.168.0.2, 8.8.8.8, 8.8.4.4}
            DNSSuffixSearch : {inqu1s1t0r.local}
            PhysicalAdapter : True
            Speed           : 1000 Mbit

            Disk Information 

            DeviceID    : C:
            VolumeName  : 
            VolumeDirty : 
            Size        : 59.90 GB
            FreeSpace   : 45.23 GB
            PercentFree : 75.51 %

            Hotfix(s) Installed:  198 

            Description : Update
            HotfixID    : KB2849697
            InstalledOn : 

            Description : Update
            HotfixID    : KB2849696
            InstalledOn : 

            Description : Update
            HotfixID    : KB2841134
            InstalledOn : 

            Description : Update
            HotfixID    : KB2670838
            InstalledOn : 

            Description : Security Update
            HotfixID    : KB2425227
            InstalledOn : 
            .LINK
            Registry Class
            http://msdn.microsoft.com/en-us/library/microsoft.win32.registry.aspx

            Win32_BIOS
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa394077(v=vs.85).aspx

            Win32_ComputerSystem
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa394102(v=vs.85).aspx

            Win32_OperatingSystem
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa394239(v=vs.85).aspx

            Win32_NetworkAdapter
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa394216(v=vs.85).aspx

            Win32_NetworkAdapterConfiguration
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa394217(v=vs.85).aspx

            Win32_Processor
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa394373(v=vs.85).aspx

            Win32_PhysicalMemory
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa394347(v=vs.85).aspx

            Win32_LogicalDisk
            http://msdn.microsoft.com/en-us/library/windows/desktop/aa394173(v=vs.85).aspx

            Component-Based Servicing
            http://technet.microsoft.com/en-us/library/cc756291(v=WS.10).aspx

            PendingFileRename/Auto Update:
            http://support.microsoft.com/kb/2723674
            http://technet.microsoft.com/en-us/library/cc960241.aspx
            http://blogs.msdn.com/b/hansr/archive/2006/02/17/patchreboot.aspx

            SCCM 2012/CCM_ClientSDK:
            http://msdn.microsoft.com/en-us/library/jj902723.aspx
            .NOTES
            Author:    Brian C. Wilhite
            Email:     bwilhite1@carolina.rr.com
            Date:      03/31/2012
            RevDate:   11/17/2016
            PoShVer:   2.0/
            ScriptVer: 0.87 (Beta)
            0.87 - Some modifications to the script added by Dave Hardy, the script ouputs installed hotfixes, the system logon server and fixed an issue where the OS is Windows 10 not displaying the OS Architecture correctly also removed the write-progress elements to allow the script to run in the remote implant of PoshC2
            0.86 - Code clean-up, now a bit easier to read
            Added several PendingReboot properites
            RebootPendingKey - Shows contents of files pending rename
            CBSRebootPending - Component-Based Servicing, see link above
            WinUpdRebootPending - Pending Reboot due to Windows Update
            Added PAEEnabled Property
            0.85 - Now reports LogicalProcessors & Domain (2K3/2K8)
            Better PendingReboot support for Windows 2008+
            Minor Write-Progress Changes
                   
    #>

    [CmdletBinding()]
    param(
        [Parameter(Position = 0,ValueFromPipeline = $true)]
        [Alias('CN','Computer')]
        [String[]]$ComputerName = "$env:COMPUTERNAME"
    )

    Begin
    {
        $i = 0
        #Adjusting ErrorActionPreference to stop on all errors
        $TempErrAct = $ErrorActionPreference
        $ErrorActionPreference = 'Stop'
		
        #Defining $CompInfo Select Properties For Correct Display Order
        $CompInfoSelProp = @(
            'Computer'
            'Domain'
            'OperatingSystem'
            'OSArchitecture'
            'BuildNumber'
            'ServicePack'
            'Manufacturer'
            'Model'
            'SerialNumber'
            'Processor'
            'LogicalProcessors'
            'PhysicalMemory'
            'OSReportedMemory'
            'PAEEnabled'
            'InstallDate'
            'LastBootUpTime'
            'UpTime'
            'RebootPending'
            'RebootPendingKey'
            'CBSRebootPending'
            'WinUpdRebootPending'
            'LogonServer'
            'PageFile'
        )#End $CompInfoSelProp
		
        #Defining $NetInfo Select Properties For Correct Display Order
        $NetInfoSelProp = @(
            'NICName'
            'NICManufacturer'
            'DHCPEnabled'
            'MACAddress'
            'IPAddress'
            'IPSubnetMask'
            'DefaultGateway'
            'DNSServerOrder'
            'DNSSuffixSearch'
            'PhysicalAdapter'
            'Speed'
        )#End $NetInfoSelProp
		
        #Defining $VolInfo Select Properties For Correct Display Order
        $VolInfoSelProp = @(
            'DeviceID'
            'VolumeName'
            'VolumeDirty'
            'Size'
            'FreeSpace'
            'PercentFree'
        )#End $VolInfoSelProp
    }#End Begin Script Block

    Process
    {
        Foreach ($Computer in $ComputerName)
        {
            Try
            {
                If ($ComputerName.Count -gt 1)
                {
                    #Setting up Main Write-Progress Process, If Querying More Than 1 Computer.
                    $WriteProgParams = @{
                        Id              = 1
                        Activity        = "Processing Get-ComputerInfo For $Computer"
                        Status          = "Percent Complete: $([int]($i/($ComputerName.Count)*100))%"
                        PercentComplete = [int]($i++/($ComputerName.Count)*100)
                    }#End $WriteProgParam Hashtable		
                    Write-Progress @WriteProgParams
                }#End If ($ComputerName.Count -gt 1)
                        
                #Gathering WMI Data
                       
                $WMI_PROC = Get-WmiObject -Class Win32_Processor -ComputerName $Computer                      
                $WMI_BIOS = Get-WmiObject -Class Win32_BIOS -ComputerName $Computer
                $WMI_CS = Get-WmiObject -Class Win32_ComputerSystem -ComputerName $Computer
                $WMI_OS = Get-WmiObject -Class Win32_OperatingSystem -ComputerName $Computer                      
                $WMI_PM = Get-WmiObject -Class Win32_PhysicalMemory -ComputerName $Computer
                $WMI_LD = Get-WmiObject -Class Win32_LogicalDisk -Filter "DriveType = '3'" -ComputerName $Computer                  
                $WMI_NA = Get-WmiObject -Class Win32_NetworkAdapter -ComputerName $Computer                    
                $WMI_NAC = Get-WmiObject -Class Win32_NetworkAdapterConfiguration -Filter "IPEnabled=$true" -ComputerName $Computer
                $WMI_HOTFIX = Get-WmiObject -Class Win32_quickfixengineering -ComputerName $ComputerName
                $WMI_NETLOGIN = Get-WmiObject -Class win32_networkloginprofile -ComputerName $Computer
                $WMI_PAGEFILE = Get-WmiObject -Class Win32_PageFileUsage

                #Connecting to the Registry to determine PendingReboot status.
                $RegCon = [Microsoft.Win32.RegistryKey]::OpenRemoteBaseKey([Microsoft.Win32.RegistryHive]'LocalMachine',$Computer)
                #If Windows Vista & Above, CBS Will Not Write To The PFRO Reg Key, Query CBS Key For "RebootPending" Key.
                #Also, since there are properties that are exclusive to 2K8+ marking "Unaval" for computers below 2K8.
                $WinBuild = $WMI_OS.BuildNumber
                $CBSRebootPend, $RebootPending = $false, $false
                If ([INT]$WinBuild -ge 6001)
                {
                    #Querying the Component Based Servicing reg key for pending reboot status.
                    $RegSubKeysCBS  = $RegCon.OpenSubKey('SOFTWARE\Microsoft\Windows\CurrentVersion\Component Based Servicing\').GetSubKeyNames()
                    $CBSRebootPend  = $RegSubKeysCBS -contains 'RebootPending'

                    #Values that are present in 2K8+
                    $OSArchitecture = $WMI_OS.OSArchitecture
                    $LogicalProcs   = $WMI_CS.NumberOfLogicalProcessors
                }#End If ($WinBuild -ge 6001)
                Else
                {
                    #Win32_OperatingSystem does not have a value for OSArch in 2K3 & Below
                    $OSArchitecture = '**Unavailable**'

                    #In order to gather processor count for 2K3 & Below, Win32_Processor Instance Count is needed.
                    If ($WMI_PROC.Count -gt 1)
                    {
                        $LogicalProcs = $WMI_PROC.Count
                    }#End If ($WMI_PROC.Count -gt 1)
                    Else
                    {
                        $LogicalProcs = 1
                    }#End Else
                }#End Else
						
                #Querying Session Manager for both 2K3 & 2K8 for the PendingFileRenameOperations REG_MULTI_SZ to set PendingReboot value.
                $RegSubKeySM      = $RegCon.OpenSubKey('SYSTEM\CurrentControlSet\Control\Session Manager\')
                $RegValuePFRO     = $RegSubKeySM.GetValue('PendingFileRenameOperations',$false)

                #Querying WindowsUpdate\Auto Update for both 2K3 & 2K8 for "RebootRequired"
                $RegWindowsUpdate = $RegCon.OpenSubKey('SOFTWARE\Microsoft\Windows\CurrentVersion\WindowsUpdate\Auto Update\').GetSubKeyNames()
                $WUAURebootReq    = $RegWindowsUpdate -contains 'RebootRequired'
                $RegCon.Close()
						
                #Setting the $RebootPending var based on data read from the PendingFileRenameOperations REG_MULTI_SZ and CBS Key.	
                If ($CBSRebootPend -or $RegValuePFRO -or $WUAURebootReq)
                {
                    $RebootPending = $true
                }#End If ($RegValuePFRO -eq "NoValue")
						
                #Calculating Memory, Converting InstallDate, LastBootTime, Uptime.
                [int]$Memory  = ($WMI_PM | Measure-Object -Property Capacity -Sum).Sum / 1MB
                $InstallDate  = ([WMI]'').ConvertToDateTime($WMI_OS.InstallDate)
                $LastBootTime = ([WMI]'').ConvertToDateTime($WMI_OS.LastBootUpTime)
                $UpTime       = New-TimeSpan -Start $LastBootTime -End (Get-Date)
						
                #PAEEnabled is only valid on x86 systems, setting value to false first.
                $PAEEnabled = $false
                If ($WMI_OS.PAEEnabled)
                {
                    $PAEEnabled = $true
                }
						
                #Creating the $CompInfo Object
                New-Object PSObject -Property @{
                    Computer            = $WMI_CS.Name
                    Domain              = $WMI_CS.Domain.ToUpper()
                    OperatingSystem     = $WMI_OS.Caption
                    OSArchitecture      = $OSArchitecture
                    BuildNumber         = $WinBuild
                    ServicePack         = $WMI_OS.ServicePackMajorVersion
                    Manufacturer        = $WMI_CS.Manufacturer
                    Model               = $WMI_CS.Model
                    SerialNumber        = $WMI_BIOS.SerialNumber
                    Processor           = ($WMI_PROC | Select-Object -ExpandProperty Name -First 1)
                    LogicalProcessors   = $LogicalProcs
                    PhysicalMemory      = $Memory
                    OSReportedMemory    = [int]$($WMI_CS.TotalPhysicalMemory / 1MB)
                    PAEEnabled          = $PAEEnabled
                    InstallDate         = $InstallDate
                    LastBootUpTime      = $LastBootTime
                    UpTime              = $UpTime
                    RebootPending       = $RebootPending
                    RebootPendingKey    = $RegValuePFRO
                    CBSRebootPending    = $CBSRebootPend
                    WinUpdRebootPending = $WUAURebootReq
                    LogonServer         = $ENV:LOGONSERVER
                    PageFile            = $WMI_PAGEFILE.Caption
                } | Select-Object $CompInfoSelProp
						
                #There may be multiple NICs that have IPAddresses, hence the Foreach loop.
                Write-Output 'Network Adaptors'`n
                Foreach ($NAC in $WMI_NAC)
                {
                    #Getting properties from $WMI_NA that correlate to the matched Index, this is faster than using $WMI_NAC.GetRelated('Win32_NetworkAdapter'). 
                    $NetAdap = $WMI_NA | Where-Object {
                        $NAC.Index -eq $_.Index
                    }
								
                    #Since there are properties that are exclusive to 2K8+ marking "Unaval" for computers below 2K8.
                    If ($WinBuild -ge 6001)
                    {
                        $PhysAdap = $NetAdap.PhysicalAdapter
                        $Speed    = '{0:0} Mbit' -f $($NetAdap.Speed / 1000000)
                    }#End If ($WinBuild -ge 6000)
                    Else
                    {
                        $PhysAdap = '**Unavailable**'
                        $Speed    = '**Unavailable**'
                    }#End Else

                    #Creating the $NetInfo Object
                    New-Object PSObject -Property @{
                        NICName         = $NetAdap.Name
                        NICManufacturer = $NetAdap.Manufacturer
                        DHCPEnabled     = $NAC.DHCPEnabled
                        MACAddress      = $NAC.MACAddress
                        IPAddress       = $NAC.IPAddress
                        IPSubnetMask    = $NAC.IPSubnet
                        DefaultGateway  = $NAC.DefaultIPGateway
                        DNSServerOrder  = $NAC.DNSServerSearchOrder
                        DNSSuffixSearch = $NAC.DNSDomainSuffixSearchOrder
                        PhysicalAdapter = $PhysAdap
                        Speed           = $Speed
                    } | Select-Object $NetInfoSelProp
                }#End Foreach ($NAC in $WMI_NAC)
							
                #There may be multiple Volumes, hence the Foreach loop.
                Write-Output 'Disk Information'`n
                Foreach ($Volume in $WMI_LD)
                {
                    #Creating the $VolInfo Object
                    New-Object PSObject -Property @{
                        DeviceID    = $Volume.DeviceID
                        VolumeName  = $Volume.VolumeName
                        VolumeDirty = $Volume.VolumeDirty
                        Size        = $('{0:F} GB' -f $($Volume.Size / 1GB))
                        FreeSpace   = $('{0:F} GB' -f $($Volume.FreeSpace / 1GB))
                        PercentFree = $('{0:P}' -f $($Volume.FreeSpace / $Volume.Size))
                    } | Select-Object $VolInfoSelProp
                }#End Foreach ($Volume in $WMI_LD)
                Write-Output 'Hotfix(s) Installed: '$WMI_HOTFIX.Count`n
                $WMI_HOTFIX|Select-Object -Property Description, HotfixID, InstalledOn
            }#End Try

            Catch
            {
                Write-Warning "$_"
            }#End Catch
        }#End Foreach ($Computer in $ComputerName)

    }#End Process
	
    End
    {
        #Resetting ErrorActionPref
        $ErrorActionPreference = $TempErrAct
    }#End End
}#End Function Get-ComputerInfo
