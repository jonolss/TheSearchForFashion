using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Runtime.InteropServices;

namespace NamedPipeTest
{

    class Program
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

        [StructLayout(LayoutKind.Sequential)]
        public class SecurityAttributes
        {
        }


        static IntPtr semaphore;

        static void Main(string[] args)
        {
            semaphore = CreateSemaphore(null, 1, 1, @"Local\sema");

            NamedPipeClient PClient1 = new NamedPipeClient(".", "myNamedPipe2", "myNamedPipe1","sem");


            Thread t1 = new Thread(new ThreadStart(MyFunc));
            t1.Start();

            for (int i = 0;i<10;i++)
            {
                WaitForSingleObject(semaphore, uint.MaxValue);
                Console.WriteLine("Main thread entering mutex.");
                PClient1.Run();
                Console.WriteLine("Main thread exiting mutex.");
                ReleaseSemaphore(semaphore, 1, IntPtr.Zero);
            }
            

        }

        private static void MyFunc()
        {
            NamedPipeClient PClient2 = new NamedPipeClient(".", "myNamedPipe2", "myNamedPipe1", "sem");

            for (int i = 0; i < 10; i++)
            {
                WaitForSingleObject(semaphore, uint.MaxValue);
                Console.WriteLine("Secondary thread entering mutex.");
                PClient2.Run();
                Console.WriteLine("Secondary thread exiting mutex.");
                ReleaseSemaphore(semaphore, 1, IntPtr.Zero);
            }
        }

    }
}
