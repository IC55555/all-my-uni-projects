#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <set>

using namespace std;

class SLAE 
{
public:
	int global_flag = 0;
	int n_x; //узлы областей
	int n_y;//узлы областей
	int count_x, count_y, count; //узлы областей + внутренние
	int max_it = 1000;
	double eps = 1e-10;
	vector<double> ax, ay; //узлы областей
	vector<double> x, y; //узлы c внутренними
	vector<int> oxn, oyn; //вектор для подсчета текущего количества отрезков
	vector<int> nx, ny; // число отрезков между двумя координатами
	vector<double> qx, qy; //коэффицинт разрядки (если равны 1, то сетка равномерная)
	int m; //количество узлов
	double w = 1;//параметр релаксации

	//краевые
	int ncond1;
	vector<int> cond1;
	//матрицы и вектора
	vector<vector<double>> G;//локальные матрицы в плотном формате
	vector<vector<double>> M;
	vector<vector<double>> C;
	vector<double> b_loc;
	vector<double> Ggg;//глобальные матрицы в разреженном формате
	vector<double> Mgg;
	vector<double> Gdi;
	vector<double> Mdi;
	vector<double> Adi;
	vector<double> Agg;
	vector<double> Adi_new;
	vector<double> Agu_new;
	vector<double> Agl_new;
	vector<double> ig;//портрет
	vector<double> jg;//портрет
	vector<double> b; //глобальная пр часть(промежуточная часть)
	vector<double> d; //глобальная пр часть
	vector<double> p_prev; //предыдущее решение по нелинейности
	vector<double> p_next; //следующее решение по нелинейности
	vector<double> q_cur; //текущее решение по времени
	vector<double> q_next; //следующее решение по времени
	vector<double> temp1, temp2, temp3;
	vector<double> r, z, p;
	int count_t, ntimes;
	vector<double> times, at, nt, qt;
	double k;

	SLAE(string net_file, string cond_file_1, string time_file) 
	{
		read_net(net_file); //чтение из файла
		init_net(); //инициализация сетки
		read_cond(cond_file_1); //считывание сначала количества, потом самих краевых условий
		read_time(time_file);//считываем время
		init_time(); //время
		resize_func(); //просто функция с ресайзами...
	}

	~SLAE() //дструктор Слау
	{
		ax.~vector();
		x.~vector();
		nx.~vector();
		qx.~vector();
		cond1.~vector();

		for (int i = 0; i < 2; i++) 
		{
			G[i].~vector();
			M[i].~vector();
			C[i].~vector();
		}

		G.~vector();
		M.~vector();
		C.~vector();
		b_loc.~vector();

		Ggg.~vector();
		Mgg.~vector();
		Gdi.~vector();
		Mdi.~vector();
		Adi.~vector();
		Agg.~vector();

		Adi_new.~vector();
		Agu_new.~vector();
		Agl_new.~vector();

		ig.~vector();
		jg.~vector();
		b.~vector();
		d.~vector();

		p_prev.~vector();
		p_next.~vector();
		q_cur.~vector();
		q_next.~vector();

		temp1.~vector();
		temp2.~vector();
		temp3.~vector();
		r.~vector();
		z.~vector();
		p.~vector();

		times.~vector();
		at.~vector();
		nt.~vector();
		qt.~vector();
	}

	void make_q(vector<double> &MV, double t) 
	{
		for (int i = 0; i < m; i++) 
		{
			MV[i] = get_Ug(x[i], t);
		}
	}

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
	
		m = count_x;
	}

	void init_net() 
	{
		double hx, hy, tx, ty;
		for (int j = 0; j < n_x - 1; j++)
		{
			//обработка по х
			if (qx[j] != 1)
				hx = (ax[j + 1] - ax[j]) * (1. - qx[j]) / (1. - pow(qx[j], nx[j]));
			else hx = (ax[j + 1] - ax[j]) / nx[j];

			tx = ax[j];

			for (int k = 0; k < nx[j]; k++) 
			{
				x.push_back(tx);
				tx += hx;
				hx *= qx[j];
			}
		}
		x.push_back(tx);
	}

	void read_cond(string cond_file_1) 
	{
		ifstream fin1(cond_file_1 + ".txt");
		fin1 >> ncond1;
		cond1.resize(ncond1);
		for (int i = 0; i < ncond1; i++) 
		{
			fin1 >> cond1[i];
		}
		fin1.close();
	}

	void read_time(string time_file) 
	{
		ifstream fin1(time_file + ".txt");
		fin1 >> ntimes;
		at.resize(ntimes);
		nt.resize(ntimes);
		qt.resize(ntimes);

		for (int i = 0; i < ntimes; i++)
			fin1 >> at[i];

		for (int i = 0; i < ntimes - 1; i++)
			fin1 >> nt[i];
		for (int i = 0; i < ntimes - 1; i++)
			fin1 >> qt[i];

		count_t = 1;
		for (int i = 0; i < ntimes - 1; i++)
			count_t += nt[i];

		fin1.close();
	}

	void init_time() 
	{
		double ht, tt;
		for (int j = 0; j < ntimes - 1; j++) 
		{
			//обработка по t
			if (qt[j] != 1)
				ht = (at[j + 1] - at[j]) * (1. - qt[j]) / (1. - pow(qt[j], nt[j]));
			else ht = (at[j + 1] - at[j]) / nt[j];

			tt = at[j];

			for (int k = 0; k < nt[j]; k++) 
			{
				times.push_back(tt);
				tt += ht;
				ht *= qt[j];
			}
		}
		times.push_back(tt);
	}

	void resize_func()
	{
		G.resize(2);
		M.resize(2);
		C.resize(2);

		for (int i = 0; i < 2; i++) 
		{
			G[i].resize(2);
			M[i].resize(2);
			C[i].resize(2);
		}
		b_loc.resize(2);

		Ggg.resize(m - 1);
		Gdi.resize(m);
		Mgg.resize(m - 1);
		Mdi.resize(m);
		Agg.resize(m - 1);
		Adi.resize(m);

		Adi_new.resize(m);
		Agu_new.resize(m - 1);
		Agl_new.resize(m - 1);

		ig.resize(m + 1);
		jg.resize(m - 1);
		b.resize(m);
		d.resize(m);

		p_prev.resize(m);
		p_next.resize(m);
		q_cur.resize(m);
		q_next.resize(m);

		temp1.resize(m);
		temp2.resize(m);
		temp3.resize(m);
		r.resize(m);
		z.resize(m);
		p.resize(m);
	}

	double get_lyam(double hx, double x, int p, vector<double>current)  //подсчет лямбды
	{
		double arg = (current[p + 1] - current[p]) / hx;
		return arg * arg * arg;
	}

	//заранее подготовленные значения под переменные
	double get_sigma(double hx) //сигма
	{
		return 1;
	}

	double get_f(double x, double t)  //функция f(x)
	{
		return -64 * x * x * x;
	}

	double get_Ug(double x, double t) //возведение в квадрат
	{
		return x * x;
	}

	void make_portrait() //создаем портрет
	{
		ig[0] = 0;
		ig[1] = 0;
		int k = 1;
		for (int i = 2; i < m + 1; i++, k++)
		{
			ig[i] = k;
			jg[i - 2] = i - 2;
		}
	}

	void make_local_G(int p, vector<double>current) //создание окальной матрицы G в плотном формате
	{
		double hx = x[p + 1] - x[p];
		double lyam1 = get_lyam(hx, x[p], p, current);
		double lyam2 = get_lyam(hx, x[p + 1], p, current);
		G[0][0] = (lyam1 + lyam2) / (2 * hx);
		G[0][1] = -G[0][0];
		G[1][0] = -G[0][0];
		G[1][1] = G[0][0];
	}

	void make_local_M(int p) //создание локальной матрицы М в плотном формате
	{
		double hx = x[p + 1] - x[p];
		double sigma = get_sigma(hx);
		M[0][0] = sigma * hx * 2 / 6;
		M[0][1] = sigma * hx / 6;
		M[1][0] = M[0][1];
		M[1][1] = M[0][0];
	}
	void make_local_vec(int p, double t) //создание локального вектора правой части
	{
		double f1, f2, hx;
		hx = x[p + 1] - x[p];
		f1 = get_f(x[p], t);
		f2 = get_f(x[p + 1], t);
		b_loc[0] = hx * (2 * f1 + f2) / 6;
		b_loc[1] = hx * (f1 + 2 * f2) / 6;
	}

	void add_local_G(int p) // суммирование G
	{
		Gdi[p] += G[0][0];
		Gdi[p + 1] += G[1][1];
		Ggg[p] += G[0][1];
	}
	void add_local_M(int p) // суммирование М
	{
		Mdi[p] += M[0][0];
		Mdi[p + 1] += M[1][1];
		Mgg[p] += M[0][1];
	}
	void add_local_vec(int p) // суммирование вектора правой части
	{
		b[p] += b_loc[0];
		b[p + 1] += b_loc[1];
	}

	void make_global_G(vector<double> current) //создание глобальной матрицы G
	{
		for (int i = 0; i < m - 1; i++) 
		{
			Gdi[i] = 0;
			Ggg[i] = 0;
		}
		Gdi[m - 1] = 0;

		for (int p = 0; p < count_x - 1; p++) 
		{
			make_local_G(p, current);
			add_local_G(p);
		}
	}

	void make_global_M(double k) //создание глобальной матрицы М
	{
		for (int i = 0; i < m - 1; i++) 
		{
			Mdi[i] = 0;
			Mgg[i] = 0;
		}
		Mdi[m - 1] = 0;
		for (int p = 0; p < count_x - 1; p++) 
		{
			make_local_M(p);
			add_local_M(p);
		}

		for (int i = 0; i < m; i++)
			Mdi[i] *= k;
		for (int i = 0; i < Mgg.size(); i++)
			Mgg[i] *= k;
	}

	void make_global_vec(double t) //создание глобального вектора правой части
	{
		for (int i = 0; i < m; i++)
			b[i] = 0;
		for (int p = 0; p < count_x - 1; p++) 
		{
			make_local_vec(p, t);
			add_local_vec(p);
		}
	}

	void make_A() //построение матрицы А на основе глобальных матриц G и М в разреженном формате
	{
		for (int i = 0; i < m; i++)
			Adi[i] = Gdi[i] + Mdi[i];
		for (int i = 0; i < Mgg.size(); i++)
			Agg[i] = Ggg[i] + Mgg[i];
	}

	void make_d() //построениевектора правой части
	{
		mult(temp1, q_cur, Mgg, Mgg, Mdi);
		for (int i = 0; i < m; i++)
			d[i] = b[i] + k * temp1[i];
	}

	void add_first(double t) //начальные условия
	{
		long B = 1e+10;
		for (int i = 0; i < ncond1; i++) //0 то левый край, если 1(или что то ещё) то правый край
		{
			int x_num;
			int ax_num = cond1[i];
			if (ax_num == 0) x_num = 0;
			else x_num = count_x - 1;

			Adi[x_num] = B;
			d[x_num] = B * get_Ug(x[x_num], t); 
		}
	}

	//SLAE

	void LU_sq()//матрица LU
	{
		//копирование-инициализация
		for (int i = 0; i < m; i++) 
		{
			Adi_new[i] = Adi[i];
		}
		for (int i = 0; i < ig[m]; i++) 
		{
			Agl_new[i] = Agg[i];
			Agu_new[i] = Agg[i];
		}

		for (int i = 0; i < m; i++)
		{
			double sd = 0; //переменные суммирования

			int i0 = ig[i];
			int i1 = ig[i + 1];

			for (int k = i0; k < i1; k++) 
			{
				int j = jg[k];
				double sl = 0, su = 0;
				int j0 = ig[j];
				int j1 = ig[j + 1];
				int ki = i0;
				int kj = j0;

				for (; ki < k && kj < j1;) 
				{
					int jl = jg[ki];
					int ju = jg[kj];
					if (jl == ju) {
						sl += Agu_new[kj] * Agl_new[ki];
						su += Agl_new[kj] * Agu_new[ki];
						ki++; kj++;
					}
					else if (jl < ju) ki++;
					else kj++;
				}
				Agu_new[k] = (Agu_new[k] - su) / Adi_new[j];
				Agl_new[k] = (Agl_new[k] - sl) / Adi_new[j];
				sd += Agu_new[k] * Agl_new[k];
			}

			Adi_new[i] = sqrt(Adi_new[i] - sd);
		}
	}
	void mult(vector<double> &MV, vector<double> vec, vector<double>ggl, vector<double>ggu, vector<double>di) //перемножение векторов и их суммирование
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
	double scalar_mult(vector<double> vec1, vector<double> vec2) //скалярное произведение
	{
		double s = 0;
		for (int i = 0; i < m; i++)
		{
			s += vec1[i] * vec2[i];
		}
		return s;
	}
	double norm(vector<double> vec) //норма
	{
		double sum = 0;
		for (int i = 0; i < m; i++)
			sum += vec[i] * vec[i];
		return sqrt(sum);
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
			y[i] = (bb[i] - s) / Adi_new[i];
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

			y[i] /= Adi_new[i];

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
			MV[i] = Adi[i] * vec[i];
			for (int k = k0; k < k1; k++) 
			{
				int j = jg[k];
				//MV[i] += vec[j] * al[k];
				MV[j] += vec[i] * Agu_new[k];
			}
		}
	}
	void mult_tr(vector<double> &MV, vector<double> vec) 
	{
		for (int i = 0; i < m; i++)
		{
			int k0 = ig[i];
			int k1 = ig[i + 1];
			MV[i] = Adi[i] * vec[i];
			for (int k = k0; k < k1; k++) 
			{
				int j = jg[k];
				MV[i] += vec[j] * Agg[k];
				MV[j] += vec[i] * Agg[k];
			}
		}
	}
	void LOS_sq(vector<double> sol_pred, vector<double> &sol_next) 
	{
		double skal1, skal2;
		int max_it = 10000;
		double err = 1e-20;
		LU_sq();

		for (int i = 0; i < sol_next.size(); i++)
			sol_next[i] = sol_pred[i];

		//      инициализация
		mult(temp1, sol_pred, Agg, Agg, Adi);
		for (int i = 0; i < m; i++)
		{
			temp2[i] = d[i] - temp1[i];
		}
		mult_pr(Agl_new, r, temp2);
		mult_obr(Agu_new, z, r);
		mult(temp1, z, Agg, Agg, Adi);
		mult_pr(Agl_new, p, temp1);

		//iteration
		double nev = scalar_mult(r, r);
		for (int k = 0; k < max_it && nev > err; k++) 
		{
			skal1 = scalar_mult(p, r);
			skal2 = scalar_mult(p, p);

			double alfa = skal1 / skal2;
			for (int i = 0; i < m; i++) 
			{
				sol_next[i] += alfa * z[i];
				r[i] -= alfa * p[i];
			}
			mult_obr(Agu_new, temp1, r);
			mult(temp2, temp1, Agg, Agg, Adi);
			mult_pr(Agl_new, temp3, temp2);

			skal1 = scalar_mult(p, temp3);

			double beta = -skal1 / skal2;

			mult_obr(Agu_new, temp2, r);

			for (int i = 0; i < m; i++) 
			{
				z[i] = temp1[i] + beta * z[i];
			}

			for (int i = 0; i < m; i++) 
			{
				p[i] = temp3[i] + beta * p[i];
			}
			nev = scalar_mult(r, r);
		}
	};

	void do_smth() 
	{
		cout << scientific << setprecision(8);

		ofstream fout("out.txt");
		fout << scientific << setprecision(8);
		//инициализация
		make_q(q_cur, times[0]);
		//for (int i = 0; i < m; i++)
			//q_cur[i] = 0;

		//сборка
		make_portrait();
		make_global_G(q_cur);
		make_global_M(1);

		//итерации по времени
		for (int cur_t = 1; cur_t < times.size(); cur_t++) 
		{
			double dt = times[cur_t] - times[cur_t - 1];
			k = 1. / dt;
			make_global_vec(times[cur_t]);
			make_d();

			//итерации по нелинейности
			double nel_nev = 1;
			double dp = 1;
			int iter = 0;
			p_prev = q_cur;

			do
			{
				iter++;
				//сборка А d
				make_global_G(p_prev);
				make_global_M(k);
				make_A();

				add_first(times[cur_t]);
				LOS_sq(p_prev, p_next);

				//релаксация(скорее бы)
				for (int i = 0; i < m; i++)
					p_next[i] = w * p_next[i] + (1 - w) * p_prev[i];

				make_global_G(p_next);
				make_global_M(k);
				make_A();
				make_global_vec(cur_t);
				make_d();
				add_first(times[cur_t]);
				mult(temp1, p_next, Agg, Agg, Adi);

				for (int i = 0; i < m; i++) 
				{

					temp1[i] -= d[i];
					temp2[i] = p_next[i] - p_prev[i];
				}
				nel_nev = norm(temp1) / norm(d);
				dp = norm(temp2) / norm(p_next);
				p_prev = p_next;
				/*cout << iter << endl;*/
			} while (nel_nev > eps && dp > eps && iter < max_it);

			q_cur = p_next;
			cout << "iter = " << iter << endl;
			for (int i = 0; i < m; i++)
				fout << q_cur[i] << endl;
		}
	}
};

int main()
{
	SLAE slv = SLAE("net", "cond1", "time");
	slv.do_smth();
	system("pause");
	return 0;
}
