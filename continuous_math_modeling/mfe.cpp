#include "mfe.h"

#include <iostream>

using namespace std;
// Тестовая функция. Необходима для вычисления значений в дополнительных узлах
// при построении первых краевых условий
//=========================================================================================
double mfe::test_func(double x, double y)
{
   return 0;
}
//=========================================================================================
//=========================================================================================

// Конструктор: читаем сетку, КЭ с номерами их узлов, 1-ые и 2-ые краевые, материалы
// и свойства фаз, параметры для решения СЛАУ. Задаем точки и веса для интегрирования
void mfe::Compute(mvector sigma)
{
   if (di.size() == 0)
   {
      buildPortraitOfMatrix();
   }
   assemblyGlobalMatrix(sigma);
   q.clear();
   q.resize(di.size(), 0);
   bc_1();
   LOS_LU();
}

mfe::mfe()
{
   //-----------------------------------------------------
   // Построение сетки
   mvector r_temp, z_temp, _r, _z;
   ifstream inGrid("node.txt");
   if (inGrid.is_open())
   {
      _r.resize(2, 0);
      inGrid >> KolR >> qr >> _r[0] >> _r[1];

      double hr, hz;
      //Обработка по r
      if (qr != 1)
         hr = (_r[1] - _r[0]) * (1. - qr) / (1. - pow(qr, KolR));
      else
         hr = (_r[1] - _r[0]) / KolR;


      r_temp.push_back(_r[0]);
      for (int i = 0; i < KolR; i++)
      {
         r_temp.push_back(r_temp.back() + hr);
         hr *= qr;
      }

      for (int l = 0; l < 3; l++)
      {
         _z.resize(2, 0);

         inGrid >> KolZ >> qz >> _z[0] >> _z[1];


         //Обработка по z
         if (qz != 1)
            hz = (_z[1] - _z[0]) * (1. - qz) / (1. - pow(qz, KolZ));
         else
            hz = (_z[1] - _z[0]) / KolZ;

         if (l != 0)
         {
            _z[0] += hz;
            hz *= qz;
            KolZ--;
         }

         z_temp.push_back(_z[0]);

         for (int i = 0; i < KolZ; i++)
         {
            z_temp.push_back(z_temp.back() + hz);
            hz *= qz;
         }
      }
      KolR = r_temp.size();
      KolZ = z_temp.size();
      Nuz = KolR * KolZ;
      MeshRZ.resize(Nuz);
      for (int i = 0, k = 0; i < KolZ; i++)
         for (int j = 0; j < KolR; j++, k++)
            MeshRZ[k] = make_pair(r_temp[j], z_temp[i]);
      r_temp.clear();
      z_temp.clear();
      _r.clear();
      _z.clear();
   }
   else throw BAD_READ;
   inGrid.close();
   //-----------------------------------------
   ifstream inMats("mats.txt");
   Materials.resize(3);
   for (int i = 0; i < 3; i++)
   {
      inMats >> Materials[i].h0 >> Materials[i].h1 >> Materials[i].sigma;
   }
   inMats.close();
   //-----------------------------------------
   ofstream outGrid("nodeRZ.txt");
   outGrid.precision(15);
   outGrid << MeshRZ.size() << endl;
   for (int i = 0; i < MeshRZ.size(); i++)
   {
      outGrid << MeshRZ[i].first << " " << MeshRZ[i].second << endl;
   }
   outGrid.close();


   //-----------------------------------------------------
   //Конечный элемент задается номерами его узлов
   F.resize(Nuz);
   Nel = (KolR - 1) * (KolZ - 1);
   FE.resize(Nel);
   for (int i = 0; i < Nel; i++)
      FE[i].resize(4);
   int n1, n2, n3, n4;
   for (int i = 0, k = 0, m = 1; i < KolZ - 1; i++, m++)
   {
      for (int j = 0; j < KolR - 1; j++, m++, k++)
      {
         n1 = m - 1;
         n2 = m;
         n3 = m + KolR - 1;
         n4 = m + KolR;
         FE[k] = { n1, n2, n3, n4 };
      }
   }
   //-----------------------------------------------------
   //Nbc1 = 2 * (KolR + KolZ - 2);
   Nbc1 = (KolR + KolZ - 1);
   bc1.resize(Nbc1);
   double value_bc1;
   int k = 0;

   ////Нижняя грань
   //for (int i = 0; i < KolR - 1; i++, k++)
   //{
   //   value_bc1 = test_func(MeshRZ[i].first, MeshRZ[i].second);
   //   bc1[k] = make_pair(i, value_bc1);
   //   //bc1[k] = make_pair(i, 0);
   //}
   //Правая грань
   for (int i = KolR - 1; i < KolR * KolZ - 1; k++, i += KolR)
   {
      value_bc1 = test_func(MeshRZ[i].first, MeshRZ[i].second);
      bc1[k] = make_pair(i, value_bc1);
      //bc1[k] = make_pair(i, 0);
   }
   //Верхняя грань
   for (int i = KolR * KolZ - 1; i >= KolR * (KolZ - 1); i--, k++)
   {
      value_bc1 = test_func(MeshRZ[i].first, MeshRZ[i].second);
      bc1[k] = make_pair(i, value_bc1);
   }
   ////Левая грань
   //for (int i = KolR * (KolZ - 1); i > 0; k++, i -= KolR)
   //{
   //   value_bc1 = test_func(MeshRZ[i].first, MeshRZ[i].second);
   //   bc1[k] = make_pair(i, value_bc1);
   //}
   //-----------------------------------------------------
   maxIter = eps = 0;
   ifstream slau("kuslau.txt");
   if (slau.is_open())
   {
      slau >> maxIter >> eps >> maxIterNel >> eps_nel;
      slau.close();
   }
   else throw BAD_READ;
   // Для численного интегрирования
   points.resize(3);
   weights.resize(3);
   // Точки Гаусса-3
   points[0] = 0.0;
   points[1] = sqrt(3.0/5);
   points[2] = -sqrt(3.0 / 5);
   // Квадратурные веса
   weights[0] = 8.0 / 9;
   weights[1] = 5.0 / 9;
   weights[2] = 5.0 / 9;
   // Матрица жесткости
   G.resize(4);
   for (int i = 0; i < 4; i++)
      G[i].resize(4);

   //G_odn.resize(2);
   //for (int i = 0; i < G_odn.size(); i++)
   //   G_odn[i].resize(2);
   //M_odn.resize(2);
   //for (int i = 0; i < M_odn.size(); i++)
   //   M_odn[i].resize(2);

   //G_odn = { {1, -1},
   //          {-1, 1}};

   //M_odn = { {2./6, 1./6},
   //          {1./6, 2./6}};
}
// Определить номера всех глобальных функций каждого элемента
void mfe::PointsToPlot()
{
   ofstream r("r.txt");
   ofstream z("z.txt");
   if (r.is_open() && z.is_open())
   {
      for (int i = 0; i < KolR; i++)
         if(abs(MeshRZ[i].first) <= 1e-1)
            r << MeshRZ[i].first << endl;

      for (int j = 0; abs(MeshRZ[j].second) <= 1e-1; j += KolR)
         z << MeshRZ[j].second << endl;

      r.close();
      z.close();
   }
}

double mfe::psi(int ielem, int func, double x, double y)
{
   double xk = MeshRZ[FE[ielem][0]].first;
   double yk = MeshRZ[FE[ielem][0]].second;
   double xk1 = MeshRZ[FE[ielem][3]].first;
   double yk1 = MeshRZ[FE[ielem][3]].second;
   double hx = abs(xk1 - xk);
   double hy = abs(yk1 - yk);
   switch (func)
   {
   case 0:
      return (xk1 - x) / hx * (yk1 - y) / hy;
   case 1:
      return (x - xk) / hx * (yk1 - y) / hy;
   case 2:
      return (xk1 - x) / hx * (y - yk) / hy;
   case 3:
      return (x - xk) / hx * (y - yk) / hy;
   }
}

double mfe::d_psi(int ielem, int i, double x, double y, int var)
{
   double h = 1e-2; // Шаг для численного дифференцирования
   switch (var)
   {
   case 1:
   {
      return (psi(ielem, i, x + h, y) - psi(ielem, i, x - h, y)) / (2.0 * h);
   }
   case 2:
   {
      return (psi(ielem, i, x, y + h) - psi(ielem, i, x, y - h)) / (2.0 * h);
   }
   }
   /*double xk = MeshRZ[FE[ielem][0]].first;
   double yk = MeshRZ[FE[ielem][0]].second;
   double xk1 = MeshRZ[FE[ielem][3]].first;
   double yk1 = MeshRZ[FE[ielem][3]].second;
   double hx = abs(xk1 - xk);
   double hy = abs(yk1 - yk);
   switch (var)
   {
   case 0:
   {
      switch (i)
      {
      case 0:
         return (y - yk1) / (hx * hy);
      case 1:
         return (yk1 - y) / (hx * hy);
      case 2:
         return (yk - y) / (hx * hy);
      case 3:
         return (y - yk) / (hx * hy);
      }
   }
   case 1:
   {
      switch (i)
      {
      case 0:
         return (x - xk1) / (hx * hy);
      case 1:
         return (xk - x) / (hx * hy);
      case 2:
         return (xk1 - x) / (hx * hy);
      case 3:
         return (x - xk) / (hx * hy);
      }
   }
   }*/
}


int mfe::FindMaterial(int ielem)
{
   int k = 0;
   double eps_mat = 1e-16;
   for (int i = 0; i < Materials.size(); i++)
   {
      double e1 = abs(Materials[i].h0 - MeshRZ[FE[ielem][0]].second);
      double e2 = abs(Materials[i].h1 - MeshRZ[FE[ielem][3]].second);
      if (Materials[i].h0 <= MeshRZ[FE[ielem][0]].second && Materials[i].h1 >= MeshRZ[FE[ielem][3]].second)
         k = i;
   }
   return k;
}

// Построить локальную матрицу жесткости
void mfe::buildLocalG(int ielem, double hx, double hy, int num_mat)
{
   ScalFunc2D f;
   double rk = MeshRZ[FE[ielem][0]].first;
   double zk = MeshRZ[FE[ielem][0]].second;
   double rk1 = MeshRZ[FE[ielem][3]].first;
   double zk1 = MeshRZ[FE[ielem][3]].second;
   for (int i = 0; i < 4; i++)
   {
      for (int j = 0; j <= i; j++)
      {
         f = [ielem, num_mat, i, j, this, rk, zk, rk1, zk1](double r, double z)
            {
               double dpsi_i_r = d_psi(ielem, i, r, z, 1);
               double dpsi_j_r = d_psi(ielem, j, r, z, 1);
               double dpsi_i_z = d_psi(ielem, i, r, z, 2);
               double dpsi_j_z = d_psi(ielem, j, r, z, 2);
               return r * Materials[num_mat].sigma * (dpsi_i_r * dpsi_j_r + dpsi_i_z * dpsi_j_z);
            };
         G[i][j] = G[j][i] =  Integration(f, rk, rk1, zk, zk1);
      }
   }
}
// Добавить элемент в глобальную матрицу
void mfe::addElementToGlobal(int i, int j, double elem)
{
   if (i == j)
   {
      di[i] += elem;
      return;
   }
   else
   {
      for (int ind = ig[i]; ind < ig[i + 1]; ind++)
         if (jg[ind] == j)
         {
            gg[ind] += elem;
            return;
         }
   }
}
// Посчитать divgrad
double mfe::divgrad(double x, double y)
{
   return 0;
}
// Сборка глобальной матрицы
void mfe::assemblyGlobalMatrix(mvector sigma)
{
   //b.resize(4);
   di.clear();
   gg.clear();
   di.resize(Nuz, 0);
   gg.resize(ig.back(), 0);
   for (int ielem = 0; ielem < Nel; ielem++)
   {
      double xk = MeshRZ[FE[ielem][0]].first;
      double yk = MeshRZ[FE[ielem][0]].second;
      double xk1 = MeshRZ[FE[ielem][3]].first;
      double yk1 = MeshRZ[FE[ielem][3]].second;
      double hx = abs(xk1 - xk);
      double hy = abs(yk1 - yk);

      Materials[0].sigma = sigma[0];
      Materials[1].sigma = sigma[1];
      Materials[2].sigma = sigma[2];
      int num_mat = FindMaterial(ielem);
      buildLocalG(ielem, hx, hy, num_mat);

      //ScalFunc2D f;
      //for (int j = 0; j < 4; j++)
      //{
      //   f = [ielem, num_mat, j, this, xk, yk, xk1, yk1](double r, double z)
      //      {
      //         return -r * 1./r * psi(ielem, j, r, z);
      //      };
      //   F[FE[ielem][j]] += Integration(f, xk, xk1, yk, yk1);
      //}
      //
      for (int i = 0; i < FE[ielem].size(); i++)
      {
         for (int j = 0; j <= i; j++)
            addElementToGlobal(FE[ielem][i], FE[ielem][j], G[i][j]);
      }
   }
}
// Построить портрет глобальной матрицы
void mfe::buildPortraitOfMatrix()
{
   vector<vector<int>> list;
      list.resize(Nuz);
   list[0].push_back(0);
   // Идем по всем КЭ
   for (int ielem = 0; ielem < Nel; ielem++)
   {
      // Берем 1-ую соответствующую элементу базисную функцию
      for (int i = 0; i < FE[ielem].size() - 1; i++)
         // Идем по всем остальным функциям, начиная со второй
         for (int j = i + 1; j < FE[ielem].size(); j++)
         {
            // Нужно добавить первую функцию(меньшую) в список ко всем
            // функциям, относящимся к КЭ
            // Поэтому определяем позицию, куда будем добавлять (в какой список)
            int insertPos = FE[ielem][j];
            // Берем сам элемент, который будем вставлять
            int element = FE[ielem][i];
            bool isIn = false;
            // Проверим, есть ли уже этот элемент в списке
            for (int k = 0; k < list[insertPos].size() && !isIn; k++)
               if (element == list[insertPos][k])
                  isIn = true;
            // Если он в списке не найден, то добавляем его
            if (!isIn)
               list[insertPos].push_back(element);
         }
   }
   // Сортируем все получившиеся списки (по возрастанию номеров)
   for (int i = 0; i < Nuz; i++)
      if (!isOrdered(list[i]))
         sort(list[i].begin(), list[i].end());
   //----------------------------------------------------------------
   // Формируем массив ig
   ig.resize(Nuz + 1);
   // 1-ый и 2-ой элементы всегда равны 1, но мы будем нумеровать с 0
   ig[0] = 0;
   ig[1] = 0;
   for (int i = 1; i < list.size(); i++)
      ig[i + 1] = ig[i] + list[i].size();
   //----------------------------------------------------------------
   // Формируем массив jg
   jg.resize(ig.back());
   for (int i = 1, j = 0; i < Nuz; i++)
   {
      for (int k = 0; k < list[i].size(); k++)
         jg[j++] = list[i][k];
   }
}
// Проверка списка на упорядоченность по возрастанию
bool mfe::isOrdered(const pvector& v)
{
   for (int i = 0; i < v.size() - 1; i++)
      if (v[i + 1] < v[i])
         return false;
   return true;
}
// Перевод матрицы в плотный формат
void mfe::toDense(const string _dense)
{
   mat.resize(Nuz);
      for (int i = 0; i < mat.size(); i++)
         mat[i].resize(Nuz, 0);
   for (int i = 0; i < mat.size(); i++)
   {
      mat[i][i] = di[i];
      for (int j = ig[i]; j < ig[i + 1]; j++)
      {
         mat[i][jg[j]] = gg[j];
      }
   }
   ofstream dense(_dense);
   dense.precision(5);
   if (dense.is_open())
   {
      for (int i = 0; i < mat.size(); i++)
      {
         for (int j = 0; j <= i; j++)
            dense << left << setw(12) << mat[i][j];
         dense << endl << endl << endl;
      }
   }
   mat.clear();
}

// Учет первых краевых
void mfe::bc_1()
{
   vector<int> bc1nodes(Nuz, -1);
   for (int i = 0; i < bc1.size(); i++)
   {
      bc1nodes[bc1[i].first] = i; // В узле задано краевое
   }
   int k;
   for (int i = 0; i < Nuz; i++)
   {
      if (bc1nodes[i] != -1) //задано
      {
         di[i] = 1.0;
         F[i] = bc1[bc1nodes[i]].second;
         for (int j = ig[i]; j < ig[i + 1]; j++)
         {
            k = jg[j];
            if (bc1nodes[k] == -1)
            {
               F[k] -= gg[j] * F[i];
            }
            gg[j] = 0.0;
         }
      }
      else //не задано
      {
         for (int j = ig[i]; j < ig[i + 1]; j++)
         {
            k = jg[j];
            if (bc1nodes[k] != -1)
            {
               F[i] -= gg[j] * F[k];
               gg[j] = 0.0;
            }
         }
      }
   }
   F[0] = 10;
}

// Численное интегрирование

double mfe::Integration(ScalFunc2D f, double xk, double xk1, double yk, double yk1)
{
   double qi, qj, pi, pj, hx = 0, hy = 0;
   double sum = 0.0;
      for (int i = 0; i < 3; i++)
      {
         qi = weights[i];
         hx = abs(xk1 - xk);
         pi = (xk + xk1 + points[i] * hx) / 2.0;
         for (int j = 0; j < 3; j++)
         {
            qj = weights[j];
            hy = abs(yk1 - yk);
            pj = (yk + yk1 + points[j] * hy) / 2.0;
            sum += qi * qj * f(pi, pj);
         }
      }
      return sum * hx * hy / 4.0;
}

 //Решение СЛАУ
mvector mfe::mult(const mvector& v)
{
   mvector res(v.size(), 0);
   for (int i = 0; i < v.size(); i++)
   {
      res[i] = di[i] * v[i];
      for (int j = ig[i]; j < ig[i + 1]; j++)
      {
         res[i] += gg[j] * v[jg[j]];
         res[jg[j]] += gg[j] * v[i];
      }
   }
   return res;
}

// Локально-оптимальная схема
void mfe::LOS()
{
   q.resize(di.size(), 0);
   r.resize(di.size());
   double alpha, beta;
   mvector Ar;
   r = F - mult(q);
   z = r;
   p = mult(z);
   double rr = (r * r);
   for (int k = 0; k < maxIter && rr >= eps; k++)
   {
      double pp = (p * p);
      alpha = (p * r) / pp;
      q = q + alpha * z;
      rr = (r * r) - alpha * alpha * pp;
      r = r - alpha * p;
      Ar = mult(r);
      beta = -(p * Ar) / pp;
      z = r + beta * z;
      p = Ar + beta * p;
   }
   r.clear();
   p.clear();
   z.clear();
}
// Метод сопряженных градиентов
void mfe::MSG()
{
   q.resize(di.size(), 0);
   r.resize(di.size());
   double normF = sqrt(F * F);
   double alpha, beta;
   r = F - mult(q);
   z = r;
   double rr = (r * r);
   for (int k = 0; k < maxIter && sqrt(rr) / normF >= eps; k++)
   {
      alpha = (r * r) / (mult(z) * z);
      q = q + alpha * z;
      r = r - alpha * mult(z);
      beta = (r * r) / rr;
      rr = r * r;
      z = r + beta * z;
   }
   r.clear();
   z.clear();
}

//Найти значение функции в любой точке области

double mfe::FuncInPoint(double x, double y)
{
   int ielem = FindElem(x, y);

   double V = 0;
   for (int i = 0; i < 4; i++)
   {
       //cout << q.size() << "\t" << FE[ielem][i] << endl;
       //cout << FE[ielem].size() << "\t" << i << endl;
       //cout << FE.size() << "\t" << ielem << endl;
       if (FindElem(x, y) == -1)//если не найден, то просто 0, это костыль, надо бы пофиксить
       {
           break;
       }
       V += q[FE[ielem][i]] * psi(ielem, i, x, y);
      
   }
   return V;
}

// Найти конечный элемент по точке, котоарая лежит в его границах
int mfe::FindElem(double x, double y)
{
   int i = -1;
   for (int ielem = 0; ielem < FE.size(); ielem++)
   {
      double xk = MeshRZ[FE[ielem][0]].first;
      double yk = MeshRZ[FE[ielem][0]].second;
      double xk1 = MeshRZ[FE[ielem][3]].first;
      double yk1 = MeshRZ[FE[ielem][3]].second;
      if (x >= xk && x <= xk1 && y >= yk && y <= yk1)
      {
         i = ielem;
         break;
      }
   }
   return i;
}

// Записать результат в файл
void mfe::writeToFile()
{
   ofstream res("q.txt");
   if (res.is_open())
   {
      res.precision(15);
      for (int i = 0; i < q.size(); i++) 
      {
         res << q[i] << endl;
      }
      res.close();
   }
   else
      throw BAD_WRITE;
}

void mfe::toSpline()
{
   /* for (int i = 0; i < q.size(); i++)
    {
        cout << q[i] << endl;
    }*/
    int n;
    ifstream in("nodeRZ.txt");
    in >> n;
    double *x = new double[n];
    double *y = new double[n];
    for (int i = 0; i < n; i++)
    {
        in >> x[i] >> y[i];
    }
    in.close();
    ofstream out("input.txt");
    out << n << endl;
    for (int i = 0; i < n; i++)
    {
        out << x[i] << "\t" << FuncInPoint(x[i], y[i]) << endl;;
    }
    out.close();

    out.open("d_input.txt");
    out << (FuncInPoint(x[1], y[1]) - FuncInPoint(x[0], y[0])) / (x[1]-x[0]); // правая
    for (int i = 1; i < n-1; i++)
    {
        out << (FuncInPoint(x[i+1], y[i+1]) - FuncInPoint(x[i - 1], y[i - 1])) / (x[i + 1] - x[i - 1]) << endl;
    }
    out << (FuncInPoint(x[n-1], y[n-1]) - FuncInPoint(x[n-2], y[n-2])) / (x[n-1] - x[n-2]); //левая
    //fr = (f(x + h) - f(x)) / h; // правая
    //fc = (f(x + h) - f(x - h)) / (2 * h); // центральная
    //fl = (f(x) - f(x - h)) / h; // левая
}

void mfe::LU() {
   //копирование-инициализация
   di_n = di;
   ggl_n = gg;
   ggu_n = gg;

   for (int i = 0; i < di_n.size(); i++) {
      double sd = 0; //переменные суммирования

      int i0 = ig[i];
      int i1 = ig[i + 1];

      for (int k = i0; k < i1; k++) {
         int j = jg[k];
         double sl = 0, su = 0;
         int j0 = ig[j];
         int j1 = ig[j + 1];
         int ki = i0;
         int kj = j0;

         for (; ki < k && kj < j1;) {
            int jl = jg[ki];
            int ju = jg[kj];
            if (jl == ju) {
               sl += ggu_n[kj] * ggl_n[ki];
               su += ggl_n[kj] * ggu_n[ki];
               ki++; kj++;
            }
            else if (jl < ju) ki++;
            else kj++;
         }
         ggu_n[k] = (ggu_n[k] - su) / di_n[j];
         ggl_n[k] = (ggl_n[k] - sl) / di_n[j];
         sd += ggu_n[k] * ggl_n[k];
      }

      di_n[i] = sqrt(di_n[i] - sd);
   }
}


void mfe::multT(vector<double>& res, vector<double>& vec)
{
   for (int i = 0; i < res.size(); i++)
   {
      res[i] = di[i] * vec[i];
      for (int j = ig[i]; j < ig[i + 1]; j++)
      {
         res[i] += gg[j] * vec[jg[j]];
         res[jg[j]] += gg[j] * vec[i];
      }
   }
}

// y = L^-1 * b
void mfe::mult_direct(vector<double>& ggl, vector<double>& di, vector<double>& res, vector<double>& b)
{
   for (int i = 0; i < res.size(); i++)
   {
      double sum = 0;
      for (int j = ig[i]; j < ig[i + 1]; j++)
         sum += ggl[j] * res[jg[j]];
      res[i] = (b[i] - sum) / di[i];
   }
}
// x = U^-1 * y
void mfe::mult_reverse(vector<double>& ggu, vector<double>& di, vector<double>& res, vector<double>&b)
{
   res = b;
   for (int i = res.size() - 1; i >= 0; i--)
   {
      res[i] /= di[i];
      for (int j = ig[i + 1] - 1; j >= ig[i]; j--)
         res[jg[j]] -= res[i] * ggu[j];
   }
}

void mfe::LOS_LU()
{
   double skal1, skal2;
   LU();
   mvector temp1(q.size());
   mvector temp2(q.size());
   mvector z_sl(q.size());
   r.resize(di.size());
   p.resize(di.size());
   
   //	инициализация
   temp1 = mult(q);

   temp2 = F - temp1;
   mult_direct(ggl_n, di_n, r, temp2);
   mult_reverse(ggu_n, di_n, z_sl, r);

   temp1 = mult(z_sl);
   mult_direct(ggl_n, di_n, p, temp1);

   //iteration
   double nev = sqrt(r*r);
   int iter;
   for (iter = 0; iter < maxIter && nev > eps; iter++)
   {
      skal1 = p * r;
      skal2 = p * p;

      double alpha = skal1 / skal2;

      q = q + alpha * z_sl;
      r = r - alpha * p;
      mult_reverse(ggu_n, di_n, temp1, r);
      temp2 = mult(temp1);
      mult_direct(ggl_n, di_n, temp1, temp2);

      skal1 = p * temp1;

      double beta = -skal1 / skal2;

      mult_reverse(ggu_n, di_n, temp2, r);

      z_sl = temp2 + beta * z_sl;
      p = temp1 + beta * p;

      nev = sqrt(r*r);
   }
   r.clear();
   p.clear();
}
