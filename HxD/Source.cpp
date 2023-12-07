#pragma warning(disable:4996) 
#pragma comment (lib,"Ws2_32.lib")

#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <iomanip>
#include<stdlib.h>

using namespace std;

struct Record
{
   //unsigned char DA[6]; //адрес назначения
   unsigned char *DA; //адрес назначения
   unsigned char *SA; //адрес источника
   unsigned char type[2]; //тип фрейма
   unsigned char *data; //данные
   unsigned char *IP_S; // IP адрес источника
   unsigned char *IP_D; // IP адрес назначения
   string name; //тип фрейма в текстовом формате
   unsigned short frameSize;
};

void IPv4(unsigned char *p, Record &temp, unsigned short &length)
{
   temp.DA = p;
   temp.SA = p + 6;
   temp.IP_S = p + 26;
   temp.IP_D = p + 30;
   length = ntohs(*(unsigned short *)(p + 16));
   temp.frameSize = length + 20;
   if (length < 46)
   {
      length = 46;
   }
   temp.name = "IPv4";
}

void ARP(unsigned char *p, Record &temp, unsigned short &length)
{
   temp.DA = p;
   temp.SA = p + 6;
   temp.IP_S = NULL;
   temp.IP_D = NULL;
   temp.frameSize = 60;
   temp.name = "ARP";
}

void RAW(unsigned char *p, Record &temp, unsigned short &length)
{
   temp.DA = p;
   temp.SA = p + 6;
   temp.IP_S = p + 26;
   temp.IP_D = p + 30;
   temp.frameSize = length + 14;
   temp.name = "RAW";
}

void SNAP(unsigned char *p, Record &temp, unsigned short &length)
{
   temp.DA = p;
   temp.SA = p + 6;
   temp.IP_S = p + 26;
   temp.IP_D = p + 30;
   temp.frameSize = length + 14;
   temp.name = "SNAP";
}

void LLC(unsigned char *p, Record &temp, unsigned short &length)
{
   temp.DA = p;
   temp.SA = p + 6;
   temp.IP_S = NULL;
   temp.IP_D = NULL;
   temp.frameSize = length + 14;
   temp.name = "LLC";
}

void DIX(unsigned char *p, Record &temp, unsigned short &length)
{
   temp.DA = p;
   temp.SA = p + 6;
   temp.IP_S = p + 26;
   temp.IP_D = p + 30;
   temp.frameSize = 60;
   temp.name = "DIX";
}

int main()
{
   setlocale(LC_ALL, "Rus"); //русская локализация
   vector<Record> V; //вектор фреймов/кадров/записей
   Record temp; //текущая/обрабатываемая запись
   unsigned short type, length; //тип фрейма, длина поля данных
   vector<int> frameCount(6); //количество фреймов разного типа
   const char *frameType[6] = { "IPV4: ","ARP: ","DIX: " ,"RAW: ","SNAP: ","LLC: " }; //типы фреймов
   string filename; //имя файла

   ifstream file;
   while (true)
   {
      cout << "Введите название файла: ";
      cin >> filename;
      file.open(filename, ios::in | ios::binary | ios::ate);
      if (file) break;
      else cout << "Файл не найден!" << endl;
   }

   //ifstream file("ethers07.bin", ios::in | ios::binary | ios::ate);
   int size = file.tellg();
   file.clear();
   file.seekg(0, ios::beg);

   unsigned char *buff = new unsigned char[size];
   file.read((char *)buff, size);
   file.close();

   cout << "Size of " << filename << ": " << size << " bytes" << endl << endl;
   unsigned char *p = buff;
   buff += size;

   while (p < buff)
   {
      
      type = ntohs(*(unsigned short *)(p + 12));
      length = type;

      if (type == 0x0800)
      {
         IPv4(p, temp, length);
         p += length + 14;
         frameCount[0]++;
      }
      else
         if (type == 0x0806)
         {
            ARP(p, temp, length);
            p += 60;
            frameCount[1]++;
         }
         else
            if (type > 0x05FE)
            {
               DIX(p, temp, length);
               p += 60;
               frameCount[2]++;
            }
            else
               if (type <= 0x05FE)
               {
                  type = ntohs(*(unsigned short *)(p + 14));
                  if (type == 0xFFFF)
                  {
                     RAW(p, temp, length);
                     p += length + 14;
                     frameCount[3]++;
                  }
                  else
                     if (type == 0xAAAA)
                     {
                        SNAP(p, temp, length);
                        p += length + 14;
                        frameCount[4]++;
                     }
                     else
                        if (type != 0xFFFF && type != 0xAAAA)
                        {
                           LLC(p, temp, length);
                           p += length + 14;
                           frameCount[5]++;
                        }
               }
      cout << length << endl << endl;
      V.push_back(temp);
   }

   for (unsigned int i = 0; i < V.size(); i++)
   {
      cout << "Frame number: " << dec << i + 1 << endl;
      cout << "MAC dest: ";
      for (int j = 0; j < 5; j++)
      {
         cout << hex << uppercase << setfill('0') << setw(2) << (int)V[i].DA[j] << ':';
      }
      cout << hex << uppercase << setfill('0') << setw(2) << (int)V[i].DA[5] << endl;

      cout << "MAC source: ";
      for (int j = 0; j < 5; j++)
      {
         cout << hex << uppercase << setfill('0') << setw(2) << (int)V[i].SA[j] << ':';
      }
      cout << hex << uppercase << setfill('0') << setw(2) << (int)V[i].SA[5] << endl;

      cout << "Frame type: " << V[i].name << endl;

      if (V[i].name != "ARP" && V[i].name != "LLC")
      {
         cout << "IP source: ";
         for (int j = 0; j < 3; j++)
         {
            cout << dec << (int)V[i].IP_S[j] << '.';
         }
         cout << dec << (int)V[i].IP_S[3] << endl;

         cout << "IP dest: ";
         for (int j = 0; j < 3; j++)
         {
            cout << dec << (int)V[i].IP_D[j] << '.';
         }
         cout << dec << (int)V[i].IP_D[3] << endl;
      }

      cout << "Frame size: " << dec << V[i].frameSize << " bytes" << endl << endl;
   }


   cout << "Frame count = " << V.size() << endl;
   for (int i = 0; i < 6; i++)
      cout << frameType[i] << frameCount[i] << endl;
   cout << endl;


   system("pause");
   return 0;
}


//#pragma warning(disable:4996)
//#pragma comment (lib,"Ws2_32.lib")
//#include <stdio.h>
//#include <WinSock.h>
//
//void MAC(FILE *out, char *mac)
//{
//   int i;
//   for (i = 0; i < 5; i++)
//      fprintf(out, "%02X:", (unsigned char)mac[i]);
//   fprintf(out, "%02X\n", (unsigned char)mac[i]);
//}
//
//void IP(FILE *out, char *ip)
//{
//   int i;
//   for (i = 0; i < 3; i++)
//      fprintf(out, "%d.", (unsigned char)ip[i]);
//   fprintf(out, "%d\n", (unsigned char)ip[i]);
//}
//
//void main() {
//   FILE *in = NULL;
//   FILE *out = NULL;
//   char name[100];
//   while (true) {
//      printf("Enter the file name: ");
//      scanf("%s", &name);
//      in = fopen(name, "rb");
//      if (in)
//         break;
//      else
//         printf("Error: file not found\n");
//   }
//   fseek(in, 0, SEEK_END);
//   int size = ftell(in);
//   fseek(in, 0, SEEK_SET);
//   char *data = new char[size];
//   fread(data, size, 1, in);
//   fclose(in);
//   out = fopen("result.txt", "w");
//   fprintf(out, "Size of %s: %d bytes\n", name, size);
//   fprintf(out, "\n\n");
//   const char *frameType[6] = { "IPV4: ","ARP: ","DIX: ","RAW: ","SNAP: ","LLC: " };
//   int frameTypeCount[6] = { 0 };
//   int frameCount = 1;
//   char *p = data;
//   while (p < (data + size))
//   {
//      fprintf(out, "Frame number: %d\n", frameCount);
//      fprintf(out, "MAC destination: ");
//      MAC(out, p);
//      fprintf(out, "MAC source: ");
//      MAC(out, p + 6);
//      unsigned short LT = ntohs(*(unsigned short *)(p + 12));
//      if (LT == 0x0800)
//      {
//         fprintf(out, "Frame type: IPv4\n");
//         LT = ntohs(*(unsigned short *)(p + 16));
//         if (LT < 46)
//            LT = 46;
//         fprintf(out, "Frame size: %d bytes\n", LT + 14);
//         fprintf(out, "IP source: ");
//         IP(out, p + 26);
//         fprintf(out, "IP destination: ");
//         IP(out, p + 30);
//         fprintf(out, "\n\n");
//         p += LT + 14;
//         frameCount++;
//         frameTypeCount[0]++;
//      }
//      else if (LT == 0x0806)
//      {
//         fprintf(out, "Frame type: ARP\n");
//         fprintf(out, "Frame size: %d bytes\n", 46 + 14);
//         fprintf(out, "\n\n");
//         p += 46 + 14;
//         frameCount++;
//         frameTypeCount[1]++;
//      }
//      else
//      {
//         if (LT > 0x05FE)
//         {
//            fprintf(out, "Frame type: Ethernet DIX (Ethernet II)\n");
//            fprintf(out, "Frame size: %d bytes\n", 46 + 14);
//            frameTypeCount[2]++;
//            p += (46 + 14);
//         }
//         else if (LT <= 0x05FE)
//         {
//            unsigned short FrameT = ntohs(*(unsigned short *)(p + 14));
//            if (FrameT == 0xFFFF)
//            {
//               fprintf(out, "Frame type: Raw 802.3(Ethernet802.3)\n");
//               frameTypeCount[3]++;
//            }
//            else if (FrameT == 0xAAAA)
//            {
//               fprintf(out, "Frame type: Ethernet SNAP\n");
//               frameTypeCount[4]++;
//            }
//            else
//            {
//               fprintf(out, "Frame type:802.3/LLC(Ethernet 802.2)\n");
//               frameTypeCount[5]++;
//            }
//            fprintf(out, "Frame size: %d bytes\n", LT + 14);
//            p += (LT + 14);
//         }
//         fprintf(out, "\n\n");
//         frameCount++;
//      }
//   }
//
//   for (int i = 0; i < 6; i++)
//      fprintf(out, "%s%d ", frameType[i], frameTypeCount[i]);
//   fprintf(out, "\ntotal frame: %d", frameCount - 1);
//   fclose(out);
//}
