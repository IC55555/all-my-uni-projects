#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <set>
#include <iomanip>
#include <conio.h>

using namespace std;

class SLAE 
{
public:
   int test_num;
   int n_x, n_y; //узлы областей
   int nxw, nyw, l;
   int count_x, count_y, count; //узлы областей + внутренние
   double sumvec;
   int max_it = 10000;
   double eps = 1e-10;
   vector<double> ax, ay; //узлы областей
   vector<double> x, y; //узлы по х и у
   vector<int> oxn, oyn; //вектор для подсчета текущего количества отрезков
   vector<int> nx, ny; // число отрезков между двумя координатами
   vector<double> qx, qy; //коэффицинт разрядки (если равны 1, то сетка равномерная)
   vector<int> areas;//массив областей
   //краевые
   int ncond1;
   //vector<vector<int>> cond1;
   //матрицы и вектора
   vector<vector<double>> M;//матрица в диагональном
   vector<double> b; //вектор правой части
   vector<double> u; //функция на границе
   vector<double> u_true; //функия внутренних узлов
   vector<double> f; //правая часть при полученном решении
   void read_net(string net_file)
   {
      ifstream fin(net_file + ".txt");
      fin >> n_x >> n_y;
      ax.resize(n_x);
      ay.resize(n_y);
      nx.resize(n_x - 1);
      ny.resize(n_y - 1);
      qx.resize(n_x - 1);
      qy.resize(n_y - 1);

      for (int i = 0; i < n_x; i++)
         fin >> ax[i];
      for (int i = 0; i < n_x - 1; i++)
         fin >> nx[i];
      for (int i = 0; i < n_x - 1; i++)
         fin >> qx[i];
      for (int i = 0; i < n_y; i++)
         fin >> ay[i];
      for (int i = 0; i < n_y - 1; i++)
         fin >> ny[i];
      for (int i = 0; i < n_y - 1; i++)
         fin >> qy[i];
      count_x = 1;
      for (int i = 0; i < n_x - 1; i++)
         count_x += nx[i]; //суммарное количество отрезков по х
      count_y = 1;
      for (int i = 0; i < n_y - 1; i++)
         count_y += ny[i]; //суммарное количество отрезков по у
      count = count_x * count_y;
      oxn.resize(n_x);
      oyn.resize(n_y);
      oxn[0] = 0;
      for (int i = 1; i < n_x; i++) //текущее количество отрезков по х
      {
         oxn[i] = oxn[i - 1] + nx[i - 1];
      }
      oyn[0] = 0;
      for (int i = 1; i < n_y; i++) //текущее количество отрезков по у
      {
         oyn[i] = oyn[i - 1] + ny[i - 1];
      }
      fin.close();
   }

   void init_net() //построение сетки
   {
      double hx, hy, tx, ty;
      for (int j = 0; j < n_x - 1; j++) 
      {
         //обработка по х
         if (qx[j] != 1) //для неравномерной сетки
            hx = (ax[j + 1] - ax[j]) * (1. - qx[j]) / (1. - pow(qx[j], nx[j])); //построение шага на основании соседних узлов
         else hx = (ax[j + 1] - ax[j]) / nx[j]; //для равномерной сетки
         tx = ax[j];
         for (int k = 0; k < nx[j]; k++) 
         {
            x.push_back(tx);
            tx += hx; //hx это шаг
            hx *= qx[j];
         }
      }
      x.push_back(tx);
      for (int i = 0; i < n_y - 1; i++)
      {
         //обработка по y
         if (qy[i] != 1)
            hy = (ay[i + 1] - ay[i]) * (1. - qy[i]) / (1. - pow(qy[i], ny[i]));//построение шага на основании соседних узлов
         else hy = (ay[i + 1] - ay[i]) / ny[i];
         ty = ay[i];
         for (int k = 0; k < ny[i]; k++)
         {
            y.push_back(ty);
            ty += hy; //hу это шаг
            hy *= qy[i];
         }
      }
      y.push_back(ty);
   }

   void init_areas() 
   {
      areas.resize((n_x - 1) * (n_y - 1));
      for (int i = 0; i < (n_x - 1) * (n_y - 1); i++)
         areas[i] = 1;
      areas[6] = 0;
      areas[8] = 0;
   }

   void init_matrix()
   {
      M.resize(count);
      b.resize(count);
      u.resize(count);
      f.resize(count);
      for (int i = 0; i < count; i++)
      {
         M[i].resize(5);
         M[i][0] = 0;
         M[i][1] = 0;
         M[i][2] = 1;
         M[i][3] = 0;
         M[i][4] = 0;
         b[i] = 0;
         u[i] = 0;
      }
   }

   void init_u_true()
   {
      u_true.resize(count);
      for (int iy = 0; iy < count_y; iy++) 
      {
         for (int ix = 0; ix < count_x; ix++)
            u_true[get_global(iy, ix)] = get_cond1(x[ix], y[iy]);
      }
   }

   double get_lyam(double x, double y) //лямбда(на случай если надо быстро заменить)
   {
      return 1;
   }

   double get_gamma(double x, double y) //гамма
   {
      return 1;
   }

   double get_f(double x, double y) //выбор функции в зависимости от номера теста
   {
      switch (test_num) 
      {
      case 1: return 5;
      case 2: return x + y;
      case 3: return x * x + y * y - 4;
      case 4: return x * x * x + y * y * y - 6 * x - 6 * y;
      case 5: return x * x * x * x + y * y * y * y - 12 * x * x - 12 * y * y;
      default: exit(0);
      }
   }

   double get_cond1(double x, double y)//выбор краеых в зависимости от номера теста
   {
      switch (test_num)
      {
      case 1: return 5;
      case 2: return x + y;
      case 3: return x * x + y * y;
      case 4: return x * x * x + y * y * y;
      case 5: return x * x * x * x + y * y * y * y;
      default: exit(0);
      }
   }

   void make_matrix() 
   {
      double hx_i, hx_ip, hy_i, hy_ip;
      double tx, ty;
      double glob_num;
      // производная второго порядка на неравномерной сетке
      //только по внутренним узлам
      for (int i = 0; i < n_y - 1; i++)
      {
         for (int j = 0; j < n_x - 1; j++) 
         {
            if (areas[i * (n_x - 1) + j]) 
            {
               for (int iy = oyn[i] + 1; iy < oyn[i + 1]; iy++)
               {
                  for (int ix = oxn[j] + 1; ix < oxn[j + 1]; ix++)
                  {
                     glob_num = get_global(iy, ix);
                     tx = x[ix];
                     ty = y[iy];
                     hx_i = x[ix + 1] - x[ix];
                     hx_ip = x[ix] - x[ix - 1];
                     hy_i = y[iy + 1] - y[iy];
                     hy_ip = y[iy] - y[iy - 1];
                     M[glob_num][2] = 2 * get_lyam(tx, ty) * (1 / (hx_ip * hx_i) + 1 / (hy_ip * hy_i));
                     M[glob_num][1] = -2 * get_lyam(tx, ty) / (hx_ip * (hx_ip + hx_i));
                     M[glob_num][3] = -2 * get_lyam(tx, ty) / (hx_i * (hx_ip + hx_i));
                     M[glob_num][0] = -2 * get_lyam(tx, ty) / (hy_ip * (hy_ip + hy_i));
                     M[glob_num][4] = -2 * get_lyam(tx, ty) / (hy_i * (hy_ip + hy_i));
                     M[glob_num][2] += get_gamma(tx, ty);
                     b[glob_num] += get_f(tx, ty);
                  }
               }
            }
         }
      }
      // производная второго порядка на неравномерной сетке
      //по внутренним границам
      //по вертикальным
      for (int i = 0; i < n_y - 1; i++)
      {
         for (int j = 1; j < n_x - 1; j++) 
         {
            if (areas[i * (n_x - 1) + j] && areas[i * (n_x - 1) + j - 1]) 
            {
               for (int iy = oyn[i] + 1; iy < oyn[i + 1]; iy++)
               {
                  int ix = oxn[j];
                  glob_num = get_global(iy, ix);
                  tx = x[ix];
                  ty = y[iy];
                  hx_i = x[ix + 1] - x[ix];
                  hx_ip = x[ix] - x[ix - 1];
                  hy_i = y[iy + 1] - y[iy];
                  hy_ip = y[iy] - y[iy - 1];
                  M[glob_num][2] = 2 * get_lyam(tx, ty) * (1 / (hx_ip * hx_i) + 1 / (hy_ip * hy_i));
                  M[glob_num][1] = -2 * get_lyam(tx, ty) / (hx_ip * (hx_ip + hx_i));
                  M[glob_num][3] = -2 * get_lyam(tx, ty) / (hx_i * (hx_ip + hx_i));
                  M[glob_num][0] = -2 * get_lyam(tx, ty) / (hy_ip * (hy_ip + hy_i));
                  M[glob_num][4] = -2 * get_lyam(tx, ty) / (hy_i * (hy_ip + hy_i));
                  M[glob_num][2] += get_gamma(tx, ty);
                  b[glob_num] += get_f(tx, ty);
               }
            }
         }
      }
      // производная второго порядка на неравномерной сетке
      //по горизонтальным
      for (int j = 0; j < n_x - 1; j++)
      {
         for (int i = 1; i < n_y - 1; i++)
         {
            if (areas[i * (n_x - 1) + j] && areas[(i - 1) * (n_x - 1) + j])
            {
               for (int ix = oxn[j] + 1; ix < oxn[j + 1]; ix++) 
               {
                  int iy = oyn[i];
                  glob_num = get_global(iy, ix);
                  tx = x[ix];
                  ty = y[iy];
                  hx_i = x[ix + 1] - x[ix];
                  hx_ip = x[ix] - x[ix - 1];
                  hy_i = y[iy + 1] - y[iy];
                  hy_ip = y[iy] - y[iy - 1];
                  M[glob_num][2] = 2 * get_lyam(tx, ty) * (1 / (hx_ip * hx_i) + 1 / (hy_ip * hy_i));
                  M[glob_num][1] = -2 * get_lyam(tx, ty) / (hx_ip * (hx_ip + hx_i));
                  M[glob_num][3] = -2 * get_lyam(tx, ty) / (hx_i * (hx_ip + hx_i));
                  M[glob_num][0] = -2 * get_lyam(tx, ty) / (hy_ip * (hy_ip + hy_i));
                  M[glob_num][4] = -2 * get_lyam(tx, ty) / (hy_i * (hy_ip + hy_i));
                  M[glob_num][2] += get_gamma(tx, ty);
                  b[glob_num] += get_f(tx, ty);
               }
            }
         }
      }
   }

   void add_cond1() 
   {
      double glob_num;
      //нижняя
      for (int j = 0; j < count_x; j++) 
      {
         b[j] = get_cond1(x[j], y[0]);
      }
      //горизонтальные - сверху
      for (int j = 0; j < n_x - 1; j++)
      {
         if (areas[(n_x - 1) + j])
         {
            for (int ix = oxn[j]; ix <= oxn[j + 1]; ix++)
            {
               glob_num = get_global(oyn[n_y - 1], ix);
               b[glob_num] = get_cond1(x[ix], y[oyn[n_y - 1]]);
            }
         }
         else
            for (int ix = oxn[j]; ix <= oxn[j + 1]; ix++) 
            {
               glob_num = get_global(oyn[n_y - 2], ix);
               b[glob_num] = get_cond1(x[ix], y[oyn[n_y - 2]]);
            }
      }

      //вертикальные - справа и слева
      for (int i = 1; i < count_y - 1; i++)
      {
         glob_num = get_global(i, 0);
         b[glob_num] = get_cond1(x[0], y[i]);
         glob_num = get_global(i, count_x - 1);
         b[glob_num] = get_cond1(x[count_x - 1], y[i]);
      }

      //вертикальные - внутри
      for (int j = 1; j < n_x - 1; j++) 
      {
         for (int iy = oyn[n_y - 2] + 1; iy < oyn[n_y - 1]; iy++) 
         {
            glob_num = get_global(iy, oxn[j]);
            b[glob_num] = get_cond1(x[oxn[j]], y[iy]);
         }
      }
   }

   int get_global(int s, int p) //получение глобального номера по локальному
   { 
      int k;
      //s-номер строки, p-номер столбца
      k = s * count_x + p;
      return k;
   }

   void make_f_r() //если грид равен 0
   {
      int glob_num;
      double tmpx, tmpy;
      //только по внутренним узлам
      for (int i = 0; i < n_y - 1; i++) //нахожение вектора правой части
      {
         for (int j = 0; j < n_x - 1; j++)
         {
            if (areas[i * (n_x - 1) + j]) 
            {
               for (int iy = oyn[i] + 1; iy < oyn[i + 1]; iy++) 
               {
                  for (int ix = oxn[j] + 1; ix < oxn[j + 1]; ix++)
                  {
                     glob_num = get_global(iy, ix);
                     tmpx = (u_true[get_global(iy, ix + 1)] - 2 * u_true[get_global(iy, ix)] + u_true[get_global(iy, ix - 1)]) / ((x[ix] - x[ix - 1]) * (x[ix] - x[ix - 1]));
                     tmpy = (u_true[get_global(iy + 1, ix)] - 2 * u_true[get_global(iy, ix)] + u_true[get_global(iy - 1, ix)]) / ((y[iy] - y[iy - 1]) * (y[iy] - y[iy - 1]));
                     f[glob_num] = -tmpx - tmpy + u_true[glob_num];
                  }
               }
            }
         }
      }

      //по внутренним границам
      //по вертикальным
      for (int i = 0; i < n_y - 1; i++) //нахожение вектора правой части на границце(в пределе)
      {
         for (int j = 1; j < n_x - 1; j++) 
         {
            if (areas[i * (n_x - 1) + j] && areas[i * (n_x - 1) + j - 1]) 
            {
               for (int iy = oyn[i] + 1; iy < oyn[i + 1]; iy++) 
               {
                  int ix = oxn[j];
                  glob_num = get_global(iy, ix);
                  tmpx = (u_true[get_global(iy, ix + 1)] - 2 * u_true[get_global(iy, ix)] + u_true[get_global(iy, ix - 1)]) / ((x[ix] - x[ix - 1]) * (x[ix] - x[ix - 1]));
                  tmpy = (u_true[get_global(iy + 1, ix)] - 2 * u_true[get_global(iy, ix)] + u_true[get_global(iy - 1, ix)]) / ((y[iy] - y[iy - 1]) * (y[iy] - y[iy - 1]));
                  f[glob_num] = -tmpx - tmpy + u[glob_num];
               }
            }
         }
      }

      //по горизонтальным
      for (int j = 0; j < n_x - 1; j++) //нахожение вектора правой части на границце(в пределе)
      {
         for (int i = 1; i < n_y - 1; i++) 
         {
            if (areas[i * (n_x - 1) + j] && areas[(i - 1) * (n_x - 1) + j])
            {
               for (int ix = oxn[j] + 1; ix < oxn[j + 1]; ix++) 
               {
                  int iy = oyn[i];
                  glob_num = get_global(iy, ix);
                  tmpx = (u_true[get_global(iy, ix + 1)] - 2 * u_true[get_global(iy, ix)] + u_true[get_global(iy, ix - 1)]) / ((x[ix] - x[ix - 1]) * (x[ix] - x[ix - 1]));
                  tmpy = (u_true[get_global(iy + 1, ix)] - 2 * u_true[get_global(iy, ix)] + u_true[get_global(iy - 1, ix)]) / ((y[iy] - y[iy - 1]) * (y[iy] - y[iy - 1]));
                  f[glob_num] = -tmpx - tmpy + u[glob_num];
               }
            }
         }
      }

      //нижняя
      for (int j = 0; j < count_x; j++) 
      {
         f[j] = get_f(x[j], y[0]);
      }

      //горизонтальные - сверху
      for (int j = 0; j < n_x - 1; j++) 
      {
         if (areas[(n_x - 1) + j])
         {
            for (int ix = oxn[j]; ix <= oxn[j + 1]; ix++) 
            {
               glob_num = get_global(oyn[n_y - 1], ix);
               f[glob_num] = get_f(x[ix], y[oyn[n_y - 1]]);
            }
         }
         else
            for (int ix = oxn[j]; ix <= oxn[j + 1]; ix++) 
            {
               glob_num = get_global(oyn[n_y - 2], ix);
               f[glob_num] = get_f(x[ix], y[oyn[n_y - 2]]);
            }
      }

      //вертикальные - справа и слева
      for (int i = 1; i < count_y - 1; i++)
      {
         glob_num = get_global(i, 0);
         f[glob_num] = get_f(x[0], y[i]);
         glob_num = get_global(i, count_x - 1);
         f[glob_num] = get_f(x[count_x - 1], y[i]);
      }

      //вертикальные - внутри
      for (int j = 1; j < n_x - 1; j++)
      {
         for (int iy = oyn[n_y - 2] + 1; iy < oyn[n_y - 1]; iy++) 
         {
            glob_num = get_global(iy, oxn[j]);
            f[glob_num] = get_f(x[oxn[j]], y[iy]);
         }
      }
   }

   void make_f_nr() //если грид не равен 0
   {
      int glob_num;
      double tmpx1, tmpy1, tmpx2, tmpy2, tmpx3, tmpy3;
      double hx1, hx2, hy1, hy2;
      //только по внутренним узлам
      for (int i = 0; i < n_y - 1; i++)
      {
         for (int j = 0; j < n_x - 1; j++) 
         {
            if (areas[i * (n_x - 1) + j]) 
            {
               for (int iy = oyn[i] + 1; iy < oyn[i + 1]; iy++)
               {
                  for (int ix = oxn[j] + 1; ix < oxn[j + 1]; ix++) 
                  {
                     //пятиточечное разностное выражение
                     glob_num = get_global(iy, ix);
                     hx1 = x[ix] - x[ix - 1];
                     hx2 = x[ix + 1] - x[ix];
                     hy1 = y[iy] - y[iy - 1];
                     hy2 = y[iy + 1] - y[iy];
                     tmpx1 = 2 * u_true[get_global(iy, ix - 1)] / (hx1 * (hx1 + hx2));
                     tmpx2 = 2 * u_true[get_global(iy, ix)] / (hx2 * hx1);
                     tmpx3 = 2 * u_true[get_global(iy, ix + 1)] / (hx2 * (hx1 + hx2));
                     tmpy1 = 2 * u_true[get_global(iy - 1, ix)] / (hy1 * (hy1 + hy2));
                     tmpy2 = 2 * u_true[get_global(iy, ix)] / (hy2 * hy1);
                     tmpy3 = 2 * u_true[get_global(iy + 1, ix)] / (hy2 * (hy1 + hy2));
                     f[glob_num] = -tmpx1 + tmpx2 - tmpx3 - tmpy1 + tmpy2 - tmpy3 + u_true[glob_num];
                  }
               }
            }
         }
      }

      //по внутренним границам
      //по вертикальным
      for (int i = 0; i < n_y - 1; i++) 
      {
         for (int j = 1; j < n_x - 1; j++) 
         {
            if (areas[i * (n_x - 1) + j] && areas[i * (n_x - 1) + j - 1]) 
            {
               for (int iy = oyn[i] + 1; iy < oyn[i + 1]; iy++)
               {
                  //пятиточечное разностное выражение
                  int ix = oxn[j];
                  glob_num = get_global(iy, ix);
                  hx1 = x[ix] - x[ix - 1];
                  hx2 = x[ix + 1] - x[ix];
                  hy1 = y[iy] - y[iy - 1];
                  hy2 = y[iy + 1] - y[iy];
                  tmpx1 = 2 * u_true[get_global(iy, ix - 1)] / (hx1 * (hx1 + hx2));
                  tmpx2 = 2 * u_true[get_global(iy, ix)] / (hx2 * hx1);
                  tmpx3 = 2 * u_true[get_global(iy, ix + 1)] / (hx2 * (hx1 + hx2));
                  tmpy1 = 2 * u_true[get_global(iy - 1, ix)] / (hy1 * (hy1 + hy2));
                  tmpy2 = 2 * u_true[get_global(iy, ix)] / (hy2 * hy1);
                  tmpy3 = 2 * u_true[get_global(iy + 1, ix)] / (hy2 * (hy1 + hy2));
                  f[glob_num] = -tmpx1 + tmpx2 - tmpx3 - tmpy1 + tmpy2 - tmpy3 + u_true[glob_num];
               }
            }
         }
      }

      //по горизонтальным
      for (int j = 0; j < n_x - 1; j++) 
      {
         for (int i = 1; i < n_y - 1; i++) 
         {
            if (areas[i * (n_x - 1) + j] && areas[(i - 1) * (n_x - 1) + j]) 
            {
               for (int ix = oxn[j] + 1; ix < oxn[j + 1]; ix++) 
               {
                  //пятиточечное разностное выражение
                  int iy = oyn[i];
                  glob_num = get_global(iy, ix);
                  hx1 = x[ix] - x[ix - 1];
                  hx2 = x[ix + 1] - x[ix];
                  hy1 = y[iy] - y[iy - 1];
                  hy2 = y[iy + 1] - y[iy];
                  tmpx1 = 2 * u_true[get_global(iy, ix - 1)] / (hx1 * (hx1 + hx2));
                  tmpx2 = 2 * u_true[get_global(iy, ix)] / (hx2 * hx1);
                  tmpx3 = 2 * u_true[get_global(iy, ix + 1)] / (hx2 * (hx1 + hx2));
                  tmpy1 = 2 * u_true[get_global(iy - 1, ix)] / (hy1 * (hy1 + hy2));
                  tmpy2 = 2 * u_true[get_global(iy, ix)] / (hy2 * hy1);
                  tmpy3 = 2 * u_true[get_global(iy + 1, ix)] / (hy2 * (hy1 + hy2));
                  f[glob_num] = -tmpx1 + tmpx2 - tmpx3 - tmpy1 + tmpy2 - tmpy3 + u_true[glob_num];
               }
            }
         }
      }

      //нижняя
      for (int j = 0; j < count_x; j++)
      {
         f[j] = get_f(x[j], y[0]);
      }

      //горизонтальные - сверху
      for (int j = 0; j < n_x - 1; j++)
      {
         if (areas[(n_x - 1) + j]) 
         {
            for (int ix = oxn[j]; ix <= oxn[j + 1]; ix++) 
            {
               glob_num = get_global(oyn[n_y - 1], ix);
               f[glob_num] = get_f(x[ix], y[oyn[n_y - 1]]);
            }
         }
         else
            for (int ix = oxn[j]; ix <= oxn[j + 1]; ix++) 
            {
               glob_num = get_global(oyn[n_y - 2], ix);
               f[glob_num] = get_f(x[ix], y[oyn[n_y - 2]]);
            }
      }

      //вертикальные - справа и слева
      for (int i = 1; i < count_y - 1; i++) 
      {
         glob_num = get_global(i, 0);
         f[glob_num] = get_f(x[0], y[i]);
         glob_num = get_global(i, count_x - 1);
         f[glob_num] = get_f(x[count_x - 1], y[i]);
      }

      //вертикальные - внутри
      for (int j = 1; j < n_x - 1; j++) 
      {
         for (int iy = oyn[n_y - 2] + 1; iy < oyn[n_y - 1]; iy++)
         {
            glob_num = get_global(iy, oxn[j]);
            f[glob_num] = get_f(x[oxn[j]], y[iy]);
         }
      }

}

double iteration(vector<double> &y, vector<double> &y_new, double w) //w = 1(из конструктора)
{
   //вычисление невязки 
   double s = 0, sumnev = 0, nev;
   for (int i = 0; i < count; i++)
   {
      s = 0;
      if (M[i][0]) s += M[i][0] * y[i - count_x];
      if (M[i][1]) s += M[i][1] * y[i - 1];
      if (M[i][2]) s += M[i][2] * y[i];
      if (M[i][3]) s += M[i][3] * y[i + 1];
      if (M[i][4]) s += M[i][4] * y[i + count_x];
      nev = b[i] - s;
      y_new[i] = y[i] + (w / M[i][2]) * nev;
      sumnev += nev * nev;
   }
   sumnev = sqrt(sumnev);
   return sumnev;
}

void seidel(double w) //w = 1
{
   double norm_b = 0;
   for (int i = 0; i < count; i++)
      norm_b += b[i] * b[i];
   norm_b = sqrt(norm_b);
   double eps_b;
   for (int k = 0; k < count; k++)
      u[k] = 0;
   int i = 1;

   do 
   {
      double nev = iteration(u, u, w);
      eps_b = nev / norm_b; //невязка поделить на норму вектора правой части, для получения погрешности
      i++;
   } 
   while (i <= max_it && eps_b > eps);
   cout << "iter=" << i << endl;
   if (i > max_it) cout << "max iteration reached" << endl;
   cout << setprecision(15) <<"nevyazka " << eps_b << endl;
}

SLAE(string net_file, int test, int grid) //конструктор класса слау
{
   test_num = test;
   read_net(net_file); //чтение из файла
   init_net(); //инициализация сетки
   init_areas(); //инициализаия областей
   init_matrix(); //инит матрицы
   init_u_true(); //инит функции
   make_matrix(); //построение матрицы
   add_cond1(); //добавление краевых условий
   seidel(1); //метод зэйделя
   if (grid == 0) make_f_r();
   else make_f_nr();
   cout << scientific << setprecision(15);
   for (int i = 0; i < y.size(); i++) 
   {
      for (int j = 0; j < x.size(); j++)
         cout << endl << i * x.size() + j << "= " << u[i * x.size() + j] << " f= " << f[i * x.size() + j] << " u_t= " << u_true[i * x.size() + j];
   }
   cout << endl;
}

~SLAE() //деструктор векторов
{
   ax.~vector();
   ay.~vector();
   x.~vector();
   y.~vector();
   oxn.~vector();
   oyn.~vector();
   nx.~vector();
   ny.~vector();
   qx.~vector();
   qy.~vector();
   areas.~vector();
   //cond1.~vector();
   b.~vector();
   u.~vector();
   f.~vector();
   for (int i = 0; i < M.size(); i++)
      M[i].~vector();
   M.~vector();
}
};

int main() 
{
   char key_test, key_grid;
   cout << "Select a test" << endl;
   key_test = _getch();
   cout << "You've selected test number " << (int)key_test - 48 << endl;
   cout << "Select a grid" << endl;
   key_grid = _getch();
   cout << "You've selected grid number " << (int)key_grid - 48 << endl;
   //net- равномерная, nernet-неравномерная
   SLAE slv = SLAE("net3", (int)key_test - 48, (int)key_grid - 48); //создание объекта класса слау и запрашивание файла, теста и сетки
   cout << slv.test_num;
   system("pause");
   return 0;
}

//напомминалка, изначально 5,1