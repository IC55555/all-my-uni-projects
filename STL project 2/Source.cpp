#include <iostream>
#include <vector>
#include <Windows.h>
#include <algorithm>
using namespace std;

int main()
{
   int a = 1;  // ��� ����� �����
   vector<int> A;  // ����� �������������� 0(����)
   cout << "Enter the numbers: ";
   while (a != 0)
   {
      cin >> a;
      A.push_back(a);
   }

   sort(A.begin(), A.end()); //���������� �� ���������(����������)

   for (int i = 0; i < A.size(); i++) //����� �� ����� ���������������� �������
   {
      cout << A[i] << ' ';
   }
   cout << endl;

   a = A[0];
   for (int i = 1; i < A.size(); i++) // �������� ������������� ���������
   {
      if (A[i] == a)
      {
         A.erase(A.begin() + i);
         i--;
      }
      else
         a = A[i];
   }

   for (int i = 0; i < A.size(); i++) //����� �� ����� ��������� ������
   {
      cout << A[i] << ' ';
   }
   cout << endl;
      
   system("pause");
}