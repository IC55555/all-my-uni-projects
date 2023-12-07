#include "Pirson3Method.h"
#include <conio.h>

int ITER = 0;
double Pirson3Method::func(vector<double> &x, char key)
{
   ITER++;
   cout << ITER << endl;
   
   double A1 = 2, A2 = 3, a1 = 1, b1 = 2, c1 = 1, d1 = 2, a2 = 1, b2 = 3, c2 = 3, d2 = 3;
   switch (key)
   {
      case '1':
         // квадратичная функция; ответ (1; 1)
         return 100 * (x[1] - x[0]) * (x[1] - x[0]) + (1 - x[0]) * (1 - x[0]);
         break;
      case '2':
         // функция Розенброка; ответ (1; 1)
         return 100 * (x[1] - x[0] * x[0]) * (x[1] - x[0] * x[0]) + (1 - x[0]) * (1 - x[0]);
         break;
      case '3':
         // заданная f(x)
         // т.к. максимизируем f(x), то минимизировать будем -f(x)
         // тут задано -f(x) нечетных вариантов
         // константы варианта 7
         // ответ
         return -(A1 * exp(-((x[0] - a1) / b1) * ((x[0] - a1) / b1) - ((x[1] - c1) / d1) * ((x[1] - c1) / d1)) + A2 * exp(-((x[0] - a2) / b2) * ((x[0] - a2) / b2) - ((x[1] - c2) / d2) * ((x[1] - c2) / d2)));
         break;
      case '4':
         // простейшая функция; ответ (7; 1)
         return (x[0] - 7) * (x[0] - 7) + (x[1] - 1) * (x[1] - 1);
         break;
      default:
         return 0;
         break;
   }
}

double Pirson3Method::f(double lambda)
{
   vector<double> temp = x;
   for (int i = 0; i < N; i++)
   {
      temp[i] -= lambda * mult[i];
   }
   return func(temp, key);
}

int Pirson3Method::d_func(vector<double> &x, vector<double> &grad_f)
{
   double A1 = 2, A2 = 3, a1 = 1, b1 = 2, c1 = 1, d1 = 2, a2 = 1, b2 = 3, c2 = 3, d2 = 3;

   switch (key)
   {
       case '1':
          // градиент квадратичной
          grad_f[0] = -200 * (x[1] - x[0]) - 2 * (1 - x[0]);
          grad_f[1] = 200 * (x[1] - x[0]);
          break;
       case '2':
          // градиент Розенброка
          grad_f[0] = -2 * x[0] * 200 * (x[1] - x[0] * x[0]) - 2 * (1 - x[0]);
          grad_f[1] = 200 * (x[1] - x[0] * x[0]);
          break;
       case '3':
          // градиент -f(x) нечетных вариантов   
          grad_f[0] = -(A1 * (-2 * ((x[0] - a1) / b1)) * exp(-((x[0] - a1) / b1) * ((x[0] - a1) / b1) - ((x[1] - c1) / d1) * ((x[1] - c1) / d1)) + A2 * (-2 * ((x[0] - a2) / b2)) *  exp(-((x[0] - a2) / b2) * ((x[0] - a2) / b2) - ((x[1] - c2) / d2) * ((x[1] - c2) / d2)));
          grad_f[1] = -(A1 * (-2 * ((x[1] - c1) / d1)) * exp(-((x[0] - a1) / b1) * ((x[0] - a1) / b1) - ((x[1] - c1) / d1) * ((x[1] - c1) / d1)) + A2 * (-2 * ((x[1] - c2) / d2)) * exp(-((x[0] - a2) / b2) * ((x[0] - a2) / b2) - ((x[1] - c2) / d2) * ((x[1] - c2) / d2)));
          break;
       case '4':
          // градиент простейшей
          grad_f[0] = 2 * (x[0] - 7);
          grad_f[1] = 2 * (x[1] - 1);
          break;
       default:
          break;

   }
   // численная производная
   /*double h = 1E-7;
   vector<double> temp = x;
   for (int i = 0; i < N; i++)
   {
       temp[i] += h;
       grad_f[i] = (func(temp) - func(x)) / h;
       temp[i] -= h;
   }*/
   return 0;
}

int Pirson3Method::Algorithm(vector<double> &x0)
{
   directionMatrix.resize(N);
   for (int i = 0; i < N; i++)
   {
      directionMatrix[i].resize(N);
      directionMatrix[i][i] = 1;
   }
   x.resize(N);
   xNew.resize(N);
   mult.resize(N);
   x = x0;
   vector<double> grad_f1(N), grad_f2(N);
   double stop = 10;
   d_func(x, grad_f1);

   ofstream out("out.txt");
   out.precision(4);
   out.imbue(locale("Russian"));
   int iter;
   for (iter = 0; iter < MAXITER && stop > EPS; iter++)
   {
      // calculate ню(x(k)) * grad f(x(k))
      for (int i = 0; i < N; i++)
      {
         double sum = 0;
         for (int j = 0; j < N; j++)
         {
            sum += directionMatrix[i][j] * grad_f1[j];
         }
         mult[i] = sum;
      }

      // find lambda
      double a, b;
      Search_Interval_With_Minimum(0, &a, &b);
      //lambda = Fibonacci(a, b);
      lambda = parabola_method(a, b);

      // -------------------------------------------
      // get x(k+1)
      // -------------------------------------------
      for (int i = 0; i < N; i++)
      {
         xNew[i] = x[i] - lambda * mult[i];
      }
      d_func(xNew, grad_f2);
      ////////////////////////////////

      /*out << iter << "\t"
          << func(xNew) << "\t"
          << xNew[0] << "\t" << xNew[1] << "\t"
          << mult[0] << "\t" << mult[1] << "\t"
          << lambda << "\t"
          << abs(x[0] - xNew[0]) << "\t" << abs(x[1] - xNew[1]) << "\t"
          << abs(func(x) - func(xNew)) << "\t"
          << acos((xNew[0] * mult[0] + xNew[1] * mult[1]) / (sqrt(xNew[0] * xNew[0] + xNew[1] * xNew[1]) * sqrt(mult[0] * mult[0] + mult[1] * mult[1]))) << "\t"
          << grad_f2[0] << "\t" << grad_f2[1] << endl;*/
          ////////////////////////////////

          // критерий останова
          /*stop = 0;
          for (int i = 0; i < N; i++)
          {
              stop += grad_f1[i] * grad_f1[i];
          }
          stop = sqrt(stop);*/
      stop = 0;
      for (int i = 0; i < N; i++)
      {
         stop += (x[i] - xNew[i]) * (x[i] - xNew[i]);
      }
      stop = sqrt(stop);

      // -------------------------------------------
      // modify direction matrix
      // -------------------------------------------
      // обновление матрицы
      if (iter % 3 == 0)
      {
         for (int i = 0; i < N; i++)
         {
            directionMatrix[i].resize(N);
            directionMatrix[i][i] = 1;
         }
      }
      else
      {
         // x = x(k+1) - x(k)
         // grad_f1 = grad f(x(k+1)) - grad f(x(k))

         for (int i = 0; i < N; i++)
         {
            x[i] = xNew[i] - x[i];
            grad_f1[i] = grad_f2[i] - grad_f1[i];
         }

         // mult = ню * (grad f(x(k+1)) - grad f(x(k)))
         // temp = знаменатель
         double temp = 0;
         for (int i = 0; i < N; i++)
         {
            double sum = 0;
            for (int j = 0; j < N; j++)
            {
               sum += directionMatrix[i][j] * grad_f1[i];
            }
            mult[i] = sum;
            temp += sum * grad_f1[i];
         }
         for (int i = 0; i < N; i++)
         {
            for (int j = 0; j < N; j++)
            {
               directionMatrix[i][j] += (mult[i] * (x[i] - mult[i])) / temp;
            }
         }
      }
      // -------------------------------
      // make xNew as x
      // -------------------------------
      x.swap(xNew);
      grad_f1.swap(grad_f2);
   }
   x0 = x;

   return iter;
}

int Pirson3Method::Search_Interval_With_Minimum(double xprev, double *res_a, double *res_b)
{
   int iter = 0;
   double a, b;
   double xnext, xcurr = 0;
   double h = 0;

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
   else
   {
      a = xprev + delta;
      b = xprev - delta;
      *res_a = a;
      *res_b = b;
      return iter;
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
   }
   *res_a = a;
   *res_b = b;
   return iter;
}

double Pirson3Method::parabola_method(double a, double b)
{
   double x = (a + b) / 2,
      h = 0.001,
      fx = f(x),
      fxk,
      fxh = f(x + h),
      fhx = f(x - h),
      xk, tau;
   int i = 0;
   do
   {
      xk = x - h / 2 * (fxh - fhx) / (fxh - 2 * fx + fhx);
      tau = 0.5;
      if (fxh - 2 * fx + fhx <= 0)
         h = -3 * h;
      else
      {
         fxk = f(xk);
         if (fxk >= fx)
            h = tau * (xk - x);
         else
         {
            h = abs(x - xk) / 2;
            x = xk;
            fx = fxk;
            fxh = f(x + h);
            fhx = f(x - h);
         }
      }
      i++;
   } while (h > 2 * eps_find);
   return x;
}

double Pirson3Method::Fibonacci(double a, double b)
{
   double x1, x2, f1, f2;
   double F_max = (b - a) / eps_find;

   int temp_fib;
   int n;

   int type_p;

   vector<long long int> fib_num;
   fib_num.reserve(50);
   fib_num.push_back(1);
   fib_num.push_back(1);

   n = 2;

   do {
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

   for (int k = 1; k < n; k++)
   {
      iter++;
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
      switch (type_p)
      {
      case 1:
         x1 = a + fib_num[n - k] * (b - a) / fib_num[n - k + 2];
         f1 = f(x1);
         break;
      case 2:
         x2 = a + fib_num[n - k + 1] * (b - a) / fib_num[n - k + 2];
         f2 = f(x2);
         break;
      };
   }
   iter++;
   if (f1 < f2)
   {
      b = x2;
      f2 = f1;
      type_p = 1;
   }
   else
   {
      a = x1;
      f1 = f2;
      type_p = 2;
   }
   x1 = a + fib_num[0] * (b - a) / fib_num[2];
   x2 = x1;

   return x1;
}