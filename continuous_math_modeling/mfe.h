#pragma once
#ifndef MFE_H
#define MFE_H
#include <vector>
#include <functional>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

typedef vector <pair<double, double>> grid;
typedef vector<int> pvector;
typedef vector<double> mvector;
typedef vector<vector<double>> matrix;
typedef vector<vector<int>> finiteElem;
typedef function<double(double, double) > ScalFunc2D;
enum exceptions { BAD_READ, OUT_OF_AREA, BAD_WRITE };
class mfe
{
   struct material
   {
      double h0;
      double h1;
      double sigma;
   };
   struct _bc2
   {
      int n_i;
      int loc_node1_i;
      int loc_node2_i;
      double Tetta_i;
   };
   struct Field // Области с различными параметрами
   {
      double x0, x1, y0, y1;
      int num_mat;
      Field(double x0, double x1, double y0, double y1, int num_mat) : x0(x0), x1(x1), y0(y0), y1(y1), num_mat(num_mat) {};
   };
public:
   mfe();
   double test_func(double x, double y);
   //========================================================================
   void Compute(mvector sigma);
   void PointsToPlot(); // Не нужно
   int FindMaterial(int ielem); // Определяет сигму на кэ
   int FindElem(double x, double y); // Определяет кэ по координатам
   void buildPortraitOfMatrix(); // Строит портрет слау
   void buildLocalG(int ielem, double hx, double hy, int num_mat); // Формирует локальную матрицу жесткости
   void addElementToGlobal(int i, int j, double elem); // Добавляет локальный элемент в глобальную слау
   void assemblyGlobalMatrix(mvector sigma); // Сборка глобальной матрицы
   void toDense(const string _dense); // Не нужно
   double psi(int ielem, int func, double x, double y); // Локальные базисные функции
   double d_psi(int ielem, int i, double x, double y, int var); // Производная базисной функции
   void bc_1(); // установка первых краевых
   double FuncInPoint(double x, double y); // Значение напряжения в точке области
   void toSpline();
   double divgrad(double x, double y); // Не нужно
   //========================================================================
   mvector mult(const mvector& v);
   double Integration(ScalFunc2D f, double r1, double r2, double z1, double z2);
   void LOS();
   void LOS_LU();
   void LU();
   void MSG();
   void writeToFile();
   void multT(vector<double>& res, vector<double>& vec);
   void mult_reverse(vector<double>& ggu, vector<double>& di, vector<double>& res, vector<double>& b);
   void mult_direct(vector<double>& ggl, vector<double>& di, vector<double>& res, vector<double>& b);
   //========================================================================

   vector<Field> Fields;
   vector<material> Materials;
   double qr, qz; // Массивы с коэффициентами разрядки
   matrix G; // Локальная матрица жесткости
   mvector b; // Локальный вектор правой части
   mvector q; // Вектор весов
   mvector F; // Вектор правой части
   vector<pair<double, double>> point; // Точка, в которой необходимо получить значение искомой функции
   matrix G_odn, M_odn; // Одномерные матрицы жёсткости и массы для составления локальной матрицы жёсткости
   int KolR, KolZ, Nuz; // Размер сетки
   grid MeshRZ; // Сетка
   int Nel; // Количество КЭ
   finiteElem FE; // Конечные элементы
   finiteElem L; // Глобальные функции
   // 1-ые краевые
   int Nbc1;
   vector <pair<int, double>> bc1;
   // 2-краевые
   int Nbc2;
   vector<_bc2> bc2;
   // Глобальная матрица
   mvector di;
   mvector gg;
   pvector ig;
   pvector jg;
   matrix mat;
   bool isOrdered(const pvector& v);
   mvector r;
   mvector z;
   mvector p;
   mvector r0;
   mvector _y;
   int maxIter;
   double eps = 1e-10;
   double pi = 3.14159265;
   double Mu0 = 4 * pi * 1e-7;
   double eps_nel;
   int maxIterNel;
   double eps_q = 1e-4;
   mvector points;
   mvector weights;
   mvector ggl_n;
   mvector ggu_n;
   mvector di_n;
};
inline mvector operator+(const mvector& a, const mvector& b)
{
   mvector res = a;
   for (int i = 0; i < res.size(); i++)
      res[i] += b[i];
   return res;
}
inline mvector operator-(const mvector& a, const mvector& b)
{
   mvector res = a;
   for (int i = 0; i < res.size(); i++)
      res[i] -= b[i];
   return res;
}
inline double operator*(const mvector& a, const mvector& b)
{
   double scalar = 0.0;
   for (int i = 0; i < a.size(); i++)
      scalar += a[i] * b[i];
      return scalar;
}
inline mvector operator*(double c, const mvector& a)
{
   vector<double> res = a;
   for (int i = 0; i < res.size(); i++)
      res[i] *= c;
   return res;
}
#endif