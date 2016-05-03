using System;
using Microsoft.Win32.SafeHandles;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.IO;
using System.IO.Pipes;


namespace NamedPipeTest
{
    class NamedPipeClient
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr CreateSemaphore(
            SecurityAttributes lpSemaphoreAttributes,
            uint lInitialCount,
            uint lMaximumCount,
            String lpName);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr OpenSemaphore(
            uint dwDesiredAccess,
            bool bInheritHandle,
            String lpName);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool ReleaseSemaphore(
            IntPtr hSemaphore,
            ulong lReleaseCount,
            IntPtr lpPreviousCount);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern uint WaitForSingleObject(
            IntPtr hHandle,
            uint dwMilliseconds);


        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr CreateFile(
            String lpFileName,                        // file name
            uint dwDesiredAccess,                     // access mode
            uint dwShareMode,                               // share mode
            SecurityAttributes attr,                // SD
            uint dwCreationDisposition,         // how to create
            uint dwFlagsAndAttributes,          // file attributes
            uint hTemplateFile);					  // handle to template file

        [StructLayout(LayoutKind.Sequential)]
        public class SecurityAttributes
        {
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool ReadFile(
            IntPtr hHandle,                                         // handle to file
            byte[] lpBuffer,                                // data buffer
            uint nNumberOfBytesToRead,          // number of bytes to read
            byte[] lpNumberOfBytesRead,         // number of bytes read
            uint lpOverlapped                               // overlapped buffer
            );

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool WriteFile(
            IntPtr hHandle,                                         // handle to file
            byte[] lpBuffer,                              // data buffer
            uint nNumberOfBytesToWrite,         // number of bytes to write
            byte[] lpNumberOfBytesWritten,  // number of bytes written
            uint lpOverlapped                               // overlapped buffer
            );

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool CloseHandle(
            IntPtr hHandle);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern uint GetLastError();


        private IntPtr mutexSema;
        private IntPtr syncSema;
        private IntPtr clientHandleIN;
        private IntPtr clientHandleOUT;
        private string mutexSemaName;
        private string syncSemaName;
        private string pipeNameIN;
        private string pipeNameOUT;

        private const uint bufferSize = 1000;


        public NamedPipeClient(string server, string nameIN, string nameOUT, string nameSem)
        {
            this.mutexSemaName = @"Global\" + nameSem + @"_mutex";
            this.syncSemaName = @"Global\" + nameSem + @"_sync";
            this.pipeNameIN = @"\\" + server + @"\pipe\" + nameIN;
            this.pipeNameOUT = @"\\" + server + @"\pipe\" + nameOUT;
        }

        public bool ConnectToServer()
        {
            
            syncSema = OpenSemaphore(
                0x1F0003,
                false,
                syncSemaName);

            if (syncSema == IntPtr.Zero)
            {
                Console.WriteLine("CreateSyncSemaphore error: {0}", GetLastError());
                return false;
            }
            

            clientHandleOUT = CreateFile(pipeNameOUT, 0x40000000, 0, null, 3,0 /*0x40000000*/, 0);
            if (clientHandleOUT.ToInt32() > 0)
            {
                WaitForSingleObject(syncSema, uint.MaxValue);
                clientHandleIN = CreateFile(pipeNameIN, 0x80000000, 0, null, 3, 0 /*0x40000000*/, 0);
                if (clientHandleIN.ToInt32() <= 0)
                {
                    Console.WriteLine("Could not open the pipeIN  - (error {0})", GetLastError());
                    return false;
                }
            }
            else
            {
                Console.WriteLine("Could not open the pipeOUT  - (error {0})", GetLastError());
                return false;
            }
            return true;
        }

        public bool WriteToServer(string message)
        {
            if (message.Length-1 > bufferSize)
            {
                Console.WriteLine("Message is too big.");
                return false;
            }
                
            
            byte[] buffer = Encoding.UTF8.GetBytes(message);
            byte[] bytesWritten = new byte[4];

            if (!WriteFile(clientHandleOUT, buffer, bufferSize, bytesWritten, 0))
                return false;
            Console.WriteLine((int)bytesWritten[0] + ((int)bytesWritten[1]) * (byte.MaxValue + 1));
            return true;
        }

        public string ReadFromServer()
        {
            byte[] buffer = new byte[bufferSize];
            byte[] bytesRead = new byte[4];

            if (!ReadFile(clientHandleIN, buffer, bufferSize, bytesRead, 0)) // <-- Funkar inte alltid, känns stohkastiskt om det funkar.
            {
                Console.WriteLine("Could not read the pipe  - (error {0})", GetLastError());
                return "Error when trying to read from pipe.";
            }
            Console.WriteLine(bytesRead[0]);
            return Encoding.UTF8.GetString(buffer);
        }

        public void DisconnectFromServer()
        {
            CloseHandle(clientHandleIN);
            CloseHandle(clientHandleOUT);
            CloseHandle(syncSema);
        }

        public int Run()
        {
            mutexSema = OpenSemaphore(
                0x1F0003,
                false,
                mutexSemaName);

            if (mutexSema == IntPtr.Zero)
            {
                Console.WriteLine("CreateMutexSemaphore error: {0}", GetLastError());
                return 1;
            }

            Console.WriteLine("Requesting access to server...");
            if (WaitForSingleObject(mutexSema, uint.MaxValue) == 0)
            {
                ConnectToServer();

                bool loop = false;
                while (loop)
                {
                    string message = Console.ReadLine();

                    WriteToServer(message);
                    if (!message.ToLower().Equals("quit"))
                    {
                        Console.WriteLine(ReadFromServer());
                        message = Console.ReadLine();
                    }
                    else
                    {
                        loop = false;
                    }
                }

                Console.WriteLine(WriteToServer("imgSearch\n" +
                                                "testfiles/dress0.jpg\n" +
                                                "12\n" +
                                                "All\n" +
                                                "1,1,2,2,2\n" +
                                                "None\n"));

                Console.WriteLine(ReadFromServer()); //<-- doesnt 

                DisconnectFromServer();

                return 0;
            }
            else
            {
                Console.WriteLine("Server is busy.");
                return 2;
            }

        }
        
    }
}
