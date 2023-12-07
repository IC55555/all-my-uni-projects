#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <Windows.h>
#include <array>

using namespace std;

struct coords
{
   double x = 0, y = 0;
};

struct elems
{

};


double function(double x)//функция
{
   if (x > 0 && x < 2)
      return 0;
   if (x > 2 && x < 3)
      return 1;
   if (x > 3 && x < 7)
      return 0.25;
   //return x;
   //return sqrt(x);
}

class elem //конечные элементы
{
   //double a1 = 0, a2 = 0;//a1 свободный коэф, a2 угловой коэф
   //double left = 0, right = 0; //интервал для текущего элемента
   double x_left = 0, x_right = 0; //граница элементов по x
   double y_left = 0, y_right = 0; //граница элементов по y

   double h_x = 0, h_y = 0;//длина и высота эдемента


   elem(double x0, double x1, double y0, double y1)
   {
      x_left = x0;
      x_right = x1;
      y_left = y0;
      y_right = y1;
      h_x = x1 - x0;
      h_y = y1 - y0;
   }

   //double h_x()//длинна элемента по x
   //{
   //   return x_right - x_left; 
   //}

   //double h_y()//высота элемента по y
   //{
   //   return y_right - y_left;
   //}

   double X1(double x)
   {
      return (x_right - x) / h_x;
   }

   double X2(double x)
   {
      return (x - x_left) / h_x;
   }

   double Y1(double y)
   {
      return (y_right - y) / h_y;
   }

   double Y2(double y)
   {
      return (y - y_left) / h_y;
   }

   double psi_1(double x, double y)
   {
      return X1(x) * Y1(y);
   }

   double psi_2(double x, double y)
   {
      return X2(x) * Y1(y);
   }

   double psi_3(double x, double y)
   {
      return X1(x) * Y2(y);
   }

   double psi_4(double x, double y)
   {
      return X2(x) * Y2(y);
   }


  /* double calculate(double x)
   {
      if (x >= left && x < right)
      {
         return a1 + a2 * x;
      }
      else
      {
         return 0;
      }
   }*/




   double len()
   {
      return right - left;
   }

   double limit(double x) // находим предел функции в точках разрыва
   {
      if (x > left && x < right)
      {
         return function(x);
      }

      else if (x == left)
      {
         return function(x + 0.000001);
      }
      else if (x == right)
      {
         return function(x - 0.000001);
      }
   }
};

double a, b; //границы отрезка

int nodes_count; //колличество узлов
double h; //шаг разбиения
vector<elems> result;
int N; //колличество точек для проверки

vector<coords> nodes;//узлы
double u1, u2; //краевое первого типа, второго типа
double T;//функция температуры

vector<vector<double>> A_global; //глобальная матрица(nodes_count на nodes_count)
vector<vector<vector<double>>> A_local;//локальные матрицы(2 на 2)
vector<vector<double>> B_local; //локальная вектор правой части (столбцы 1 на 2)
vector<double> B_global; //глобльный векторправой части (1 на nodes_count)

vector<vector<double>> L;
vector<vector<double>> U;

vector<double> y; //прямой ход гауса
vector<double> x; //обратный ход гауса
vector<double> q; //вектор решения

vector<double> nodes_X;//узловые иксы
vector<double> nodes_Y;//узловые игрики


int x_count; //колличество х для подстановки в качестве аргумента u_pr


double xi_1(double x, elems elem) // первая локальная функция для конечного эдемента
{
   double h_k = elem.right - elem.left;
   return (elem.right - x) / h_k;
}

double xi_2(double x, elems elem) // вторая локальная функция для конечного эдемента
{
   double h_k = elem.right - elem.left;
   return (x - elem.left) / h_k;
}

double lambda()
{
   return 1;
}

double sigma()
{
   return 0;
}

double gamma()
{
   return 1;
}

double khi()
{
   return 0;
}



double length(coords v)
{
   return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

void input() //ввод из файла
{
   ifstream in("input.txt");
   in >> nodes_count >> x_count;
   nodes.resize(nodes_count);
   for (int i = 0; i < nodes_count; i++)
   {
      in >> nodes[i].x;
   }
   in.close();

   ifstream S1("S1.txt");

   S1 >> u1 /*>> u2*/;
   S1.close();

   ifstream S2("S2.txt");

   S2 >> u2 /*>> u2*/;
   S2.close();

}


void init()//нахождение шага проверки, заполнение узлов по y
{

   //double x = a;
   h = (b - a) / (N - 1.0);

   for (int i = 0; i < nodes_count; i++)
   {

      nodes[i].y = function(nodes[i].x);
   }

   result.resize(nodes_count - 1);

   for (int i = 0; i < nodes_count - 1; i++)
   {
      //result[i].a2 = sqrt(pow(nodes[i].y - nodes[i + 1].y, 2) + pow(nodes[i].x - nodes[i + 1].x, 2));
      //a1 свободный коэф, a2 угловой коэф
      //result[i].a1 = (nodes[i].y * nodes[i + 1].x - nodes[i].x * nodes[i + 1].y) / fabs(nodes[i + 1].x - nodes[i].x);
     //result[i].a2 = fabs(nodes[i + 1].y - nodes[i].y) / fabs(nodes[i + 1].x - nodes[i].x);
      result[i].left = nodes[i].x;
      result[i].right = nodes[i + 1].x;
   }

   y.resize(nodes_count);
   x.resize(nodes_count);
   //double temp_matrix[2][2];
}


double psi(double x, int i)
{

   if (i == 0)
   {
      if (x >= nodes[0].x && x <= nodes[1].x)
      {
         double h_0 = result[0].len();
         return (nodes[1].x - x) / h_0;
      }
      else
      {
         return 0;
      }
   }

   if (i == nodes_count - 1)
   {
      if (x <= nodes[i - 1].x)
      {
         return 0;
      }
      else
      {
         double h_last = result[i - 1].len();
         return (x - nodes[i - 1].x) / h_last;
      }
   }

   if (x <= nodes[i - 1].x)
   {
      return 0;
   }
   else if (x >= nodes[i - 1].x && x <= nodes[i].x)
   {
      double h_i_minus_1 = result[i - 1].len();
      return (x - nodes[i - 1].x) / h_i_minus_1;

   }
   else if (x >= nodes[i].x && x <= nodes[i + 1].x)
   {
      double h_i = result[i].len();
      return (nodes[i + 1].x - x) / h_i;
   }
   else if (x >= nodes[i + 1].x)
   {
      return 0;
   }
}

void output()//вычисление погрешности
{
   q = x;
   //double x = a;
   double f;
   double f_uprox = 0;
   double div;
   N = nodes_count;

   cout << "Nodes: (";
   for (int i = 0; i < N; i++)
   {
      cout << nodes[i].x << "; ";

   }
   cout << ')' << endl;

   cout << "q = (";
   for (int i = 0; i < N; i++)
   {
      cout << q[i] << "; ";
   }
   cout << ')' << endl;


   cout << "u_pr(x) = " << "\t";
   for (int i = 0; i < nodes_count - 1; i++)
   {
      cout << q[i] << " * " << "xi_" << i << "(x)" << " + ";

   }
   cout << q[nodes_count - 1] << " * " << "xi_" << nodes_count - 1 << "(x)" << endl;

   double x = nodes[0].x;
   double h = (nodes[nodes_count - 1].x - nodes[0].x) / (x_count - 1.0); //шаг разбиения

   double u_pr = 0;



   for (int i = 0; i < x_count; i++)
   {
      u_pr = 0;
      cout << "u_pr(" << x << ") = ";
      for (int i = 0; i < nodes_count; i++)
      {
         //cout << q[i] << " * " << "xi_" << i << "(x)" << " + ";
         u_pr += q[i] * psi(x, i);
      }
      cout << u_pr << endl;
      x += h;
   }

}

void LU()
{
   L.resize(nodes_count);
   U.resize(nodes_count);
   // U = A_global;

   for (int k = 0; k < nodes_count; k++)
   {
      L[k].resize(nodes_count);
      L[k].assign(nodes_count, 0);
      U[k].resize(nodes_count);
      U[k].assign(nodes_count, 0);
      U[k][k] = 1;
   }

   L[0][0] = A_global[0][0];
   for (int i = 1; i < nodes_count; i++)
   {
      for (int j = 0; j <= i - 1; j++)
      {
         L[i][j] = A_global[i][j];
         for (int k = 0; k <= j - 1; k++)
         {
            L[i][j] -= L[i][k] * U[k][j];
         }
      }

      for (int j = 0; j <= i - 1; j++)
      {
         U[j][i] = 1.0 / L[j][j] * A_global[j][i];
         for (int k = 0; k <= j - 1; k++)
         {
            U[j][i] -= 1.0 / L[j][j] * L[j][k] * U[k][i];
         }
      }

      L[i][i] = A_global[i][i];
      for (int k = 0; k <= i - 1; k++)
      {
         L[i][i] -= L[i][k] * U[k][i];
      }
   }

   /* for (int i = 0; i < nodes_count; i++)
    {
       for (int j = 0; j < nodes_count; j++)
       {
          cout << L[i][j] << "\t";
       }

       cout << endl;
    }

    cout << endl;
    cout << endl;
    cout << endl;
    for (int i = 0; i < nodes_count; i++)
    {
       for (int j = 0; j < nodes_count; j++)
       {
          cout << U[i][j] << "\t";
       }

       cout << endl;
    }
    cout << endl;
    cout << endl;
    cout << endl;*/
}

double gauss(vector<vector<double>> M, vector<double> B, int N, vector<double> &Q) //гаус для подсчета
{
   double max;
   int k, index;
   const double eps = 1e-15;  // точность 
   k = 0;
   while (k < N)
   {
      //Поиск строки с максимальным M[i][k] 
      max = abs(M[k][k]);
      index = k;
      for (int i = k + 1; i < N; i++)
      {
         if (abs(M[i][k]) > max)
         {
            max = abs(M[i][k]);
            index = i;
         }
      }

      //Проверка на наличие ненулевых элементов в столбце ? ? 
      if (max <= eps)
      {
         cout << "Решение получить невозможно из-за нулевого столбца ";
         cout << index << " матрицы A" << endl;
         /*system("pause");
         exit(-1);*/
      }

      //Перестановка строк 
      for (int j = 0; j < N; j++)
      {
         double temp = M[k][j];
         M[k][j] = M[index][j];
         M[index][j] = temp;
      }

      double temp = B[k];
      B[k] = B[index];
      B[index] = temp;
      //Нормализация уравнений 
      for (int i = k; i < N; i++)
      {
         double temp = M[i][k];
         if (abs(temp) > eps) // для нулевого коэффициента пропустить 
         {
            for (int j = 0; j < N; j++)
            {
               M[i][j] = M[i][j] / temp;
            }

            B[i] = B[i] / temp;
            if (i != k) { // уравнение не вычитать само из себя 
               for (int j = 0; j < N; j++)
               {
                  M[i][j] = M[i][j] - M[k][j];
               }
               B[i] = B[i] - B[k];
            }
         }
      }
      k++;
   }
   //cout << setprecision(15) << "y; " << B[0] << "  " << B[1] << "  " << B[2] << endl;
   //обратный ход 
   for (k = N - 1; k >= 0; k--)
   {
      Q[k] = B[k];
      for (int i = 0; i < k; i++)
      {
         B[i] = B[i] - M[i][k] * Q[k];
      }
   }
   return 0;
}

void S1()
{
   int i = 0;
   A_global[i][i] = 1;
   for (int j = 1; j < nodes_count; j++)
   {
      A_global[i][j] = 0;
   }
   B_global[i] = u1;
}

void S2()
{
   int i = nodes_count - 1;
   B_global[i] += u2;

}


void MFE()//МКЕ
{
   A_local.resize(nodes_count - 1);

   for (int i = 0; i < nodes_count - 1; i++)//построение локальных матриц
   {
      //значения интегралов со страницы 75
      A_local[i].resize(2);

      A_local[i][0].resize(2);
      A_local[i][1].resize(2);

      A_local[i][0][0] = 1.0 / (result[i].right - result[i].left);
      A_local[i][0][1] = -1.0 / (result[i].right - result[i].left);
      A_local[i][1][0] = -1.0 / (result[i].right - result[i].left);
      A_local[i][1][1] = 1.0 / (result[i].right - result[i].left);
      //A_local.push_back(temp_matrix);// вот тут ошибка
      //A_local[i] = temp_matrix;

     /* cout << A_local[i][0][0] << "   " << A_local[i][0][1] << endl;
      cout << A_local[i][1][0] << "   " << A_local[i][1][1] << endl << endl;*/
   }

   A_global.resize(nodes_count);

   for (int k = 0; k < nodes_count; k++)
   {
      A_global[k].resize(nodes_count);
      A_global[k].assign(nodes_count, 0);
   }

   for (int k = 0; k < nodes_count - 1; k++)//построение глобальной матрицы
   {
      A_global[k][k] += A_local[k][0][0];
      A_global[k][k + 1] += A_local[k][0][1];
      A_global[k + 1][k] += A_local[k][1][0];
      A_global[k + 1][k + 1] += A_local[k][1][1];
   }

   double h_k;
   B_local.resize(nodes_count - 1);

   for (int k = 0; k < nodes_count - 1; k++)//построение локальных векторов правой части страница 78, формула 2.68
   {
      h_k = result[k].right - result[k].left;
      B_local[k].resize(2);
      B_local[k][0] = h_k / 6.0 * (2 * result[k].limit(nodes[k].x) + result[k].limit(nodes[k + 1].x));
      B_local[k][1] = h_k / 6.0 * (result[k].limit(nodes[k].x) + 2 * result[k].limit(nodes[k + 1].x));

      //B_local[k][0] = h_k / 6.0 * (2 * function(nodes[k].x) + function(nodes[k + 1].x));
      //B_local[k][1] = h_k / 6.0 * (function(nodes[k].x) + 2 * function(nodes[k + 1].x));
   }

   B_global.resize(nodes_count);
   B_global.assign(nodes_count, 0);

   for (int k = 0; k < nodes_count - 1; k++)//построение глобального вектора правой части
   {
      B_global[k] += B_local[k][0];
      B_global[k + 1] += B_local[k][1];
   }

   //for (int i = 0; i < nodes_count; i++)
   //{
   //   for (int j = 0; j < nodes_count; j++)
   //   {
   //      cout << A_global[i][j] << "\t";
   //   }
   //   cout << endl;
   //  
   //}
   //cout << endl;
   //cout << endl;

   //for (int i = 0; i < nodes_count; i++)
   //{
   //  cout << B_global[i] << "\t";
   //}
   //cout << endl;
   //cout << endl;
}

void main()
{
   setlocale(LC_ALL, "rus");
   input();
   init();
   MFE();
   S1();
   S2();

   /*for (int i = 0; i < nodes_count; i++)
   {
      for (int j = 0; j < nodes_count; j++)
      {
         cout << A_global[i][j] << "\t";
      }
      cout << endl;

   }
   cout << endl;
   cout << endl;

   for (int i = 0; i < nodes_count; i++)
   {
      cout << B_global[i] << "\t";
   }
   cout << endl;
   cout << endl;*/

   LU();
   //gauss(L, B_global, nodes_count, y);
   //gauss(U, y, nodes_count, x);


   /*ifstream in("test.txt");

   for (int i = 0; i < A_global.size(); i++)
   {
      for (int j = 0; j < A_global[i].size(); j++)
      {
         in >> A_global[i][j];
      }

   }
   in.close();

   B_global[0] = 1;
   B_global[1] = 0.5;
   B_global[2] = 1;
   B_global[3] = -0.5;*/



   gauss(A_global, B_global, nodes_count, x);
   output();
   system("pause");
}
