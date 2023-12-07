#include <iostream>
#include <vector>
#include <conio.h>

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
   double g = -25 * y + cos(t) + 25 * sin(t);
   return g;
}

vector<double> Adams_Bashfort_3(vector<double> &y) //explicit_euler_method
{

   int N = round(length / h);
   vector<double> t;
   for (int i = 0; i < N + 1; i++)
   {
      t.push_back(i * h);
   }

   for (int i = 0; i < y.size()-3; i++)
   { 
      y[i + 3] = y[i + 2] + h * (23.0 / 12.0 * f(t[i + 2], y[i + 2]) - 4.0 / 3.0 * f(t[i + 1], y[i + 1]) + 5.0 / 12.0 * f(t[i], y[i]));
   }
   return y;
}

vector<double> Adams_Bashfort_4(vector<double> &y) //explicit_euler_method
{

   int N = round(length / h);
   vector<double> t;
   for (int i = 0; i < N + 1; i++)
   {
      t.push_back(i * h);
   }

   for (int i = 0; i < y.size()-4; i++)
   {
      y[i + 4] = y[i + 3] + h * (55.0 / 24.0 * f(t[i + 3], y[i + 3]) - 59.0 / 24.0 * f(t[i + 2], y[i + 2]) + 37.0 / 24.0 * f(t[i + 1], y[i + 1]) - 3.0 / 8.0 * f(t[i], y[i]));
   }
   return y;
}

vector<double> Adams_Multon_3(vector<double> &u) //implicit_euler_method
{
   double y = 1, y1, x;
   int N = round(length / h);
   vector<double> Y;
   Y.push_back(1);
   double eps = 1e-14;
   vector<double> t;
   for (int i = 0; i < N + 1; i++)
   {
      t.push_back(i * h);
   }
   for (int i = 0; i < u.size() -3; i++)
   {
      //x = y + h * g(t, y);
      u[i + 3] = u[i + 2] + h * (3.0 / 8.0 * f(t[i + 2], u[i + 2]) + 19.0 / 24.0 * f(t[i + 2], u[i + 2]) - 5.0 / 24.0 * f(t[i + 1], u[i + 1]) - 1.0 / 24.0 * f(t[i], u[i]));
      //t = i * h;
      //y1 = y + h * g(t, x);
      y1 = u[i + 2] + h * (3.0 / 8.0 * f(t[i + 3], u[i + 3]) + 19.0 / 24.0 * f(t[i + 2], u[i + 2]) - 5.0 / 24.0 * f(t[i + 1], u[i + 1]) - 1.0 / 24.0 * f(t[i], u[i]));
      while (abs(y1 - u[i + 3]) > eps)
      {
         u[i + 3] = y1;
         y1 = u[i + 3] - (y + h * f(t[i+3], u[i + 3]) - u[i + 3]) / (-25 * h - 1);
      }
      u[i+3] = y1;
      //Y.push_back(y);
   }
   return u;
}


vector<double> Adams_Multon_4(vector<double> &u) //implicit_euler_method
{

   double y = 1, y1, x;
   int N = round(length / h);
   vector<double> Y;
   Y.push_back(1);
   double eps = 1e-14;
   vector<double> t;
   for (int i = 0; i < N + 1; i++)
   {
      t.push_back(i * h);
   }
   for (int i = 0; i < u.size() - 4; i++)
   {
      
      u[i + 4] = u[i + 3] + h* (251.0/ 720.0 * f(t[i + 3], u[i + 3])+ 646.0/720.0* f(t[i + 3], u[i + 3]) - 264.0/720.0 * f(t[i + 2], u[i + 2]) + 106.0/720.0 * (t[i + 1], u[i + 1]) - 19.0/720.0* f(t[i], u[i]));
      y1 = u[i + 3] + h * (251.0 / 720.0 * f(t[i + 4], u[i + 4]) + 646.0 / 720.0 * f(t[i + 3], u[i + 3]) - 264.0 / 720.0 * f(t[i + 2], u[i + 2]) + 106.0 / 720.0 * (t[i + 1], u[i + 1]) - 19.0 / 720.0 * f(t[i], u[i]));
      while (abs(y1 - u[i + 4]) > eps)
      {
         u[i + 4] = y1;
         y1 = u[i + 4] - (y + h * f(t[i + 4], u[i + 4]) - u[i + 4]) / (-25 * h - 1);
      }
      u[i + 4] = y1;
      //Y.push_back(y);
   }
   return u;
}

char menu()
{
   cout << "Select the functions from 1 to 2" << endl;
   cout << "1 is Runge-kutta(4ty)" << endl << "2 is explicit_euler_method" << endl << "3 is implicit_euler_method" << endl << "4 is modified euler" << endl << "5 is Trapezoid" << endl << "6 is Runge-kutta(-25*y+cos(t)+sin(t))" << endl;
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
  /* switch (choice)
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
   }*/

   for (double i = 0; i < y.size(); i++)
   {
      cout << y[i] << "\n";
   }
   system("pause");
}