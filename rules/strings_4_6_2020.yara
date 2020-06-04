import "pe"

rule shad0w_beacon {
meta:
    description = "Shad0w beacon defualt suspicous strings"
    author = "SBousseaden"
    reference = "https://github.com/bats3c/shad0w"
    date = "2020-06-04"
strings:
    $s1 = "LdrLoadD"
    $s2 = {53 65 74 50 72 2A 65 73 73 4D} // SetPr*essM
    $s3 = "Policy" // combined with above gives SetProcessMitigationPolicy
condition: uint16(0) == 0x5a4d and all of ($s*)
  and pe.sections[0].name == "XPU0" and pe.imports("winhttp.dll","WinHttpOpen")
}