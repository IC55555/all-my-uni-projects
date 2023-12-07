#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

double sigma = 0.1;
//double sigma0 = 0.01;
double alpha;
vector<double> I = { 1.,1.,1. };
//vector<double> I_true = { 1.,1.,1. };
vector<double> I_true = { 1.,2.,3. };
vector<double> V_true(3);
vector<vector<double>> Matrix(3, vector<double>(3));
vector<double> vector_B(3);
vector<double> omega(3);
vector<double> res(3);
double delta_I = 1e-05;
vector<double> I_new(3);
vector<double> I_cur(3);
vector<double> rx, zy;
int n, m;
int nr, nz; //число разбиений(отрезочков)

//краевые
int ncond1;
vector<vector<int>> cond1;

vector<double> local_cond;
vector<double> local_cond_vl;

//матрицы и вектора
vector<vector<double>> G;//локальные матрицы в плотном формате
vector<vector<double>> C;
vector<vector<double>> local;
vector<double> b_loc;
vector<double> f;//вспомогательный, значения функции в координатах
int s_true, p_true;

vector<vector<double>> G_small, M_small_r, M_small_h;

vector<double> r_loc, z_loc;//координаты локальных узлов каждого элемента

vector<double> ggl, di, ggu, ggl_new, di_new, ggu_new;
vector<int> ig, jg;
vector<double> b; //глобальная пр часть
vector<double> q; //истинное решение

vector<int> global_num;

//slae
vector<double> r, p, z, temp1, temp2;

vector<vector<double>> source(6, vector<double>(3)), receiver(6, vector<double>(3));

vector<double> operator-(vector<double>vec1, vector<double>vec2)
{
   vector < double >res(3);
   for (int i = 0; i < 3; i++)
   {
      res[i] = vec1[i] - vec2[i];
   }
   return res;
}


struct point
{
   double x, y, z;
};

struct receiver
{
   point M, N;
};

struct source
{
   point A, B;
};



double R(point A, point B)
{
   double x = A.x - B.x;
   double y = A.y - B.y;
   double z = A.z - B.z;
   double l = sqrt(x * x + y * y + z * z);
   return l;
}

point get_point_from_source(vector<double> V)
{
   point A;
   A.x = V[0];
   A.y = V[1];
   A.z = V[2];
   return A;
}

double potential_dif(double I, point A, point B, point M, point N)
{
   double V = I / (2 * sigma * M_PI) * ((1 / R(M, B) - 1 / R(M, A)) - (1 / R(N, B) - 1 / R(N, A)));
   return V;
}

double potential_for_receiver(int i, double I1, double I2, double I3)
{
   //point M1, N1, M2, N2, M3, N3, A1, B1, A2, B2, A3, B3;

   //один приемник от трех источников
   double v1, v2, v3;
   v1 = potential_dif(I1, get_point_from_source(source[0]), get_point_from_source(source[1]), get_point_from_source(receiver[2 * i]), get_point_from_source(receiver[2 * i + 1]));
   v2 = potential_dif(I2, get_point_from_source(source[2]), get_point_from_source(source[3]), get_point_from_source(receiver[2 * i]), get_point_from_source(receiver[2 * i + 1]));
   v3 = potential_dif(I3, get_point_from_source(source[4]), get_point_from_source(source[5]), get_point_from_source(receiver[2 * i]), get_point_from_source(receiver[2 * i + 1]));
   return v1 + v2 + v3;
}


int gauss(vector<vector<double>> a, vector<double> y, int n, vector<double> &solve)
{
	
   double max;
   int k, index;
   const double eps = 1e-15;  // точность 
   k = 0;
   while (k < n)
   {
      //Поиск строки с максимальным a[i][k] 
      max = abs(a[k][k]);
      index = k;
      for (int i = k + 1; i < n; i++)
      {
         if (abs(a[i][k]) > max)
         {
            max = abs(a[i][k]);
            index = i;
         }
      }

      //Проверка на наличие ненулевых элементов в столбце ? ? 
      if (max <= eps)
      {
         cout << "Решение получить невозможно из-за нулевого столбца ";
         cout << index << " матрицы A" << endl;
         return 1;
      }

      //Перестановка строк 
      for (int j = 0; j < n; j++)
      {
         double temp = a[k][j];
         a[k][j] = a[index][j];
         a[index][j] = temp;
      }

      double temp = y[k];
      y[k] = y[index];
      y[index] = temp;
		//cout << "temp= " << temp << endl;
      //Нормализация уравнений 
      for (int i = k; i < n; i++) 
      {
         double temp = a[i][k];
         if (abs(temp) > eps) { // для нулевого коэффициента пропустить 
            for (int j = 0; j < n; j++) 
            {
               a[i][j] = a[i][j] / temp;
            }

            y[i] = y[i] / temp;
            if (i != k) { // уравнение не вычитать само из себя 
               for (int j = 0; j < n; j++) 
               {
                  a[i][j] = a[i][j] - a[k][j];
               }
               y[i] = y[i] - y[k];
            }
         }
      }
      k++;
   }
	cout << setprecision(15) << "y; " << y[0] << "  " << y[1] << "  " << y[2] << endl;
   //обратный ход 
   for (k = n - 1; k >= 0; k--)
   {
      solve[k] = y[k];
      for (int i = 0; i < k; i++) 
      {
         y[i] = y[i] - a[i][k] * solve[k];
      }
   }
   return 0;
}


vector<double> MultMV(vector<vector<double>> &Matrix, vector<double> &vec)
{
   vector<double>temp(3);
   for (int i = 0; i < 3; i++)
   {
      for (int j = 0; j < 3; j++)
      {
         temp[i] += Matrix[i][j] * vec[j];
      }
   }
   return temp;
}

double norm(vector<double>vec)
{
   double sum = 0;
   for (int i = 0; i < vec.size(); i++)
   {
      sum += vec[i] * vec[i];
   }
   return sqrt(sum);
}

double nev(vector<double> z)
{
   vector<double> Az;
   Az = MultMV(Matrix, z);
   Az = Az - vector_B;
   return norm(Az);
}

vector<double> get_vector(point A)
{
   vector<double> V(3);
   V[0] = A.x;
   V[1] = A.y;
   V[2] = A.z;

   return V;
}


void init(point M1, point N1, point M2, point N2, point M3, point N3, point A1, point B1, point A2, point B2, point A3, point B3)
{
   source[0] = get_vector(A1);
   source[1] = get_vector(B1);
   source[2] = get_vector(A2);
   source[3] = get_vector(B2);
   source[4] = get_vector(A3);
   source[5] = get_vector(B3);

   receiver[0] = get_vector(M1);
   receiver[1] = get_vector(N1);
   receiver[2] = get_vector(M2);
   receiver[3] = get_vector(N2);
   receiver[4] = get_vector(M3);
   receiver[5] = get_vector(N3);
}

void read_file()
{
   //string filename = "input.txt";
   ifstream in("input.txt");
   point M1, N1, M2, N2, M3, N3, A1, B1, A2, B2, A3, B3;

	in >> M1.x >> M1.y >> M1.z;
	in >> N1.x >> N1.y >> N1.z;

	in >> M2.x >> M2.y >> M2.z;
	in >> N2.x >> N2.y >> N2.z;

	in >> M3.x >> M3.y >> M3.z;
	in >> N3.x >> N3.y >> N3.z;

	in >> A1.x >> A1.y >> A1.z;
	in >> B1.x >> B1.y >> B1.z;

	in >> A2.x >> A2.y >> A2.z;
	in >> B2.x >> B2.y >> B2.z;

	in >> A3.x >> A3.y >> A3.z;
	in >> B3.x >> B3.y >> B3.z;

   init(M1, N1, M2, N2, M3, N3, A1, B1, A2, B2, A3, B3);
	in.close();
}



double R(double &alpha)
{
	vector<vector<double>>A(3, vector<double>(3));
	vector<double>z(3);
	A = Matrix;
	for (int i = 0; i < 3; i++)
		A[i][i] += alpha;
	gauss(A, vector_B, 3, z);
	return nev(z);
}



double func()
{
	return 0;
}

int get_global(int s, int p, int j) //получение глобального номера по локальному
{ 
	int k;
	switch (j) 
	{
	case 0:
	{
		k = (s)*nr + p;
		break;
	}
	case 1:
	{
		k = (s)*nr + p + 1;
		break;
	}

	case 2:
	{
		k = (s + 1) * nr + p;
		break;
	}
	case 3: 
	{
		k = (s + 1) * nr + p + 1;
		break;
	}
	}
	return k;
}

double get_Ug(int num_first, double x, double y) 
{
	return 0;
}



void make_local_coor(int s, int p) 
{
	double xp, xp_next, ys, ys_next;
	xp = rx[p];
	xp_next = rx[p + 1];
	ys = zy[s];
	ys_next = zy[s + 1];

	r_loc[0] = xp;
	r_loc[1] = xp_next;

	z_loc[0] = ys;
	z_loc[1] = ys_next;
}

void make_edges_coor(int type, int s, int p) 
{
	double xp, xp_next, ys, ys_next, xp_mid, ys_mid;
	if (type == 1) 
	{
		xp = rx[p];
		xp_next = rx[p + 1];
		xp_mid = (xp + xp_next) / 2;
		r_loc[0] = xp;
		r_loc[1] = xp_mid;
		r_loc[2] = xp_next;
		z_loc[0] = zy[s];
	}
	if (type == 2) 
	{
		ys = zy[s];
		ys_next = zy[s + 1];
		ys_mid = (ys + ys_next) / 2;

		z_loc[0] = ys;
		z_loc[1] = ys_mid;
		z_loc[2] = ys_next;
		r_loc[0] = rx[p];
	}
}

void make_local_matrix(double hr, double hz, double rp)
{
	for (int i = 0; i < 4; i++) 
	{
		for (int j = 0; j <= i; j++) 
		{

			int mu_i = (i % 2);
			int mu_j = (j % 2);
			int nu_i = ((int)(i / 2));
			int nu_j = ((int)(j / 2));


			G[i][j] = sigma * ((rp + hr / 2) / hr * G_small[mu_i][mu_j] * hz / 6 * M_small_r[nu_i][nu_j] + hr / 6 * (rp * M_small_r[mu_i][mu_j] + hr / 2 * M_small_h[mu_i][mu_j]) * 1 / hz * G_small[nu_i][nu_j]);
			// Gr[mu_i][mu_j] * Mz[nu_i][nu_j] + Mr[mu_i][mu_j] * Gz[nu_i][nu_j]


			local[i][j] = G[i][j];
			if (i != j)
			{
				G[j][i] = G[i][j];
				local[j][i] = local[i][j];
			}
		}
	}

}
void make_local_vec()
{
	for (int i = 0; i < 4; i++)
	{
		b_loc[i] = 0;
	}
}

void add_local(int s, int p)
{
	for (int i = 0; i < 4; i++)
		global_num[i] = get_global(s, p, i);

	for (int i = 0; i < 4; i++)
	{
		di[global_num[i]] += local[i][i];
		int beg = ig[global_num[i]];
		for (int j = 0; j < i; j++, beg++)
		{
			int end = ig[global_num[i] + 1] - 1;
			int ind = beg;
			while (jg[ind] != global_num[j])
			{
				ind++;
			}
			ggl[ind] += local[i][j];
			ggu[ind] = ggl[ind];
		}
		b[global_num[i]] += b_loc[i];
	}


}

void mult(vector<double> &MV, vector<double> vec)
{
	for (int i = 0; i < m; i++)
	{
		int k0 = ig[i];
		int k1 = ig[i + 1];
		MV[i] = di[i] * vec[i];
		for (int k = k0; k < k1; k++)
		{
			int j = jg[k];
			MV[i] += vec[j] * ggl[k];
			MV[j] += vec[i] * ggu[k];
		}
	}
}
double skal_mult(vector<double> vec1, vector<double> vec2) 
{
	double s = 0;
	for (int i = 0; i < m; i++)
	{
		s += vec1[i] * vec2[i];
		cout << "vec1 = " << vec2[i] << endl;
	}
	return s;
}

void mult_pr(vector<double> aa, vector<double> &y, vector<double> bb)
{

	for (int i = 0; i < m; i++) 
	{
		double s = 0; //переменные суммирования

		int i0 = ig[i];//индекс 1го элемента в iтой строке
		int i1 = ig[i + 1];

		for (int k = i0; k < i1; k++) 
		{
			int j = jg[k];
			s += y[j] * aa[k];
		}
		y[i] = (bb[i] - s) / di[i];
	}
}
void mult_obr(vector<double> aa, vector<double> &y, vector<double> bb) 
{
	for (int i = 0; i < m; i++)
		y[i] = bb[i];
	for (int i = m - 1; i >= 0; i--)
	{
		int i0 = ig[i];//индекс 1го элемента в iтой строке
		int i1 = ig[i + 1];

		y[i] /= di[i];

		for (int k = i1 - 1; k >= i0; k--) 
		{
			int j = jg[k];
			y[j] -= y[i] * aa[k];
		}
	}
}
void multU(vector<double> &MV, vector<double>vec) 
{
	for (int i = 0; i < m; i++) 
	{
		int k0 = ig[i];
		int k1 = ig[i + 1];
		MV[i] = di[i] * vec[i];
		for (int k = k0; k < k1; k++) 
		{
			int j = jg[k];
			//MV[i] += vec[j] * al[k];
			MV[j] += vec[i] * ggu_new[k];
		}
	}
}
void mult_tr(vector<double> &MV, vector<double> vec) 
{
	for (int i = 0; i < m; i++) 
	{
		int k0 = ig[i];
		int k1 = ig[i + 1];
		MV[i] = di[i] * vec[i];
		for (int k = k0; k < k1; k++) 
		{
			int j = jg[k];
			MV[i] += vec[j] * ggu[k];
			MV[j] += vec[i] * ggl[k];
		}
	}
}


void regular()
{
	double alpha_pred, alpha_next;
	alpha_pred = 0;
	alpha_next = 1e-15;
	//ищем альфу, при которой решается
	do
	{
		for (int i = 0; i < 3; i++)
		{
			Matrix[i][i] -= alpha_pred;
			Matrix[i][i] += alpha_next;
		}
		alpha_pred = alpha_next;
		alpha_next *= 1.2;
	} while (gauss(Matrix, vector_B, 3, res) == 1);

	//ищем лучшую альфу

	double first_nev = R(alpha_pred);
	double cur_nev;
	alpha_next = alpha_pred * 1.2;

	do
	{
		for (int i = 0; i < 3; i++)
		{
			Matrix[i][i] -= alpha_pred;
			Matrix[i][i] += alpha_next;
		}
		cur_nev = R(alpha_next);
		alpha_pred = alpha_next;
		alpha_next *= 1.2;

	} while (cur_nev / first_nev < 2);
	//СРАВНИТЬ ФУНКЦИОНАЛЫ

	alpha = alpha_pred / 1.2;

}


//double potential(double I, vector<double>A, vector<double>B, vector<double>M, vector<double>N)
//{
//	//один источник с одним приемников
//	double rbm = distance(B, M);
//	double ram = distance(A, M);
//	double rbn = distance(B, N);
//	double ran = distance(A, N);
//	return I / (2 * PI * sigma) * ((1. / rbm - 1. / ram) - (1. / rbn - 1. / ran));
//}



void find_ke(double kr, double kz)
{
	for (int s = 0; s < rx.size() - 1; s++) 
	{
		if (rx[s] <= kr && kr <= rx[s + 1])
		{
			s_true = s;//номер интервала по х
			break;
		}
	}
	for (int p = 0; p < zy.size() - 1; p++)
	{
		if (zy[p] <= kz && kz <= zy[p + 1])
		{
			p_true = p;//номер интервала по y
			break;
		}
	}
}

double func_in_point(double kr, double kz)
{
	find_ke(kr, kz);
	double f1, f2, f3, f4;
	double q1, q2, q3, q4;
	double r1, r2, z1, z2;
	/*cout << kr << endl;
	cout << kz << endl;
	cout << rx[s_true] << endl;
	cout << rx[s_true + 1] << endl;*/
	double ksi, kso;
	ksi = (kr - rx[s_true]) / (rx[s_true + 1] - rx[s_true]);///tyt
	kso = (kz - zy[p_true]) / (zy[p_true + 1] - zy[p_true]);

	r1 = 1 - ksi;
	r2 = ksi;
	z1 = 1 - kso;
	z2 = kso;

	f1 = r1 * z1;
	f2 = r2 * z1;
	f3 = r1 * z2;
	f4 = r2 * z2;

	q1 = q[get_global(p_true, s_true, 0)];
	q2 = q[get_global(p_true, s_true, 1)];
	q3 = q[get_global(p_true, s_true, 2)];
	q4 = q[get_global(p_true, s_true, 3)];

	double v = q1 * f1 + q2 * f2 + q3 * f3 + q4 * f4;
	return v;
}

double func_V_s_r(double sx, double sy, double rx, double ry, int sign, double I) 
{
	double dx = rx - sx;
	double dy = ry - sy;
	double dr = sqrt(dx * dx + dy * dy);
	double a = func_in_point(dr, zy[zy.size() - 1]);
	return a * sign * I;
}

double func_V_s2_r(double sAx, double sAy, double sBx, double sBy, double rx, double ry, double I)
{
	double V_rA = func_V_s_r(sAx, sAy, rx, ry, -1, I);
	double V_rB = func_V_s_r(sBx, sBy, rx, ry, 1, I);
	return V_rA + V_rB;
}

double func_V_s2_r2(int n_sour, int n_rec, vector<double> &Ii)
{

	double V_M = func_V_s2_r(source[2 * n_sour][0], source[2 * n_sour][1], source[2 * n_sour + 1][0], source[2 * n_sour + 1][1], receiver[2 * n_rec][0], receiver[2 * n_rec][1], Ii[n_sour]);
	double V_N = func_V_s2_r(source[2 * n_sour][0], source[2 * n_sour][1], source[2 * n_sour + 1][0], source[2 * n_sour + 1][1], receiver[2 * n_rec + 1][0], receiver[2 * n_rec + 1][1], Ii[n_sour]);

	return V_M - V_N;
}


double potential_for_receiver_mke(int i, vector<double> &Ii)
{
   //один приемник от трех источников
   double v1, v2, v3;
   v1 = func_V_s2_r2(0, i, Ii);
   v2 = func_V_s2_r2(1, i, Ii);
   v3 = func_V_s2_r2(2, i, Ii);
   return v1 + v2 + v3;
}

void make_matrix(int flag)
{
	flag = 0;
   for (int q = 0; q < 3; q++) 
	{
      for (int s = 0; s < 3; s++) 
		{

         Matrix[q][s] = 0;
         //сумма по приемникам
         for (int i = 0; i < 3; i++) 
			{
            double s_q = 0, s_s = 0;
            //аналитические производные
            /*s_q = potential(1., source[2 * q], source[2 * q + 1], receiver[2 * i], receiver[2 * i + 1]);
            s_s = potential(1., source[2 * s], source[2 * s + 1], receiver[2 * i], receiver[2 * i + 1]);
            */

            //численные производные
            //производная по q-му параметру

            double V_cur, V_new;

            if (flag == 0)
               V_cur = potential_for_receiver(i, I[0], I[1], I[2]);
            else
               V_cur = potential_for_receiver_mke(i, I);

            I_new = I;
            I_new[q] += delta_I;

            if (flag == 0)
               V_new = potential_for_receiver(i, I_new[0], I_new[1], I_new[2]);
            else
               V_new = potential_for_receiver_mke(i, I_new);

            s_q = (V_new - V_cur) / delta_I;

            //производная по s-му параметру
            I_new = I;
            I_new[s] += delta_I;

            if (flag == 0)
               V_new = potential_for_receiver(i, I_new[0], I_new[1], I_new[2]);
            else
               V_new = potential_for_receiver_mke(i, I_new);

            s_s = (V_new - V_cur) / delta_I;

            Matrix[q][s] += omega[i] * omega[i] * s_q * s_s;
         }

      }
   }
}

void make_vector(int flag)
{
	flag = 0;
	for (int q = 0; q < 3; q++) 
	{
		vector_B[q] = 0;
		for (int i = 0; i < 3; i++) 
		{
			double s_q = 0;
			//аналитическая производная по I[q]
			//s_q = potential(1., source[2 * q], source[2 * q + 1], receiver[2 * i], receiver[2 * i + 1]);
			//численная производная по I[q]
			double V_cur, V_new;

			if (flag == 0)
				V_cur = potential_for_receiver(i, I[0], I[1], I[2]);
			else
				V_cur = potential_for_receiver_mke(i, I);

			I_new = I;
			I_new[q] += delta_I;

			if (flag == 0)
				V_new = potential_for_receiver(i, I_new[0], I_new[1], I_new[2]);
			else
				V_new = potential_for_receiver_mke(i, I_new);

			s_q = (V_new - V_cur) / delta_I;
			//V_cur = potential_for_receiver(i, I[0], I[1], I[2]);
			double dv = -V_true[i] + V_cur;
			vector_B[q] -= omega[i] * omega[i] * s_q * dv;
		}
	}
}

void straight_1()
{
	V_true[0] = potential_for_receiver(0, I_true[0], I_true[1], I_true[2]);
	V_true[1] = potential_for_receiver(1, I_true[0], I_true[1], I_true[2]);
	V_true[2] = potential_for_receiver(2, I_true[0], I_true[1], I_true[2]);

	omega[0] = 1. / V_true[0];
	omega[1] = 1. / V_true[1];
	omega[2] = 1. / V_true[2];
	cout << "V_true:  " << V_true[0] << " " << V_true[1] << " " << V_true[2] << endl;
}

void back_1()
{
	make_matrix(0);
	make_vector(0);
	//Решение СЛАУ

	//запуск Гаусса с исходной матрицей
	if (gauss(Matrix, vector_B, 3, res) == 1)
	{
		regular();
	}

	cout << "res: ";

	for (int i = 0; i < 3; i++)
		cout << res[i] << " ";
	cout << endl;

	cout << "sum: ";

	for (int i = 0; i < 3; i++)
		cout << res[i] + I[i] << " ";
	cout << endl;
}


void straight_2()
{
   //double a = func_in_point(100, 100);
   //потенциалы на трёх приёмниках
   V_true[0] = potential_for_receiver_mke(0, I_true);
   V_true[1] = potential_for_receiver_mke(1, I_true);
   V_true[2] = potential_for_receiver_mke(2, I_true);

   omega[0] = 1. / V_true[0];
   omega[1] = 1. / V_true[1];
   omega[2] = 1. / V_true[2];
}

void back_2()
{
   make_matrix(1);
   make_vector(1);
   //Решение СЛАУ

   //запуск Гаусса с исходной матрицей
   if (gauss(Matrix, vector_B, 3, res) == 1)
   {
      regular();
   }
	
   cout << "I:" << endl;
   for (int i = 0; i < 3; i++)
      cout << res[i] << " ";
   cout << endl << endl;
	
}

void main()
{
   read_file();
   I_cur = I_true;
   //mke();
   //PrintMKESolution();

   straight_1();
   back_1();
	cout << "sigma = " << sigma;
   //straight_2();
   //back_2();
	system("pause");
}
