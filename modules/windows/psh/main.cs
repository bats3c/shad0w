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

            string b64_cmd = "Super 1337 Powershell";

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