using System;
using System.Net;
using System.Collections.Generic;
using System.Net.NetworkInformation;

namespace OSLab5
{
    class Program
    {
        public static IEnumerable<IPAddress> GetTraceRoute(string hostname)
        {
            const int timeout = 10000;
            const int maxTTL = 30;
            const int bufferSize = 32;

            byte[] buffer = new byte[bufferSize];
            new Random().NextBytes(buffer);
            Ping pinger = new Ping();

            for (int ttl = 1; ttl <= maxTTL; ttl++)
            {
                PingOptions options = new PingOptions(ttl, true);
                PingReply reply = pinger.Send(hostname, timeout, buffer, options);

                if (reply.Status == IPStatus.TtlExpired)
                {
                    yield return reply.Address;
                    continue;
                }
                if (reply.Status == IPStatus.TimedOut)
                {
                    yield return IPAddress.Parse("0.0.0.0");
                    continue;
                }
                if (reply.Status == IPStatus.Success)
                {
                    Console.WriteLine(reply.Address);
                    yield return reply.Address;
                }

                break;
            }
        }
        static void Main(string[] args)
        {
            Console.Write("Enter hostname: ");
            string hostname = Console.ReadLine();
            try
            {
                IEnumerable<IPAddress> addresses = GetTraceRoute(hostname);
                foreach (var i in addresses)
                {
                    if (i.ToString() != "0.0.0.0")
                        Console.WriteLine(i);
                    else Console.WriteLine("*******");
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Source, e.Message);
            }

        }
    }
}
