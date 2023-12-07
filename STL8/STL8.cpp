#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace chrono;

int parallel()
{
   ifstream input;
   input.open("input.txt");
   int V, E;
   input >> V >> E;
   if (!input)
   {
      return 0;
   }
   int **A = new int *[V];
   for (int i = 0; i < V; i++)
   {
      A[i] = new int[V];
      for (int j = 0; j < V; j++)
         A[i][j] = 0;
   }
   int i, j;
   for (int k = 0; k < E; k++)
   {
      input >> i >> j;
      A[i - 1][j - 1] = 1;
      A[j - 1][i - 1] = 1;
   }

   time_point<system_clock> start_time, end_time;
   start_time = system_clock::now();
#pragma omp parallel num_threads(2)
   {
      for (int k = 0; k < V; k++)
         for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
               A[i][j] = A[i][j] || (A[i][k] && A[k][j]);
   }
   end_time = system_clock::now();
   cout << "Time parallel" << ' ' << (end_time - start_time).count() << " " << endl;
   for (int i = 0; i < V; i++)
   {
      for (int j = 0; j < V; j++)
         cout << A[i][j] << ' ';
      cout << endl;
      delete[] A[i];
   }
   delete[] A;
}

int normal()
{
   ifstream input;
   input.open("input.txt");
   int V, E;
   input >> V >> E;
   if (!input)
   {
      return 0;
   }
   int **A = new int *[V];
   for (int i = 0; i < V; i++)
   {
      A[i] = new int[V];
      for (int j = 0; j < V; j++)
         A[i][j] = 0;
   }
   int i, j;
   for (int k = 0; k < E; k++)
   {
      input >> i >> j;
      A[i - 1][j - 1] = 1;
      A[j - 1][i - 1] = 1;
   }

   time_point<system_clock> start_time, end_time;
   start_time = system_clock::now();
   for (int k = 0; k < V; k++)
      for (int i = 0; i < V; i++)
         for (int j = 0; j < V; j++)
            A[i][j] = A[i][j] || (A[i][k] && A[k][j]);
   end_time = system_clock::now();
   cout << "Time normal"<< ' ' << (end_time - start_time).count() << " " << endl;

   for (int i = 0; i < V; i++)
   {
      for (int j = 0; j < V; j++)
         cout << A[i][j] << ' ';
      cout << endl;
      delete[] A[i];
   }
   cout << endl;
   delete[] A;
}

int main()
{
   
   parallel();
   normal();
   system("pause");
}