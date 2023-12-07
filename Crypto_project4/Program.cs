using System;
using System.IO;
using System.Security.Cryptography;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OK_4
{
	class Program
	{
		public static byte[] AES_Cipher(byte[] password_hash, byte[] data)
		{
			password_hash = password_hash.Where((item, i) => i < 16).ToArray(); //Фильтрация последовательности значений на основе заданного предиката
			Aes crypt_AES = Aes.Create();   //Абстрактный базовый класс, от которого наследуются все реализации стандарта AES
			crypt_AES.IV = password_hash;   //Абстрактный базовый класс c ключом, вектором инициализации IV в режиме сцепления блоков шифротекста CBC,типом заполнения Zeros
			crypt_AES.Key = password_hash;
			crypt_AES.Mode = CipherMode.CBC;
			crypt_AES.Padding = PaddingMode.Zeros;
			byte[] cipher;

			//Объект-шифратор для алгоритма симметричного шифрования с текущим свойством Key и вектором инициализации IV
			using (ICryptoTransform ict = crypt_AES.CreateEncryptor(crypt_AES.Key, crypt_AES.IV))
			{
				using (MemoryStream ms = new MemoryStream())    //Создаем поток, память которого - резервное хранилище
				{
					using (CryptoStream cs = new CryptoStream(ms, ict, CryptoStreamMode.Write))   //Поток, связывающий потоки данных с криптографическими преобразованиями
						cs.Write(data, 0, data.Length); //Текстовое представление заданного значения в стандартный выходной поток
					cipher = ms.ToArray();
				}
			}
			return cipher;
		}

		public static byte[] AES_Decipher(byte[] password_hash2, byte[] cip_data)
		{
			password_hash2 = password_hash2.Where((item, i) => i < 16).ToArray();   //Фильтрация последовательности значений на основе заданного предиката
			var crypt_AES = Aes.Create();   //Абстрактный базовый класс, от которого наследуются все реализации стандарта AES
			crypt_AES.Key = password_hash2; //Абстрактный базовый класс c ключом, вектором инициализации IV в режиме сцепления блоков шифротекста CBC,типом заполнения Zeros
			crypt_AES.IV = password_hash2;
			crypt_AES.Mode = CipherMode.CBC;
			crypt_AES.Padding = PaddingMode.Zeros;
			byte[] file;

			//Объект - шифратор для алгоритма симметричного шифрования с текущим свойством Key и вектором инициализации IV
			using (ICryptoTransform ict = crypt_AES.CreateDecryptor(crypt_AES.Key, crypt_AES.IV))
			{
				using (MemoryStream ms = new MemoryStream())    //Создаем поток, память которого резервное хранилище
				{
					using (CryptoStream cs = new CryptoStream(ms, ict, CryptoStreamMode.Write))   //Поток, связывающий потоки данных с криптографическими преобразованиями
						cs.Write(cip_data, 0, cip_data.Length);
					file = ms.ToArray();
				}
			}
			return file;
		}

		static void Main(string[] args)
		{
			int choice;
			Console.WriteLine("1 - Зашифровать файл, 2 - Расшифровать файл");
			choice = Console.Read();
			Console.ReadLine();

			if (choice == '1') // Шифрование
			{
				Console.WriteLine("Введите пароль: ");
				byte[] password_byte = Encoding.Default.GetBytes(Console.ReadLine());   //Запись пароля в массив байтов
				int length_of_h = 32;
				byte[] password_hash = new byte[length_of_h];
				var hash = HashAlgorithm.Create("SHA256");               //Создает экземпляр реализации хэш-алгоритма
				password_hash = hash.ComputeHash(password_byte);    //Вычисляет хэш-значение для входных данных

				if (length_of_h != 0)
				{
					Console.WriteLine("Введите имя файла: ");
					var bytes_fr_image = File.ReadAllBytes(Console.ReadLine()); //Запись содержимого файла в массив байтов
					var crp_data = AES_Cipher(password_hash, bytes_fr_image);   //Шифрование с помощью AES

					Console.WriteLine("Введите имя файла для результата: ");
					File.WriteAllBytes(Console.ReadLine(), crp_data);   //Запись в файл указанного массив байтов
				}
			}
			else if (choice == '2') // Дешифровка
			{
				Console.WriteLine("Введите пароль: ");
				byte[] password_byte2 = Encoding.Default.GetBytes(Console.ReadLine());
				int length_of_h = 32;
				byte[] password_hash2 = new byte[length_of_h];
				var hash = HashAlgorithm.Create("SHA256");               //Создает экземпляр реализации хэш-алгоритма
				password_hash2 = hash.ComputeHash(password_byte2);  //Вычисляет хэш-значение для входных данных

				if (length_of_h != 0)
				{
					Console.WriteLine("Введите имя зашифрованного файла: ");
					var bytes_fr_image = File.ReadAllBytes(Console.ReadLine());     //Запись содержимого файла в массив байтов
					var decr_data = AES_Decipher(password_hash2, bytes_fr_image);   //Расшифровка с помощью AES

					Console.WriteLine("Введите имя файла для результата: ");
					File.WriteAllBytes(Console.ReadLine(), decr_data);  //Запись в файл указанного массив байтов
				}
			}
			else Console.WriteLine("Ошибка ввода!");
		}
	}
}
