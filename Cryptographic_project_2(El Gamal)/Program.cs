using System;
using System.Collections.Generic;
using System.Text;
using System.Numerics;
using Xamarin.Forms.PlatformConfiguration;
using System.IO;
//using System.locale;

namespace Cryptographic_project_2
{
    class Program
    {
        public static bool SimpleNum(BigInteger p)
        {
            int i = 2;
            while (i * i <= p)
            {
                if (p % i == 0) return false;
                i++;
            }
            return true;
        }

        static List<BigInteger> Factorization(BigInteger n)
        {
            List<BigInteger> div = new List<BigInteger>();
            BigInteger number = n, pr = 2;
            int flag;
            while (number != 1 && pr * pr <= n)
            {
                flag = 0;
                while (number % pr == 0)
                {
                    number /= pr;
                    flag = 1;
                }
                if (flag == 1)
                    div.Add(pr);
                pr++;
            }
            return div;
        }

        public static bool PrimitiveRoot(BigInteger p, BigInteger g)
        {
            List<BigInteger> div = new List<BigInteger>();
            div = Factorization(p - 1);
            foreach (var d in div) //проверка среди простых делителей
            {
                BigInteger exp = (p - 1) / d;
                if (BigInteger.ModPow(g, exp, p) == 1) return false;
            }
            return true;
        }

        public static void Encryption(BigInteger p, BigInteger g, string S, BigInteger x, BigInteger y, List<BigInteger> a, List<BigInteger> b)
        {
            Random rand = new Random();
            byte[] M = Encoding.UTF8.GetBytes(S);
            //byte[] M = Convert.ToByte(S);
            BigInteger k;
            for (int i = 0; i < M.Length; i++)
            {
                k = RandomBigInteger(p - 1);
                a.Add(BigInteger.ModPow(g, (int)k, p));
                b.Add((BigInteger.Pow(y, (int)k) * M[i]) % p);
            }
        }

        public static string Decryption(BigInteger p, BigInteger g, List<BigInteger> a, List<BigInteger> b, BigInteger x)
        {
            byte[] M1 = new byte[b.Count];
            for (int i = 0; i < M1.Length; i++)
                M1[i] = (byte)(b[i] * BigInteger.Pow(a[i], (int)(p - 1 - x)) % p);
            return Encoding.UTF8.GetString(M1);
            //return Convert.ToString(M1);
        }

        static BigInteger RandomBigInteger(BigInteger p)
        {
            byte[] bytes = p.ToByteArray();
            BigInteger a;
            Random random = new Random();
            do
            {
                random.NextBytes(bytes);
                bytes[bytes.Length - 1] &= (byte)0x7F; //force sign bit to positive
                a = new BigInteger(bytes);
            } while (a >= p);
            return a;
        }

        static void Main(string[] args)
        {
            BigInteger y = 0;
            long p, x, g;
            Random rand = new Random();
           // Console.OutputEncoding = Encoding.UTF8;
           // Console.OutputEncoding = Encoding.GetEncoding("Windows-1251");
            Console.WriteLine("Введите простое число p:");
            p = Convert.ToInt64(Console.ReadLine());
            if (SimpleNum(p))
            {
                Console.WriteLine("p - простое число");
                Console.WriteLine("Введите первообразный корень g по модулю p:");
                g = Convert.ToInt64(Console.ReadLine());
                if (PrimitiveRoot(p, g))
                {
                    Console.WriteLine("g - первообразный корень p");
                    Console.WriteLine("Введите шифруемое слово");
                    string S, S1;
                    S = Console.ReadLine();
                    List<BigInteger> a = new List<BigInteger>();
                    List<BigInteger> b = new List<BigInteger>();
                    x = (long)RandomBigInteger(p - 1); //закрытый ключ
                    y = BigInteger.ModPow(g, x, p); //открытый ключ
                    Encryption(p, g, S, x, y, a, b);
                    Console.WriteLine("Зашифрованное сообщение:");
                    for (int i = 0; i < b.Count; i++)
                        Console.WriteLine($"{i + 1}. [a,b]:[{a[i]}, {b[i]}]");
                    Console.WriteLine($"\nРасшифрованное сообщение: {S1 = Decryption(p, g, a, b, x)}");
                    using (StreamWriter sw = new StreamWriter("output.txt"))
                    {
                       sw.WriteLine(S1);
                    }
                }
                else Console.WriteLine("g - НЕ первообразный корень p");
            }
            else Console.WriteLine("p - НЕ простое число");
            Console.ReadKey();
        }
    }
}
//997 корень 7