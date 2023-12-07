//задание 1
#include <iostream>
#include <stack>
#include <Windows.h>

using namespace std;


int main()
{
   int a = 1;  // для ввода чисел
   stack<int> A;  // числа оканчивающиеся 0(стэк)
   cout << "Enter the numbers: ";
   while (a != 0)
   {
      cin >> a;
      A.push(a);
   }

   while (!A.empty())
   {
      cout << A.pop() << ' ';
   }

   system("pause");
}