using System;
using Microsoft.Win32.SafeHandles;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;


namespace FileClient
{
    class TimeOutException : System.Exception { }


    class FileClient
    {
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
        public static extern uint SetFilePointer(
            IntPtr hFile,                                         // handle to file
            uint lDistanceToMove,                              
            IntPtr lpDistanceToMoveHigh,
            uint dwMoveMethod
            );


        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool CloseHandle(
            IntPtr hHandle);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern uint GetLastError();


        private IntPtr clientHandleIN;
        private IntPtr clientHandleOUT;
        private string fileNameIN;
        private string fileNameOUT;

        private static bool initiated = false;

        private const uint bufferSize = 1000000;
        private const int TIME_OUT_TIME = 10000;  //ms


        public FileClient(string nameIN, string nameOUT)
        {
            this.fileNameIN = @"D:\" + nameIN;
            this.fileNameOUT = @"D:\" + nameOUT;
        }

        /**Tries to open both input and output files.
        *
        * \return Return true if successfully opened the files, false otherwise.
        */
        public bool ConnectToServer()
        {
            if (ConnectToServerOut())
            {
                if (ConnectToServerIn())
                    return true;
                else
                    CloseHandle(clientHandleOUT);
            }
            return false;
        }
        
        /**Tries to open file where results can be read/recieved.
        *
        * \return Return true if successfully opened the file, false otherwise.
        */
        public bool ConnectToServerOut()
        {
            clientHandleOUT = CreateFile(fileNameOUT, 0x40000000 | 0x80000000 /*0x80000000*/, 0 /*0x00000002 | 0x00000001*/, null, 4, 0x00000002, 0);
            if (clientHandleOUT == IntPtr.Zero | clientHandleOUT == IntPtr.Subtract(IntPtr.Zero, 1))
                return false;
            else
                return true;
        }
        
        /**Tries to open file where requests can be written/sent.
        *
        * \return Return true if successfully opened the file, false otherwise.
        */
        public bool ConnectToServerIn()
        {
            clientHandleIN = CreateFile(fileNameIN, 0x40000000 | 0x80000000 /*0x40000000*/, 0 /*0x00000001 | 0x00000002*/, null, 4, 0x00000002, 0);
            if (clientHandleIN == IntPtr.Zero | clientHandleIN == IntPtr.Subtract(IntPtr.Zero, 1))
                return false;
            else
                return true;
        }
        
        /**Sends a request to the backend.
        *
        * \param message The request that should be sent to the backend.
        * \return Returns "true" if succesfull, "false" if failed. Can also return "INVALID_HANDLE_VALUE".
        */
        public string WriteToServer(string message)
        {
            if (message.Length - 1 > bufferSize)
            {
                Console.WriteLine("Message is too big.");
                return "false";
            }

            byte[] buffer = Encoding.UTF8.GetBytes(message);
            byte[] bytesWritten = new byte[4];

            if (clientHandleOUT == IntPtr.Zero)
                return "false";

            if (clientHandleOUT == IntPtr.Subtract(IntPtr.Zero,1))
                return "INVALID_HANDLE_VALUE";

            SetFilePointer(
                clientHandleOUT,                                  
                0,
                IntPtr.Zero,
                2
                );

            if (!WriteFile(clientHandleOUT, buffer, (uint)buffer.Length, bytesWritten, 0))
                return GetLastError().ToString();
            Console.WriteLine((int)bytesWritten[0] + ((int)bytesWritten[1]) * (byte.MaxValue + 1));
            return "true";
        }
        
        /**Fetches the result from backends output.
        *
        * \param id Identifier to the sent request.
        * \return Message from backend. If timed out, returns "Timed out".
        */
        public string ReadFromServer(string id)
        {
            Stopwatch timer = new Stopwatch();
            timer.Start();

            byte[] buffer = new byte[bufferSize];
            byte[] bytesRead = new byte[4];
            string result = "";
            
            bool found = false;
            while (!found && timer.ElapsedMilliseconds < (int)(TIME_OUT_TIME))
            {
                if (!ReadFile(clientHandleIN, buffer, bufferSize, bytesRead, 0)) // <-- Funkar inte alltid, känns stohkastiskt om det funkar.
                {
                    return "Error when trying to read from pipe. - (error " + GetLastError().ToString() + ")";
                }
                result = Encoding.UTF8.GetString(buffer);

                int foundPos = -1;
                string[] responses = result.Split('\n');
                for (int i=0; i < responses.Length;i++) //SLOW
                {
                    if (string.Compare(responses[i].Substring(0,42),id) == 0) //This seems to be extremely slow.
                    {
                        found = true;
                        foundPos = i;
                        result = responses[i];
                    }
                }

                if (!found)
                {
                    DisconnectFromServerIn();
                    Thread.Sleep(10);
                    
                    while(!ConnectToServerIn())
                    {
                        Thread.Sleep(10);
                        if (timer.ElapsedMilliseconds > (int)(TIME_OUT_TIME))
                        {
                            timer.Stop();
                            throw new TimeOutException();
                        }     
                    }
                }
                else
                {
                    found = true;
                }
            }
            

            if (timer.ElapsedMilliseconds >= (int)(TIME_OUT_TIME / 10))
                result = "Timed out";
            timer.Stop();

            string finalRespons = result;

            return finalRespons;
        }

        /**Disconnects input and output from backend.
        */
        public void DisconnectFromServer()
        {
            DisconnectFromServerIn();
            DisconnectFromServerOut();
        }
        
        /**Disconnects input from backend.
        */
        public void DisconnectFromServerIn()
        {
            CloseHandle(clientHandleIN);
        }
        
        /**Disconnects output from backend.
        */
        public void DisconnectFromServerOut()
        {
            CloseHandle(clientHandleOUT);
        }

        /**Sends a request to the backend and returns the result.
        *
        * \param msg The requests that is going to be sent to the backend.
        * \param id An unique identifier for this request.
        * \return Result from backend or error message if the requests timed out.
        */
        public string Run(string msg, string id)
        {
            try
            {
                while (!ConnectToServerOut()) { Thread.Sleep(10); }
                WriteToServer(msg);
                DisconnectFromServerOut();

                Stopwatch timer = new Stopwatch();
                timer.Start();
                while (!ConnectToServerIn())
                {
                    Thread.Sleep(10);
                    if (timer.ElapsedMilliseconds > (int)(TIME_OUT_TIME))
                        throw new TimeOutException();
                }
                string result = ReadFromServer(id);
                DisconnectFromServerIn();

                return result;
            }
            catch (TimeOutException e)
            {
                DisconnectFromServerIn();
                return "Request timed out.";
            }
        }

    }
}
