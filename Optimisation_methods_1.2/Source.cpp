#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;
const double eps = 1e-4;
const double delta = 1e-9;

double f(double x)
{
   return (x - 7) * (x - 7);
}

int Dichotomy(double a, double b)
{
   ofstream fout;
   fout.open("out.txt");
   fout.imbue(locale("Russian"));
   fout << setprecision(15);
   int iter = 0;
   fout << "\n" << "i" << '\t' << "x1" << '\t' << "x2" <<
      '\t' << "f(x1)" << '\t' << "f(x2)" << '\t' << "a" << '\t' << "b";
   //fout << "\n" << setw(2) << "i" << setw(15) << "x1" << setw(15) << "x2" <<
   //    setw(15) << "f(x1)" << setw(15) << "f(x2)" << setw(15) << "a" << setw(15) << "b";
   while (abs(b - a) > eps)
   {
      double x1 = (a + b - delta) / 2;
      double x2 = (a + b + delta) / 2;

      if (f(x1) < f(x2))
         b = x2;
      else
         a = x1;

      iter++;
      //fout << "\n" << setw(2) << iter << setw(15) << x1 << setw(15) << x2 <<
          //setw(15) << f(x1) << setw(15) << f(x2) << setw(15) << a << setw(15) << b;
      fout << "\n" << '\t' << iter << '\t' << x1 << '\t' << x2 <<
         '\t' << f(x1) << '\t' << f(x2) << '\t' << a << '\t' << b;
   }
   fout.close();
   return iter;
}

int Golden_ratio(double a, double b)
{
   ofstream fout;
   fout.open("out.txt");
   fout.imbue(locale("Russian"));
   int iter = 0;
   fout << setprecision(15);

   double x1 = a + 0.381966011 * (b - a);
   double x2 = b - 0.381966011 * (b - a);
   double f1 = f(x1);
   double f2 = f(x2);

   fout << "\n" << setw(2) << "i" << setw(15) << "x1" << setw(15) << "x2" <<
      setw(15) << "f(x1)" << setw(15) << "f(x2)" << setw(15) << "a" << setw(15) << "b";

   while (abs(b - a) > eps)
   {
      if (f(x1) < f(x2))
      {
         b = x2;
         x2 = x1;
         f2 = f1;
         x1 = a + 0.381966011 * (b - a);
         f1 = f(x1);
      }
      else
      {
         a = x1;
         x1 = x2;
         f1 = f2;
         x2 = b - 0.381966011 * (b - a);
         f2 = f(x2);
      }

      iter++;
      //fout << "\n" << setw(2) << iter << setw(15) << x1 << setw(15) << x2 <<
       //   setw(15) << f(x1) << setw(15) << f(x2) << setw(15) << a << setw(15) << b;
      fout << "\n" << '\t' << iter << '\t' << x1 << '\t' << x2 <<
         '\t' << f(x1) << '\t' << f(x2) << '\t' << a << '\t' << b;
   }
   fout.close();
   return iter;
}

int Search_Interval_With_Minimum(double xprev)
{
   ofstream fout;
   fout.open("out.txt");
   fout.imbue(locale("Russian"));

   int iter = 0;
   double a, b;
   double xnext, xcurr = 0;
   double h = 0;

   fout << "\n" << setw(2) << "i" << setw(15) << "xi" << setw(15) << "f(xi)";
   if (f(xprev) > f(xprev + delta))
   {
      xcurr = xprev + delta;
      h = delta;
   }
   else if (f(xprev) > f(xprev - delta))
   {
      xcurr = xprev - delta;
      h = -delta;
   }

   while (true)
   {
      h *= 2;
      xnext = xcurr + h;

      if (f(xcurr) > f(xnext))
      {
         xprev = xcurr;
         xcurr = xnext;
         iter++;
      }
      else
      {
         a = xprev;
         b = xnext;
         if (a > b)
            swap(a, b);
         break;
      }
      fout << "\n" << '\t' << iter << '\t' << xcurr << '\t' << f(xcurr);
      /*fout << "\n" << setw(2) << iter << setw(15) << xcurr << setw(15) << f(xcurr);*/
   }
   /*fout << "\n" << setw(2) << iter + 1 << setw(15) << xnext << setw(15) << f(xnext);
   fout << '\n' << "Интервал, содержащий минимум: " << '[' << a << ", " << b << ']';*/
   fout << "\n" << '\t' << iter + 1 << '\t' << xnext << '\t' << f(xnext);
   fout << '\n' << "Интервал, содержащий минимум: " << '[' << a << ", " << b << ']';
   fout.close();
   return iter;
}

int Fibonacci(double a, double b)
{
   fstream fout;
   fout.open("out.txt");
   fout.imbue(locale("Russian"));
   fout.precision(9);

   double x1, x2, f1, f2;
   double F_max = (b - a) / eps;

   int temp_fib;
   int n = 2;

   int type_p;

   vector<long long int> fib_num;
   fib_num.reserve(50);
   fib_num.push_back(1);
   fib_num.push_back(1);

   do 
   {
      temp_fib = fib_num[n - 1] + fib_num[n - 2];
      fib_num.push_back(temp_fib);
      n++;
   } while (F_max > temp_fib);

   n -= 3;
   int iter = 1;

   x1 = a + fib_num[n] * (b - a) / fib_num[n + 2];
   x2 = a + b - x1;
   f1 = f(x1);
   f2 = f(x2);

   if (f1 < f2)
   {
      b = x2;
      x2 = x1;
      f2 = f1;
      type_p = 1;
   }
   else
   {
      a = x1;
      x1 = x2;
      f1 = f2;
      type_p = 2;
   }

   fout << "\n" << '\t' << iter << '\t' << x1 << '\t' << x2 <<
      '\t' << f(x1) << '\t' << f(x2) << '\t' << a << '\t' << b;

   for (int k = 1; k < n; k++)
   {
      iter++;
      switch (type_p)
      {
      case 1:
         x1 = a + fib_num[n - k + 1] * (b - a) / fib_num[n - k + 3];
         f1 = f(x1);
         break;
      case 2:
         x2 = a + fib_num[n - k + 2] * (b - a) / fib_num[n - k + 3];
         f2 = f(x2);
         break;
      };

      if (f1 < f2)
      {
         b = x2;
         x2 = x1;
         f2 = f1;
         type_p = 1;
      }
      else
      {
         a = x1;
         x1 = x2;
         f1 = f2;
         type_p = 2;
      }
      fout << "\n" << '\t' << iter << '\t' << x1 << '\t' << x2 <<
         '\t' << f(x1) << '\t' << f(x2) << '\t' << a << '\t' << b;
   }

   return iter;
}

int main()
{
   double a = -2, b = 20;
   //Golden_ratio(a, b);

   Dichotomy(a, b);
   //Fibonacci(a, b);
   //Search_Interval_With_Minimum(4);
   return 0;
}
