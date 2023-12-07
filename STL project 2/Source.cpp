#include <iostream>
#include <vector>
#include <Windows.h>
#include <algorithm>
using namespace std;

int main()
{
   int a = 1;  // дл€ ввода чисел
   vector<int> A;  // числа оканчивающиес€ 0(стэк)
   cout << "Enter the numbers: ";
   while (a != 0)
   {
      cin >> a;
      A.push_back(a);
   }

   sort(A.begin(), A.end()); //сортировка по умолчанию(возрстание)

   for (int i = 0; i < A.size(); i++) //вывод на экран отсортированного вектора
   {
      cout << A[i] << ' ';
   }
   cout << endl;

   a = A[0];
   for (int i = 1; i < A.size(); i++) // удаление повтор€ющихс€ элементов
   {
      if (A[i] == a)
      {
         A.erase(A.begin() + i);
         i--;
      }
      else
         a = A[i];
   }

   for (int i = 0; i < A.size(); i++) //вывод на экран финальной версии
   {
      cout << A[i] << ' ';
   }
   cout << endl;
      
   system("pause");
}