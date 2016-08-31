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

        static void Main(string[] args)
        {
            Subprogram p1 = new Subprogram();
            
        }
        

    }

    class Subprogram
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


        static bool initiated = false;
        static IntPtr semaphore;
        static Random rand;

        NamedPipeClient PClient;

        public Subprogram()
        {
            init();
            Thread t1 = new Thread(new ThreadStart(MyFunc));
            Thread t2 = new Thread(new ThreadStart(MyFunc));
            Thread t3 = new Thread(new ThreadStart(MyFunc));
            t1.Start();
            t2.Start();
            t3.Start();
            t1.Join();
            t2.Join();
            t3.Join();
        }

        public static void init()
        {
            if (!initiated)
            {
                semaphore = CreateSemaphore(null, 1, 1, @"Local\sema");
                rand = new Random();
                initiated = true;
            }
        }

        public string ComWithBackend(string message)
        {

            //NamedPipeClient PClient = new NamedPipeClient(".", "myNamedPipe2", "myNamedPipe1", "sem");

            
            WaitForSingleObject(semaphore, uint.MaxValue);
            Console.WriteLine("Thread {0} entering mutex.", Thread.CurrentThread.ManagedThreadId);
            string result = PClient.Run(message);
            Console.WriteLine("Thread {0} exiting mutex.", Thread.CurrentThread.ManagedThreadId);
            ReleaseSemaphore(semaphore, 1, IntPtr.Zero);

            return result;
        }

        private void MyFunc()    //fixa så C# använder CreateFile för att kommunicera med C++ koden.
        {
            PClient = new NamedPipeClient(".", "myNamedPipe2", "myNamedPipe1", "sem");

            for (int i = 0; i < 100; i++)
            {
                int ms = rand.Next(1, 10000);
                Console.WriteLine(ms);

                Thread.Sleep(ms);
                if (Thread.CurrentThread.ManagedThreadId % 2 == 0)
                {
                    Console.WriteLine(Thread.CurrentThread.ManagedThreadId);
                    Console.WriteLine("The result is:\n{0}", ComWithBackend("imgSearch\n" +
                                                                            "testfiles/dress1.jpg\n" +
                                                                            "12\n" +
                                                                            "ClothingType,Silhouette,Pattern,Color,Template,\n" +
                                                                            "100,1,2,2,2\n" +
                                                                            "None\n"));
                }
                else
                {
                    Console.WriteLine(Thread.CurrentThread.ManagedThreadId);
                    Console.WriteLine("The result is:\n{0}", ComWithBackend("imgSearch\n" +
                                                                                "testfiles/dress1.jpg\n" +
                                                                                "12\n" +
                                                                                "ClothingType,Silhouette,Pattern,Color,Template,\n" +
                                                                                "1,1,2,2,2\n" +
                                                                                "None\n"));
                }
            }

        }

    }

}
