////������� 1
#include <iostream>
#include <stack>
#include <Windows.h>

using namespace std;


int main()
{
   int a = 1;  // ��� ����� �����
   stack<int> A;  // ����� �������������� 0(����)
   cout << "Enter the numbers: ";
   while (a != 0)
   {
      cin >> a;
      A.push(a);
   }

   while (!A.empty())
   {
      cout << A.top() << ' ';
      A.pop();
   }
  
   system("pause");
}

////������� 2

//#include <iostream>
//#include <vector>
//#include <Windows.h>
//using namespace std;
//
//int main()
//{
//   int a = 1;  // ��� ����� �����
//   vector<int> A;  // ����� �������������� 0(����)
//   cout << "Enter the numbers: ";
//   while (a != 0)
//   {
//      cin >> a;
//      A.push_back(a);
//   }
//
//   sort(A.begin(), A.end());
//
//   while (!A.empty())
//   {
//      cout << A.pop() << ' ';
//      A.pop();
//   }
//  
//   system("pause");
//}