import "pe"
rule shad0w_beacon_16June { 
meta: 
  description = "Shad0w beacon compressed" 
  author = "SBousseaden" 
  reference = "https://github.com/bats3c/shad0w" 
  date = "2020-06-16" 
strings:
  $s1 = {F2 AE ?? ?? ?? FF 15 ?? ?? 00 00 48 09 C0 74 09}
  $s2 = {33 2E 39 36 00 ?? ?? ?? 21 0D 24 0E 0A}
  $s3 = "VirtualProtect"
  $s4 = "GetProcAddress"
condition: uint16(0) == 0x5a4d and all of ($s*) and pe.sections[0].name != "text"
}
