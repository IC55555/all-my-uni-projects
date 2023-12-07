#include <iostream>
#include <fstream>
#include <locale>

using namespace std;

class comma : public numpunct<char>
{
public:
   comma() : numpunct<char>() {}
protected:
   char do_decimal_point() const
   {
      return ',';
   }
};

double sigma0;
double sigma_cur;
double sigma_true;
double I = 0.9;

// Приёмники
double M[3], N[3];

// Источники
double A, B;

// Синтетические данные
double Vsint[3]; // V1 M1N1, V2 M2N2, V3 M3N3

// Потенциалы
double V[3];

// Производная потенциалов по силе тока
double dVdsigma[3];

double PI = 3.1415926535;

double r(double m1, double m2) // расстояние между двумя точками
{
   return abs(m1 - m2);
}

void recalculationOfPotentials()
{
   for (int i = 0; i < 3; i++)
      V[i] = I / (2 * PI * sigma_cur) * ((1 / r(B, M[i]) - 1 / r(A, M[i])) - (1 / r(B, N[i]) - 1 / r(A, N[i])));
}

void recalculationOfDerivativesOfPotentials()
{
   for (int i = 0; i < 3; i++)
      dVdsigma[i] = -1 / (2 * PI * sigma_cur * sigma_cur) * ((1 / r(B, M[i]) - 1 / r(A, M[i])) - (1 / r(B, N[i]) - 1 / r(A, N[i])));
}

void init(double par_sigma0, double par_sigma_true, double param)
{
   M[0] = 200, N[0] = 300, M[1] = 500, N[1] = 600, M[2] = 1000, N[2] = 1100;
   A = 0, B = 100;

   sigma0 = par_sigma0;
   sigma_true = par_sigma_true;

   for (int i = 0; i < 3; i++)
      Vsint[i] = I / (2 * PI * sigma_true) * ((1 / r(B, M[i]) - 1 / r(A, M[i])) - (1 / r(B, N[i]) - 1 / r(A, N[i])));

   Vsint[0] += param * (Vsint[0]);
   Vsint[1] += param * (Vsint[1]);
   Vsint[2] += param * (Vsint[2]);
}


void output(double param)
{
   ofstream out;
   out.open("out.csv", ios::app);
   locale loccomma(cout.getloc(), new comma);
   out.imbue(loccomma);
   init(0.01, 0.1, param);
   sigma_cur = sigma0;
   double eps = 1E-7;
   double a11, b1;
   double w[3];
   for (int i = 0; i < 3; i++)
      w[i] = 1 / Vsint[i];

   double deltaI = 0;
   int k = 0;
   recalculationOfPotentials();
   double F = 0;
   for (int i = 0; i < 3; i++)
      F += (w[i] * (V[i] - Vsint[i])) * (w[i] * (V[i] - Vsint[i]));
   out << "Iter num" << ';' << "cur sigma" << ';' << "Func" << ';' << "param = " << param << "\n";
   out << k << ';' << sigma_cur << ';' << F << "\n";
   cout << k << '\t' << sigma_cur << '\t' << F << "\n";
   k++;
   recalculationOfDerivativesOfPotentials();
   while (abs(F) > eps)
   {
      a11 = 0;
      for (int i = 0; i < 3; i++)
         a11 += (w[i] * dVdsigma[i]) * (w[i] * dVdsigma[i]);
      b1 = 0;
      for (int i = 0; i < 3; i++)
         b1 += -w[i] * w[i] * dVdsigma[i] * (V[i] - Vsint[i]);
      deltaI = b1 / a11;
      sigma_cur = sigma_cur + deltaI;
      recalculationOfPotentials();
      recalculationOfDerivativesOfPotentials();
      F = 0;
      for (int i = 0; i < 3; i++)
         F += (w[i] * (V[i] - Vsint[i])) * (w[i] * (V[i] - Vsint[i]));
      out << k << ';' << sigma_cur << ';' << F << "\n";
      cout << k << '\t' << sigma_cur << '\t' << F << "\n";
      k++;
   }
   out << "\n";
}



void main()
{
   ofstream out("out.csv");
   out.clear();
   output(0);
   output(0.1);
   output(-0.1);
   out.close();
}
