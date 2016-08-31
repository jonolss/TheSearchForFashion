using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

using System.IO;
using System.IO.Pipes;
using System.Diagnostics;



namespace FileClient
{
    public class FrontEnd
    {
        public string hello { get; private set;  }

        private bool initiated = false;
        private FileClient client;

        public FrontEnd()
        {
            hello = "Hello World!";
            if(!initiated)
                client = new FileClient("tsff_back2front", "tsff_front2back");
        }

        public string Run(string msg, string id)
        {
            return client.Run(msg, id);
        }


    }

    /*
    static class PipeServer
    {
        private static bool initiated = false;
        public static int fem = 5;

        private static NamedPipeClient PClient;

        public static void init()
        {
            if(!initiated)
            {
                initiated = true;

                PClient = new NamedPipeClient(".", "myNamedPipe2", "myNamedPipe1", "sem");

                PServer1.Start();
            }
        }

        public static void end()
        {
            initiated = false;
            PServer1.StopServer();
            PServer2.StopServer();
        }

        public static string Main()
        {
            string query = "Jag vill ha blommig falukorv till lunch, mamma.";
            PServer2.SendMessage(query, PServer2.clientse);

            string result = PServer1.ReadMessage(PServer1.clientse);
            return result;
        }

        
    }
    */
}
