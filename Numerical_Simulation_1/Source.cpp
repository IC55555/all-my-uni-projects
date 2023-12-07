#include<iostream>
#include<vector>

using namespace std;
const double length = 1;

double f(double t, double y)
{
   double f = 4 * t * y;
   return f;
}

vector<double> euler1(double h)
{

   int N = round(length / h);
   vector<double> y;
   vector<double> t;
   for (int i = 0; i < N+1; i++)
   {
      t.push_back(i * h);
   }

   y.push_back(1);

   for (int i = 0; i < N; i++)
   {
      y.push_back(y[i] + h * f(t[i], y[i]));
   }

   return y;
}
   

vector<double> euler2(double h)
{

   int N = round(length / h);
   vector<double> y;
   vector<double> t;
   for (int i = 0; i < N+1; i++)
   {
      t.push_back(i * h);
      /*cout << t[i] << " ";*/
   }

   y.push_back(1);


   for (int i = 0; i < N; i++)
   {
      y.push_back(y[i] + h / 2.0 * (f(t[i], y[i]) + f(t[i + 1], y[i] + h * f(t[i], y[i]))));
   }

   return y;
}


vector<double> euler3(double h)
{
   int N = round(length / h);
   vector<double> y;
   vector<double> t; 
   for (int i = 0; i < N + 1; i++)
   {
      t.push_back(i * h);
      //cout << t[i] << " ";
   }

   y.push_back(1);

   for (int i = 0; i < N; i++)
   {
      y.push_back(y[i] + h * f(t[i] + h / 2.0, y[i] + h / 2.0 * f(t[i], y[i])));
   }

   return y;
}


int main()
{
   vector<double> y0_1;
   vector<double> y0_05;
   vector<double> y0_025;
   y0_1 = euler1(0.025);
   y0_05 = euler2(0.025);
   y0_025 = euler3(0.025);


   for (double i = 0; i < y0_1.size(); i++)
   {
      cout << y0_1[i] << " ";
   }

   cout << endl << endl;

   for (double i = 0; i < y0_05.size(); i++)
   {
      cout << y0_05[i] << " ";
   }

   cout << endl << endl;

   for (double i = 0; i < y0_025.size(); i++)
   {
      cout << y0_025[i] << " ";
   }

   cout << endl;
   system("pause");
}