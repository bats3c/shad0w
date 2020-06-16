typedef BOOLAPI (WINAPI * SetProcessMitigationPolicy_) (
    IN PROCESS_MITIGATION_POLICY MitigationPolicy,
    PVOID                     lpBuffer,
    IN SIZE_T                    dwLength
);