#include<iostream>
#include<vector>

using namespace std;
const double length = 2;

double f(double t, double y)
{
   double f = -25*y+cos(t)+25*sin(t);
   return f;
}

vector<double> N_euler(double h)
{
   double t = 0, y = 1, y1, x;
   int N = round(length / h);
   vector<double> Y;
   Y.push_back(1);
   double eps = 1E-14;
   
   // Простой неявный метод Эйлера
   for (int i = 1; i <= N; i++)
   {
      x = y + h * f(t, y);
      t = i * h;
      y1 = y + h * f(t, x);
     /* int o = 0;*/
      while (abs(y1 - x) > eps)
      {
         //cout << abs(y1 - x) << endl;
         x = y1;
         y1 = y + h * f(t, x);
         /*o++;*/
      }
      y = y1;
      Y.push_back(y);
   }
   return Y;
}

//vector<double> Trapezoid(double h)
//{
//   //Метод трапеции 
//   double t = 0, y = 1, y1, x;
//   int N = round(length / h);
//   vector<double> Y;
//   Y.push_back(1);
//   double eps = 1E-14;
//
//   for (int i = 1; i <= N; i++)
//   {
//      x = y + h * f(t, y);
//      y1 = y + (h / 2) * (f(t, y) + f(i * h, x));
//      while (abs(y1 - x) > eps)
//      {
//         x = y1;
//         y1 = y + (h / 2) * (f(t, y) + f(i * h, x));
//      }
//      y = y1;
//      t = i * h;
//      Y.push_back(y);
//   }
//   return Y;
//}

int main()
{
   vector<double> y0_1;
   vector<double> y0_05;
   y0_1 = N_euler(0.025);
   /*y0_05 = Trapezoid(0.025);*/


   for (double i = 0; i < y0_1.size(); i++)
   {
      cout << y0_1[i] << " \n";
   }

   cout << endl << endl;

   for (double i = 0; i < y0_05.size(); i++)
   {
      cout << y0_05[i] << " ";
   }

   cout << endl;
   system("pause");
}