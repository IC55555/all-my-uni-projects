#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

int N, NS1;



void input()
{
   vector<double> x;
   ifstream in;
   in.open("info.txt");
   in >> N >> NS1;

   in.close();

   in.open("xy.txt");
   
   for (int i = 0; i < N; i++)
   {
      in >> x[i];
   }


   in.close();

   vector<double> S1;
   in.open("S1.txt");
   for (int i = 0; i < NS1; i++)
   {
      int size;
      in >> size;
      S1.resize(size);
      for (int j = 0; j < size; j++)
      {
         in >> S1[i];
      }
   }
   in.close();


   vector<vector<double>> elems;
   in.open("elems.txt");
   int row_count, col_count;
   in >> row_count >> col_count;
   elems.resize(row_count);
   for (int i = 0; i < row_count; i++)
   {
      elems[i].resize(col_count);
      for (int j = 0; j < col_count; j++)
      {
         in >> elems[i][j];
      }
      
   }
   

   in.close();
}