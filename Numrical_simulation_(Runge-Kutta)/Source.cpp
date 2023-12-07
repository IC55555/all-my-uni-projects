#include <iostream>
#include <vector>
#include <conio.h>
#include <iomanip>

using namespace std;
double length = 1;
double h;

double f(double t, double y)
{
   double f = 4 * t * y;
   return f;
}

double g(double t, double y)
{
   double g = -25*y+cos(t)+25*sin(t);
   return g;
}

vector<double> explicit_euler_method()
{

   int N = round(length / h);
   vector<double> y;
   vector<double> t;
   for (int i = 0; i < N + 1; i++)
   {
      t.push_back(i * h);
   }

   y.push_back(1);

   for (int i = 0; i < N; i++)
   {
      y.push_back(y[i] + h * g(t[i], y[i]));
   }

   return y;
}


vector<double> implicit_euler_method() ///1 явный, 2 неявный *Trapezoid
{
   
   double t = 0, y = 1, y1, x;
   int N = round(length / h);
   vector<double> Y;
   Y.push_back(1);
   double eps = 1e-14;
   for (int i = 1; i <= N; i++)
   {
      x = y + h * g(t, y);
      t = i * h;
      y1 = y + h * g(t, x);
      while (abs(y1 - x) > eps)
      {
         x = y1;
         y1 = x - ( y + h * g(t, x)-x)/(-25*h-1);
      }
      y = y1;
      Y.push_back(y);
   }
   return Y;
}

vector<double> Trapezoid() // modifiede_euler
{
   //Метод трапеции 
   double t = 0, y = 1, y1, x;
   int N = round(length / h);
   vector<double> Y;
   Y.push_back(1);
   double eps = 1e-14;

   for (int i = 1; i <= N; i++)
   {
      x = y + h * g(t, y);
      y1 = y + (h / 2) * (g(t, y) + g(i * h, x));
      while (abs(y1 - x) > eps)
      {
         x = y1;
         y1 = x - ( y + (h / 2) * (g(t, y) + g(i * h, x))-x)/(-25*h-1);
      }
      y = y1;
      t = i * h;
      Y.push_back(y);
   }
   return Y;
}

vector<double> Runge_Kutta1()
{
   double t = 0;
   double k, k1, k2, k3, k4;
   int N = round(length / h);
   vector<double> y;
   y.push_back(1);
   for (int i = 0; i < N; i++)
   {
      k1 = f(t, y[i]);
      k2 = f(t + h / 2.0, y[i] + k1 * (h / 2.0));
      k3 = f(t + h / 2.0, y[i] + k2 * (h / 2.0));
      k4 = f(t + h, y[i] + h * k3);
      k = 1.0 / 6.0 * (k1 + 2 * k2 + 2 * k3 + k4);
      y.push_back(y[i] + h * k);
      t += h;
   }
   return y;
}

vector<double> modified_euler_method()
{
   double t = 0;
   vector<double> y;
   int N = round(length / h);
   y.push_back(1);
   double temp;
   for (int i = 0; i < N; i++)
   {
      temp = y[i] + h * g(t, y[i]);
      y.push_back(y[i] + h / 2.0 * (g(t, y[i]) +g(t+h, temp)));
      t += h;
   }
   return y;
}


vector<double> Runge_Kutta2()
{
   double t = 0;
   double k, k1, k2, k3, k4;
   int N = round(length / h);
   vector<double> y;
   y.push_back(1);
   for (int i = 0; i < N; i++)
   {
      k1 = g(t, y[i]);
      k2 = g(t + h / 2.0, y[i] + k1 * (h / 2.0));
      k3 = g(t + h / 2.0, y[i] + k2 * (h / 2.0));
      k4 = g(t + h, y[i] + h * k3);
      k = 1.0 / 6.0 * (k1 + 2 * k2 + 2 * k3 + k4);
      y.push_back(y[i] + h * k);
      t += h;
   }
   return y;
}

char menu()
{
   cout << "Select the functions from 1 to 6" << endl;
   cout << "1 is Runge-kutta(4ty)" << endl << "2 is explicit_euler_method" << endl << "3 is implicit_euler_method" << endl << "4 is modified euler" << endl << "5 is Trapezoid" << endl << "6 is Runge-kutta(-25*y+25*cos(t)+25*sin(t))" << endl;
   char choice = _getch();
   cout << endl;
   return choice;
}


int main()
{
   cout << "h = ";
   cin >> h;
   cout << "Length = ";
   cin >> length;
   char choice = menu();
   vector<double> y;
   switch (choice)
   {
   case '1':
      y = Runge_Kutta1();
      break;
   case '2':
      y = explicit_euler_method();
      break;
   case '3':
      y = implicit_euler_method();
      break;
   case '4':
      y = modified_euler_method();
      break;
   case '5':
      y = Trapezoid();
      break;
   case '6':
      y = Runge_Kutta2();
      break;
   default:
      cout << "Wrong key";
      break;
   }

   for (double i = 0; i < y.size(); i++)
   {
      cout << setprecision(6) << y[i] << "\n";
   }
   system("pause");
}