#include <iostream>
#include <locale>
#include <fstream>
#include <algorithm>
#include <vector>
#include <functional>
#include <string>
#include "Converter.h"
#include <Windows.h>
#include <conio.h>

using namespace std;

string alphabet = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";

// return an integral random number in the range 0 - (n - 1)
int Rand(int n)
{
   return (rand() + time(nullptr)) % n;
}

void shuffle(string &str)
{
   random_shuffle(str.begin(), str.end(), pointer_to_unary_function<int, int>(Rand));
}

void prepare(string &str)
{
   for (unsigned int i = 0; i < str.length(); i++)
   {
      if (str[i] == 32) //удаляем совпадающие/неиспользуемые коды
      {
         str.erase(str.begin() + i);
         i--;
      }
   }
   for (unsigned int i = 0; i < str.length(); i++)
   {
      if (str[i] == -63) //проверка для символа ё, код которого частично совпадает с кодом Б
      {
         if (str[i - 1] != 0)
         {
            str[i] = -72; //делаем код -72 (ё)
            str[i - 1] = 0; //на всякий случай (третье условие)
         }
      }
      else if (str[i] == -79) //проверка для символа Ё, код которого частично совпадает с кодом с
      {
         if (str[i - 1] == 0)
         {
            str[i] = -72; //делаем код -72 (ё), тем самым заменяя заглавную Ё на строчную
         }
      }
      else if (str[i] == -72) //проверка для символа ш, код которого частично совпадает с кодом ё
      {
         if (str[i - 1] != 0)
         {
            str[i] = -8; //делаем код -8 (ш)
         }
      }
   }
   for (unsigned int i = 0; i < str.length(); i++)
   {
      if (!(str[i] >= -80 && str[i] <= -17)) //удаляем все коды, не относящиеся к рабочему диапазону
      {
         if (str[i] != -8) //не трогаем ш
         {
            str.erase(str.begin() + i);
            i--;
         }
      }
      else if ((str[i] >= -80) && (str[i] <= -65)) //восстановление "обособленной" части алфавита
      {
         if (str[i] != -72) //не трогаем ё
         {
            str[i] += 64;
         }
      }
   }
   for (unsigned int i = 0; i < str.length(); i++) //заменяем заглавные на строчные
   {
      if ((str[i] >= -64) && (str[i] <= -33))
      {
         str[i] += 32;
      }
   }
}

char f(char c, string key)
{
   int index = alphabet.find(c);
   return key[index];
}

char g(char c, string key)
{
   int index = key.find(c);
   return alphabet[index];
}

void encrypt(string &str, string key)
{
   for (int i = 0; i < str.length(); i++)
   {
      str[i] = f(str[i], key);
   }
}

void decrypt(string &str, string key)
{
   for (int i = 0; i < str.length(); i++)
   {
      str[i] = g(str[i], key);
   }
}

int menu()
{
   HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
   cout << "Для шифрования текста нажмите: 1 \nДля дешифрования текста нажмите: 2" << endl;
   char key = _getch();
   while (key != '1' && key != '2')
   {
      cout << "Некоректный ввод\n";
      cout << "Для шифрования текста нажмите: 1 \nДля дешифрования текста нажмите: 2" << endl;
      key = _getch();
   }
   if (key == '1')
   {
      SetConsoleTextAttribute(handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
      cout << endl << "Шифрование выполнено." << endl;
      SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
      return 1;
   }
   else
   {
      SetConsoleTextAttribute(handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
      cout << endl << "Дешифрование выполнено." << endl;
      SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
      return 2;
   }
}

void encrypt_main()
{
   ifstream input("input.txt");
   string temp, text = "";
   string key = alphabet;
   shuffle(key);

   while (!input.eof())
   {
      getline(input, temp);
      temp = ToUTF8(temp);
      prepare(temp);
      encrypt(temp, key);
      text += temp;
   }

   input.close();
   ofstream key_save("key.txt");
   key_save << key;
   key_save.close();
   ofstream output("output_encrypt.txt");
   output << text;
   output.close();
}

void decrypt_main()
{
   ifstream input("input.txt");
   string temp;
   ifstream key_read("key.txt");
   string key;

   getline(input, temp);
   getline(key_read, key);
   temp = ToUTF8(temp);
   prepare(temp);
   decrypt(temp, key);

   input.close();
   ofstream output("output_decrypt.txt");
   output << temp;
   output.close();
   key_read.close();
}

int main()
{
   setlocale(LC_ALL, "Russian");
   int flag = menu();

   if(flag == 1)
   encrypt_main();
   else
   decrypt_main();
   system("pause");
}