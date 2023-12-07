#include <iostream>
#include <vector>
#include <Windows.h>
#include <algorithm>
using namespace std;

int main()
{
   int N;  // количество динамических массивов
   cout << "Enter the number of vectors: ";
   cin >> N;
   int a = 1, b = 1;  // для ввода чисел
   vector<int> *A = new vector<int>[N];
   cout << "Enter the pairs of numbers: ";
   while (true)
   {
      cin >> a; //номер массива
      cin >> b; // то что хотим записать в массив с номмером "а"
      if ((a == 0) && (b == 0))
         break;
      A[a - 1].push_back(b);
      
   }

   for (int i = 0; i < N; i++)
   {
      sort(A[i].begin(), A[i].end());
      for (int j = 0; j < A[i].size(); j++) //вывод на экран отсортированного вектора
      {
         cout << A[i][j] << ' ';
      }
      cout << endl;
   }
   delete[] A;
   system("pause");
}