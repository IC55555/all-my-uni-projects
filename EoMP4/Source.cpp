#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <set>
#include <ctime>

using namespace std;

class SLAE
{
public:
	//параметры
	double lyam;
	double omega;
	double hi;
	double sigma;

	int n_x, n_y, n_z; //узлы областей
	int count_x, count_y, count_z; //узлы областей + внутренние

	int max_it = 2000;
	double err = 1e-10;
	int iter;
	double nev;

	vector<double> ax, ay, az; //узлы областей
	vector<double> x, y, z; //узлы-просто все уже узлы
	vector<int> nx, ny, nz;//количество разбиений области
	vector<double> qx, qy, qz;//коэффициент разбиений по области
	vector<int> oxn, oyn, ozn;//еще массивчики соответствий узлов и номера в х,y,z

	int m; //количество глобальных узлов

	//double w = 1;//параметр релаксации

	//краевые
	int ncond1, ncond2;
	vector<vector<int>> cond1;
	vector<vector<int>> cond2;

	vector<double> local_cond;
	vector<double> local_cond_vl;

	//матрицы и вектора
	vector<vector<double>> G;//локальные матрицы в плотном формате
	vector<vector<double>> M;
	vector<vector<double>> C;
	vector<vector<double>> p_loc, c_loc;//дл€ блочного
	vector<vector<double>> local;
	vector<double> b_loc;
	vector<double> fc;//локальный вектор
	vector<double> fs;//локальный вектор
	vector<double> f;//вспомогательный, значени€ функции в координатах

	vector<vector<double>> G_small, M_small;

	vector<double> x_loc, y_loc, z_loc;//координаты локальных узлов каждого элемента

	//глобальные матрицы в разреженном формате
	vector<double> ggu, ggl;//элементы
	vector<double>ggu_new, ggl_new;
	vector<double> di;//диагональные
	vector<double> di_new;
	vector<int> ig;//портрет
	vector<int> jg;//портрет

	//матрица в профильном
	vector<int> ig_pr;
	vector<double> di_pr;
	vector<double> ggu_pr, ggl_pr;

	//глобальные вектора
	vector<double> b; //глобальна€ пр часть
	vector<double> q; //истинное решение

	//slae
	vector<double> r, p, s, temp1, temp2, r0, t, y_temp, temp3;

	double BSG_alfa, BSG_beta, BSG_gamma;
	double st_time, end_time, prog_time;

	int flag;
	//vector<int> global_num;


	SLAE(string net_file, string cond_file_1, int _flag)
	{
		flag = _flag;
		read_net(net_file);
		init_net();
		read_cond(cond_file_1);
		resize_func();
	}

	~SLAE()
	{

		for (int i = 0; i < 8; i++)
		{
			G[i].~vector();
			M[i].~vector();
			C[i].~vector();
			local[i].~vector();
			p_loc[i].~vector();
			c_loc[i].~vector();
		}

		G.~vector();
		M.~vector();
		C.~vector();
		local.~vector();
		p_loc.~vector();
		c_loc.~vector();

		b_loc.~vector();
		fs.~vector();
		fc.~vector();
		f.~vector();
		//global_num.resize(2);

		ig.~vector();

		b.~vector();
		q.~vector();

		x_loc.~vector();
		y_loc.~vector();
		z_loc.~vector();

		for (int i = 0; i < 2; i++)
		{
			G_small[i].~vector();
			M_small[i].~vector();
		}
		G_small.~vector();
		M_small.~vector();

		//дл€ —лау
		r.~vector();
		z.~vector();
		p.~vector();
		temp1.~vector();
		temp2.~vector();
		if (flag == 2)
		{
			r0.~vector();
			temp3.~vector();
			t.~vector();
			y_temp.~vector();
		}
	}

	void read_net(string net_file)
	{
		ifstream fin(net_file + ".txt");

		//read x
		fin >> n_x;
		ax.resize(n_x);
		nx.resize(n_x - 1);
		qx.resize(n_x - 1);

		for (int i = 0; i < n_x; i++)
			fin >> ax[i];
		for (int i = 0; i < n_x - 1; i++)
			fin >> nx[i];
		for (int i = 0; i < n_x - 1; i++)
			fin >> qx[i];

		count_x = 1;
		for (int i = 0; i < n_x - 1; i++)
			count_x += nx[i];

		//read y
		fin >> n_y;
		ay.resize(n_y);
		ny.resize(n_y - 1);
		qy.resize(n_y - 1);

		for (int i = 0; i < n_y; i++)
			fin >> ay[i];
		for (int i = 0; i < n_y - 1; i++)
			fin >> ny[i];
		for (int i = 0; i < n_y - 1; i++)
			fin >> qy[i];

		count_y = 1;
		for (int i = 0; i < n_y - 1; i++)
			count_y += ny[i];

		//read z
		fin >> n_z;
		az.resize(n_z);
		nz.resize(n_z - 1);
		qz.resize(n_z - 1);

		for (int i = 0; i < n_z; i++)
			fin >> az[i];
		for (int i = 0; i < n_z - 1; i++)
			fin >> nz[i];
		for (int i = 0; i < n_z - 1; i++)
			fin >> qz[i];

		count_z = 1;
		for (int i = 0; i < n_z - 1; i++)
			count_z += nz[i];

		//костыльные массивчики
		oxn.resize(n_x);
		oyn.resize(n_y);
		ozn.resize(n_z);

		oxn[0] = 0;
		for (int i = 1; i < n_x; i++)
		{
			oxn[i] = oxn[i - 1] + nx[i - 1];
		}

		oyn[0] = 0;
		for (int i = 1; i < n_y; i++)
		{
			oyn[i] = oyn[i - 1] + ny[i - 1];
		}
		ozn[0] = 0;
		for (int i = 1; i < n_z; i++)
		{
			ozn[i] = ozn[i - 1] + nz[i - 1];
		}


		fin.close();





		m = count_x * count_y * count_z;
	}

	void init_net()
	{
		double hx, hy, hz, tx, ty, tz;

		//обработка по х
		for (int j = 0; j < n_x - 1; j++)
		{
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

		//обработка по y
		for (int j = 0; j < n_y - 1; j++)
		{
			if (qy[j] != 1)
				hy = (ay[j + 1] - ay[j]) * (1. - qy[j]) / (1. - pow(qy[j], ny[j]));
			else hy = (ay[j + 1] - ay[j]) / ny[j];

			ty = ay[j];

			for (int k = 0; k < ny[j]; k++)
			{
				y.push_back(ty);
				ty += hy;
				hy *= qy[j];
			}
		}
		y.push_back(ty);

		//обработка по z
		for (int j = 0; j < n_z - 1; j++)
		{
			if (qz[j] != 1)
				hz = (az[j + 1] - az[j]) * (1. - qz[j]) / (1. - pow(qz[j], nz[j]));
			else hz = (az[j + 1] - az[j]) / nz[j];

			tz = az[j];

			for (int k = 0; k < nz[j]; k++)
			{
				z.push_back(tz);
				tz += hz;
				hz *= qz[j];
			}
		}
		z.push_back(tz);
	}

	void resize_func()
	{
		G.resize(8);
		M.resize(8);
		C.resize(8);
		local.resize(8);
		p_loc.resize(8);
		c_loc.resize(8);

		for (int i = 0; i < 8; i++)
		{
			G[i].resize(8);
			M[i].resize(8);
			C[i].resize(8);
			local[i].resize(8);
			p_loc[i].resize(8);
			c_loc[i].resize(8);
		}
		b_loc.resize(8);
		fs.resize(8);
		fc.resize(8);
		f.resize(8);
		//global_num.resize(2);

		ig.resize(2 * m + 1);

		b.resize(2 * m);
		q.resize(2 * m);

		x_loc.resize(8);
		y_loc.resize(8);
		z_loc.resize(8);

		G_small.resize(2);
		M_small.resize(2);
		for (int i = 0; i < 2; i++)
		{
			G_small[i].resize(2);
			M_small[i].resize(2);
		}

		//дл€ —лау
		r.resize(2 * m);
		z.resize(2 * m);
		p.resize(2 * m);
		temp1.resize(2 * m);
		temp2.resize(2 * m);
		if (flag == 2)
		{
			r0.resize(2 * m);
			temp3.resize(2 * m);
			t.resize(2 * m);
			y_temp.resize(2 * m);
		}

		/*
		local_cond.resize(3);
		local_cond_vl.resize(3);*/

	}

	void read_cond(string cond_file_1)
	{
		ifstream fin1(cond_file_1 + ".txt");
		fin1 >> ncond1;
		
		cond1.resize(ncond1);
		
		for (int i = 0; i < ncond1; i++)
		{
			
			cond1[i].resize(6);
			for (int j = 0; j < 6; j++)
				fin1 >> cond1[i][j];
		}
		fin1.close();
		
		//ifstream fin2(cond_file_2 + ".txt");
		//fin2 >> ncond2;
		//cond2.resize(ncond2);
		//for (int i = 0; i < ncond2; i++) 
		//{
		//	cond2[i].resize(5);
		//	for (int j = 0; j < 5; j++)
		//		fin2 >> cond2[i][j];
		//}
		//fin2.close();
	}

	void write_small_matrix()
	{
		G_small[0][0] = 1;
		G_small[0][1] = -1;
		G_small[1][0] = -1;
		G_small[1][1] = 1;

		M_small[0][0] = 2;
		M_small[0][1] = 1;
		M_small[1][0] = 1;
		M_small[1][1] = 2;
	}

	int get_global(int r, int s, int p, int j) //получение глобального номера по локальному
	{
		int k;
		switch (j)
		{
		case 0:
		{
			k = r * count_x * count_y + s * count_x + p;
			break;
		}
		case 1:
		{
			k = r * count_x * count_y + s * count_x + p + 1;
			break;
		}
		case 2:
		{
			k = r * count_x * count_y + (s + 1) * count_x + p;
			break;
		}

		case 3:
		{
			k = r * count_x * count_y + (s + 1) * count_x + p + 1;
			break;
		}
		case 4:
		{
			k = (r + 1) * count_x * count_y + s * count_x + p;
			break;
		}
		case 5:
		{
			k = (r + 1) * count_x * count_y + s * count_x + p + 1;
			break;
		}
		case 6:
		{
			k = (r + 1) * count_x * count_y + (s + 1) * count_x + p;
			break;
		}
		case 7:
		{
			k = (r + 1) * count_x * count_y + (s + 1) * count_x + p + 1;
			break;
		}
		default:
			return -1;
		}
		return k;
	}

	double get_Ugs(double x, double y, double z)
	{
		return exp(x + y + z);
	}
	double get_Ugc(double x, double y, double z)
	{
		return exp(x - y - z);
	}

	double func_s(int num_area, double x, double y, double z)
	{
		double func_loc;
		switch (num_area)
		{
		case 1:
		{
			func_loc = -3 * get_Ugs(x, y, z) - omega * sigma * get_Ugc(x, y, z) - omega * omega * hi * get_Ugs(x, y, z);
			break;
		}
		default:
			return -1;
		}
		return func_loc;
	}

	double func_c(int num_area, double x, double y, double z)
	{
		double func_loc;
		switch (num_area)
		{
		case 1:
		{
			func_loc = -3 * get_Ugc(x, y, z) + omega * sigma * get_Ugs(x, y, z) - omega * omega * hi * get_Ugc(x, y, z);
			//func_loc = -6 * x*y*y*y - 6 * x*x*x*y + x * x*x*y*y*y;
			break;
		}
		default:
			return -1;
		}
		return func_loc;
	}


	void make_portrait()
	{
		//списочки дл€ всех глоб узлов
		vector<set<int>> list(m);
		for (int r = 0; r < count_z - 1; r++)
		{
			for (int s = 0; s < count_y - 1; s++)
			{
				for (int p = 0; p < count_x - 1; p++)
				{
					for (int i = 0; i < 8; i++)
					{

						for (int j = i + 1; j < 8; j++)
						{
							int ind1 = get_global(r, s, p, i);
							int ind2 = get_global(r, s, p, j);

							if (ind1 < ind2) swap(ind1, ind2);
							list[ind1].insert(ind2);
						}
					}
				}
			}
		}

		//создание портрета по списку
		ig[0] = 0;
		ig[1] = 0;
		ig[2] = 1;

		for (int i = 1; i < m; i++)
		{

			ig[2 * i + 1] = ig[2 * i] + list[i].size() * 2;
			ig[2 * (i + 1)] = ig[2 * i + 1] + list[i].size() * 2 + 1;
			//if (i % 2 == 1) ig[i]++;
		}
		jg.resize(ig[2 * m]);
		ggu.resize(ig[2 * m]);
		ggl.resize(ig[2 * m]);
		di.resize(2 * m);

		ggu_new.resize(ig[2 * m]);
		ggl_new.resize(ig[2 * m]);
		di_new.resize(2 * m);


		for (int i = 1, k = 1; i < m; i++)
		{
			for (int j : list[i])
			{

				jg[k] = 2 * j;
				jg[k + 1] = 2 * j + 1;
				k += 2;
			}
			for (int j : list[i])
			{

				jg[k] = 2 * j;
				jg[k + 1] = 2 * j + 1;
				k += 2;
			}
			jg[k] = 2 * i;
			k++;
		}
	}

	void make_local_coor(int r, int s, int p)
	{
		double xp, xp_next, ys, ys_next, zr, zr_next;
		xp = x[p];
		xp_next = x[p + 1];
		ys = y[s];
		ys_next = y[s + 1];
		zr = z[r];
		zr_next = z[r + 1];

		x_loc[0] = xp;
		x_loc[1] = xp_next;
		x_loc[2] = xp;
		x_loc[3] = xp_next;
		x_loc[4] = xp;
		x_loc[5] = xp_next;
		x_loc[6] = xp;
		x_loc[7] = xp_next;

		y_loc[0] = ys;
		y_loc[1] = ys;
		y_loc[2] = ys_next;
		y_loc[3] = ys_next;
		y_loc[4] = ys;
		y_loc[5] = ys;
		y_loc[6] = ys_next;
		y_loc[7] = ys_next;

		z_loc[0] = zr;
		z_loc[1] = zr;
		z_loc[2] = zr;
		z_loc[3] = zr;
		z_loc[4] = zr_next;
		z_loc[5] = zr_next;
		z_loc[6] = zr_next;
		z_loc[7] = zr_next;
	}

	void make_local_matrix(double hx, double hy, double hz)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				int mu_i = (i % 2);
				int mu_j = (j % 2);
				int nu_i = ((int)(i / 2) % 2);
				int nu_j = ((int)(j / 2) % 2);
				int ips_i = ((int)(i / 4));
				int ips_j = ((int)(j / 4));

				G[i][j] = ((hy * hz / hx) * G_small[mu_i][mu_j] * M_small[nu_i][nu_j] * M_small[ips_i][ips_j]
					+ (hx * hz / hy) * M_small[mu_i][mu_j] * G_small[nu_i][nu_j] * G_small[ips_i][ips_j]
					+ (hx * hy / hz) * M[mu_i][mu_j] * M[nu_i][nu_j] * G[ips_i][ips_j]);


				M[i][j] = hx * hy * hz * M_small[mu_i][mu_j] * M_small[nu_i][nu_j] * M_small[ips_i][ips_j];


				p_loc[i][j] = lyam * G[i][j] - omega * omega * hi * M[i][j];
				c_loc[i][j] = omega * sigma * M[i][j];

				if (i != j)
				{
					G[j][i] = G[i][j];
					M[j][i] = M[i][j];
					p_loc[j][i] = p_loc[i][j];
					c_loc[j][i] = c_loc[i][j];
				}
			}
		}

	}

	void make_local_vec(int num_area)
	{
		for (int i = 0; i < 8; i++)
		{
			f[i] = func_c(num_area, x_loc[i], y_loc[i], z_loc[i]);
			fc[i] = 0;
		}

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				fc[i] += f[j] * M[i][j];
			}
		}


		for (int i = 0; i < 8; i++)
		{
			f[i] = func_s(num_area, x_loc[i], y_loc[i], z_loc[i]);
			fs[i] = 0;
		}

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				fs[i] += f[j] * M[i][j];
			}
		}
	}
	void clean_matrix()
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				G[i][j] = 0;
				M[i][j] = 0;
				p_loc[i][j] = 0;
				c_loc[i][j] = 0;
			}
			fc[i] = 0;
			fs[i] = 0;
		}
	}

	void make_global()
	{
		double hx, hy, hz;
		//double lyam, gamma;
		for (int r = 0; r < count_z - 1; r++)
		{
			for (int s = 0; s < count_y - 1; s++)
			{
				for (int p = 0; p < count_x - 1; p++)
				{

					hx = x[p + 1] - x[p];
					hy = y[s + 1] - y[s];
					hz = z[r + 1] - z[r];
					int num_area = 1;
					//int num_area=get_area(s,p);
					make_local_coor(r, s, p);
					make_local_matrix(hx, hy, hz);

					make_local_vec(num_area);
					add_local(r, s, p);
					clean_matrix();
				}
			}
		}

	}

	void add_first()
	{
		int r, s, p;
		for (int i = 0; i < ncond1; i++)
		{
			
			int x_num_beg = cond1[i][0];
			int x_num_end = cond1[i][1];
			int y_num_beg = cond1[i][2];
			int y_num_end = cond1[i][3];
			int z_num_beg = cond1[i][4];
			int z_num_end = cond1[i][5];
			long B = 1e+5;

			int flag;
			if (z_num_beg == z_num_end) flag = 0;//нижн€€ и верхн€€
			else
			{

				if (y_num_beg == y_num_end) flag = 1;//передн€€ и задн€€
				else flag = 2; //боковые
			}

			//все работает по номерам в массивах х,y,z
			if (flag == 0)
			{//нижн€€ или верхн€€				
				r = z_num_beg;
				//cout << oyn.size() << ' ' << y_num_beg << ' ' << y_num_end << endl;
				for (s = oyn[y_num_beg]; s < oyn[y_num_end]; s++)
				{
					
					for (p = oxn[x_num_beg]; p < oxn[x_num_end]; p++)
					{
						//ставим в p и c дл€ 0-3(нижних) узлов
						di[2 * get_global(r, s, p, 0)] = B;
						di[2 * get_global(r, s, p, 0) + 1] = B;
						di[2 * get_global(r, s, p, 1)] = B;
						di[2 * get_global(r, s, p, 1) + 1] = B;
						di[2 * get_global(r, s, p, 2)] = B;
						di[2 * get_global(r, s, p, 2) + 1] = B;
						di[2 * get_global(r, s, p, 3)] = B;
						di[2 * get_global(r, s, p, 3) + 1] = B;

						b[2 * get_global(r, s, p, 0)] = B * get_Ugs(x[p], y[s], z[r]);
						b[2 * get_global(r, s, p, 0) + 1] = B * get_Ugc(x[p], y[s], z[r]);
						b[2 * get_global(r, s, p, 1)] = B * get_Ugs(x[p + 1], y[s], z[r]);
						b[2 * get_global(r, s, p, 1) + 1] = B * get_Ugc(x[p + 1], y[s], z[r]);
						b[2 * get_global(r, s, p, 2)] = B * get_Ugs(x[p], y[s + 1], z[r]);
						b[2 * get_global(r, s, p, 2) + 1] = B * get_Ugc(x[p], y[s + 1], z[r]);
						b[2 * get_global(r, s, p, 3)] = B * get_Ugs(x[p + 1], y[s + 1], z[r]);
						b[2 * get_global(r, s, p, 3) + 1] = B * get_Ugc(x[p + 1], y[s + 1], z[r]);
					}
				}
				
			}

			if (flag == 1)
			{//передн€€ или задн€€				
				s = y_num_beg;
				for (r = ozn[z_num_beg]; r < ozn[z_num_end]; r++)
				{
					for (p = oxn[x_num_beg]; p < oxn[x_num_end]; p++)
					{
						//ставим в p и c дл€ 0 1 4 5(передних) узлов
						di[2 * get_global(r, s, p, 0)] = B;
						di[2 * get_global(r, s, p, 0) + 1] = B;
						di[2 * get_global(r, s, p, 1)] = B;
						di[2 * get_global(r, s, p, 1) + 1] = B;
						di[2 * get_global(r, s, p, 4)] = B;
						di[2 * get_global(r, s, p, 4) + 1] = B;
						di[2 * get_global(r, s, p, 5)] = B;
						di[2 * get_global(r, s, p, 5) + 1] = B;

						b[2 * get_global(r, s, p, 0)] = B * get_Ugs(x[p], y[s], z[r]);
						b[2 * get_global(r, s, p, 0) + 1] = B * get_Ugc(x[p], y[s], z[r]);
						b[2 * get_global(r, s, p, 1)] = B * get_Ugs(x[p + 1], y[s], z[r]);
						b[2 * get_global(r, s, p, 1) + 1] = B * get_Ugc(x[p + 1], y[s], z[r]);
						b[2 * get_global(r, s, p, 4)] = B * get_Ugs(x[p], y[s], z[r + 1]);
						b[2 * get_global(r, s, p, 4) + 1] = B * get_Ugc(x[p], y[s], z[r + 1]);
						b[2 * get_global(r, s, p, 5)] = B * get_Ugs(x[p + 1], y[s], z[r + 1]);
						b[2 * get_global(r, s, p, 5) + 1] = B * get_Ugc(x[p + 1], y[s], z[r + 1]);
					}
				}

			}

			if (flag == 2)
			{//лева€ или права€
				p = x_num_beg;
				for (r = ozn[z_num_beg]; r < ozn[z_num_end]; r++)
				{
					for (s = oyn[y_num_beg]; s < oyn[y_num_end]; s++)
					{
						//ставим в p и c дл€ 0 2 4 6(левых) узлов
						di[2 * get_global(r, s, p, 0)] = B;
						di[2 * get_global(r, s, p, 0) + 1] = B;
						di[2 * get_global(r, s, p, 2)] = B;
						di[2 * get_global(r, s, p, 2) + 1] = B;
						di[2 * get_global(r, s, p, 4)] = B;
						di[2 * get_global(r, s, p, 4) + 1] = B;
						di[2 * get_global(r, s, p, 6)] = B;
						di[2 * get_global(r, s, p, 6) + 1] = B;

						b[2 * get_global(r, s, p, 0)] = B * get_Ugs(x[p], y[s], z[r]);
						b[2 * get_global(r, s, p, 0) + 1] = B * get_Ugc(x[p], y[s], z[r]);
						b[2 * get_global(r, s, p, 2)] = B * get_Ugs(x[p], y[s + 1], z[r]);
						b[2 * get_global(r, s, p, 2) + 1] = B * get_Ugc(x[p], y[s + 1], z[r]);
						b[2 * get_global(r, s, p, 4)] = B * get_Ugs(x[p], y[s], z[r + 1]);
						b[2 * get_global(r, s, p, 4) + 1] = B * get_Ugc(x[p], y[s], z[r + 1]);
						b[2 * get_global(r, s, p, 6)] = B * get_Ugs(x[p], y[s + 1], z[r + 1]);
						b[2 * get_global(r, s, p, 6) + 1] = B * get_Ugc(x[p], y[s + 1], z[r + 1]);
					}
				}

			}
		}
	}

	void add_local(int r, int s, int p)
	{
		vector<int> global_num(8);
		for (int i = 0; i < 8; i++)
			global_num[i] = 2 * get_global(r, s, p, i);

		for (int i = 0; i < 8; i++)
		{
			di[global_num[i]] += p_loc[i][i];
			di[global_num[i] + 1] += p_loc[i][i];


			int end0 = ig[global_num[i] + 2] - 1;
			int ind0 = end0;
			ggl[ind0] += c_loc[i][i]; //-c11
			ggu[ind0] -= c_loc[i][i];


			int beg = ig[global_num[i]];
			for (int j = 0; j < i; j++, beg++)
			{
				int end = ig[global_num[i] + 1] - 1;
				int ind = beg;
				while (jg[ind] != global_num[j])
				{
					ind++;
				}
				ggl[ind] += p_loc[i][j]; //p21
				ggl[ind + 1] -= c_loc[i][j]; //-c21
			}

			int beg2 = ig[global_num[i] + 1];
			for (int j = 0; j < i; j++, beg++)
			{
				int end2 = ig[global_num[i] + 2] - 1;
				int ind2 = beg2;
				while (jg[ind2] != global_num[j])
				{
					ind2++;
				}
				ggl[ind2] += c_loc[i][j]; //c21
				ggl[ind2 + 1] += p_loc[i][j]; //p21
			}

			//FOR GGU
			beg = ig[global_num[i]];
			for (int j = 0; j < i; j++, beg++)
			{
				int end = ig[global_num[i] + 1] - 1;
				int ind = beg;
				while (jg[ind] != global_num[j])
				{
					ind++;
				}
				ggu[ind] += p_loc[i][j]; //p21
				ggu[ind + 1] = c_loc[i][j]; //-c21
			}

			beg2 = ig[global_num[i] + 1];
			for (int j = 0; j < i; j++, beg++)
			{
				int end2 = ig[global_num[i] + 2] - 1;
				int ind2 = beg2;
				while (jg[ind2] != global_num[j])
				{
					ind2++;
				}
				ggu[ind2] -= c_loc[i][j]; //c21
				ggu[ind2 + 1] += p_loc[i][j]; //p21
			}

			b[global_num[i]] += fs[i];
			b[global_num[i] + 1] += fc[i];
		}
	}

	//SLAE 
	void LU_sq()
	{
		//копирование-инициализаци€
		for (int i = 0; i < di.size(); i++)
		{
			di_new[i] = di[i];
		}
		for (int i = 0; i < ggl.size(); i++)
		{
			ggl_new[i] = ggl[i];
			ggu_new[i] = ggu[i];
		}

		for (int i = 0; i < di.size(); i++)
		{
			double sd = 0; //переменные суммировани€

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
					if (jl == ju)
					{
						sl += ggu_new[kj] * ggl_new[ki];
						su += ggl_new[kj] * ggu_new[ki];
						ki++; kj++;
					}
					else if (jl < ju) ki++;
					else kj++;
				}

				ggu_new[k] = (ggu_new[k] - su) / di_new[j];
				ggl_new[k] = (ggl_new[k] - sl) / di_new[j];
				sd += ggu_new[k] * ggl_new[k];
			}

			di_new[i] = sqrt(di_new[i] - sd);
		}
	}
	void mult(vector<double> &MV, vector<double> vec)
	{
		for (int i = 0; i < 2 * m; i++)
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
		for (int i = 0; i < vec1.size(); i++)
		{
			s += vec1[i] * vec2[i];
		}
		return s;
	}
	long double norm(vector<double> vec)
	{
		double sum = 0;
		for (int i = 0; i < vec.size(); i++)
			sum += vec[i] * vec[i];
		return sqrt(sum);
	}
	void mult_pr(vector<double> aa, vector<double>di, vector<double> &y, vector<double> bb)
	{

		for (int i = 0; i < y.size(); i++)
		{

			double s = 0; //переменные суммировани€

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
	void mult_obr(vector<double> aa, vector<double>di, vector<double> &y, vector<double> bb)
	{
		for (int i = 0; i < y.size(); i++)
			y[i] = bb[i];
		for (int i = y.size() - 1; i >= 0; i--)
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
		for (int i = 0; i < vec.size(); i++)
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
		for (int i = 0; i < vec.size(); i++)
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

	void LOS_sq()
	{
		double skal1, skal2;
		LU_sq();

		//	инициализаци€
		mult(temp1, q);
		for (int i = 0; i < 2 * m; i++)
		{
			temp2[i] = b[i] - temp1[i];
		}
		mult_pr(ggl_new, di_new, r, temp2);

		mult_obr(ggu_new, di_new, z, r);

		mult(temp1, z);
		mult_pr(ggl_new, di_new, p, temp1);

		//iteration
		nev = skal_mult(r, r);
		for (iter = 0; iter < max_it && nev > err; iter++)
		{


			skal1 = skal_mult(p, r);
			skal2 = skal_mult(p, p);

			double alfa = skal1 / skal2;
			for (int i = 0; i < 2 * m; i++)
			{

				q[i] += alfa * z[i];
				r[i] -= alfa * p[i];
			}
			mult_obr(ggu_new, di_new, temp1, r);
			mult(temp2, temp1);
			mult_pr(ggl_new, di_new, temp1, temp2);

			skal1 = skal_mult(p, temp1);

			double beta = -skal1 / skal2;

			mult_obr(ggu_new, di_new, temp2, r);

			for (int i = 0; i < 2 * m; i++)
			{
				z[i] = temp2[i] + beta * z[i];
			}

			for (int i = 0; i < 2 * m; i++)
			{
				p[i] = temp1[i] + beta * p[i];
			}

			nev = skal_mult(r, r);
			cout << iter << ": " << nev << endl;
		}
	};
	void generate_profil()
	{
		ig_pr.resize(ig.size());
		di_pr.resize(ig.size() - 1);
		ig_pr[0] = 0;
		ig_pr[1] = 0;
		di_pr[0] = di[0];

		ggl_pr.resize(0);
		ggu_pr.resize(0);


		for (int i = 1; i < ig.size() - 1; i++)
		{
			di_pr[i] = di[i];


			int j0 = jg[ig[i]];
			int j1 = jg[ig[i + 1] - 1];
			int k_r = ig[i + 1] - 1 - ig[i];
			int k = i - j0;//количество элементов в строке профильного
			ig_pr[i + 1] = ig_pr[i] + k;
			for (int j = 0; j < k; j++)
			{
				ggl_pr.push_back(0);
				ggu_pr.push_back(0);
			}

			int i0 = ig[i];
			int i0_pr = ig_pr[i];
			for (int j = 0; j <= k_r; j++)
			{
				ggl_pr[i0_pr + jg[i0 + j] - j0] = ggl[i0 + j];
				ggu_pr[i0_pr + jg[i0 + j] - j0] = ggu[i0 + j];
			}

		}

	}

	void LU_mult_pr()
	{

		for (int i = 0; i < q.size(); i++)
		{
			double s = 0; //переменна€ суммировани€
			int j0 = i - (ig_pr[i + 1] - ig_pr[i]); //индекс 1го элемента в iтой строке
			int k = ig_pr[i];
			for (int j = j0; j < i; j++, k++)
			{
				s += temp1[j] * ggl_pr[k];
			}
			temp1[i] = b[i] - s;
		}
	}
	void LU_mult_obr()
	{
		for (int i = 0; i < q.size(); i++)
			q[i] = temp1[i];
		for (int i = q.size() - 1; i >= 0; i--)
		{ //i-номер столбца
			int j0 = i - (ig_pr[i + 1] - ig_pr[i]); //индекс 1го элемента в iтом столбце
			int k = ig_pr[i + 1] - 1;
			q[i] /= di_pr[i];
			for (int j = i - 1; j >= j0; j--, k--)
			{

				q[j] -= q[i] * ggu_pr[k];
			}
		}
	}
	void LU()
	{
		for (int i = 0; i < q.size(); i++)
		{
			double s1 = 0, s2 = 0, s3 = 0; //переменные суммировани€
			int j0 = i - (ig_pr[i + 1] - ig_pr[i]);
			int k = ig_pr[i];
			int j, p0;
			for (j = j0; j < i; j++, k++)
			{
				s1 = 0, s2 = 0;
				int i0 = j - (ig_pr[j + 1] - ig_pr[j]);
				if (i0 > j0) p0 = i0;
				else p0 = j0;
				for (int p = p0; p < j; p++)
				{
					s1 += ggu_pr[ig_pr[j] + p - i0] * ggl_pr[ig_pr[i] + p - j0];
					s2 += ggl_pr[ig_pr[j] + p - i0] * ggu_pr[ig_pr[i] + p - j0];
				}
				ggl_pr[k] -= s1;
				ggu_pr[k] -= s2;
				ggl_pr[k] /= di_pr[j];
			}
			for (int p = j0; p < j; p++)
			{
				s3 += ggu_pr[ig_pr[i] + p - j0] * ggl_pr[ig_pr[i] + p - j0];
			}
			di_pr[i] -= s3;
		}
	}

	void LU_sol()
	{
		//перегенераци€ в профильный
		generate_profil();
		//Ћ”-разложение дл€ профильного
		LU();
		LU_mult_pr();
		LU_mult_obr();
		for (int i = 0; i < q.size(); i++)
			temp2[i] = temp1[i] - b[i];
		nev = (norm(temp2)) / norm(b);
		cout << "nev=" << nev << endl;
	}

	void BSG_Stab_LU_sq()
	{
		double skal1, skal2, skal3;
		LU_sq();

		//	инициализаци€
		mult(temp1, q);
		for (int i = 0; i < 2 * m; i++)
		{
			temp2[i] = b[i] - temp1[i];
		}
		mult_pr(ggl_new, di_new, r0, temp2);
		mult_obr(ggu_new, di_new, z, r0);
		r = r0;
		skal3 = skal_mult(r, r0);


		nev = skal_mult(r, r);

		//iteration
		for (iter = 0; iter < max_it && nev > err; iter++)
		{

			//skal3=r(k-1)*r0

			mult_obr(ggu_new, di_new, temp1, z);
			mult(temp2, temp1);
			mult_pr(ggl_new, di_new, temp1, temp2);
			//temp1=L-1*A*U-1*z 

			skal2 = skal_mult(temp1, r0);
			BSG_alfa = skal3 / skal2;

			for (int i = 0; i < 2 * m; i++)
			{
				p[i] = r[i] - BSG_alfa * temp1[i];
			}

			mult_obr(ggu_new, di_new, temp3, p);
			mult(temp2, temp3);
			mult_pr(ggl_new, di_new, temp3, temp2);
			//temp3=L-1*A*U-1*p

			skal1 = skal_mult(p, temp3);
			skal2 = skal_mult(temp3, temp3);
			BSG_gamma = skal1 / skal2;

			for (int i = 0; i < 2 * m; i++)
			{
				y_temp[i] = y_temp[i] + BSG_alfa * z[i] + BSG_gamma * p[i];
				r[i] = p[i] - BSG_gamma * temp3[i];
			}

			skal1 = skal_mult(r, r0);

			BSG_beta = BSG_alfa * skal1 / (BSG_gamma * skal3);
			skal3 = skal1;

			for (int i = 0; i < 2 * m; i++)
			{
				z[i] = r[i] + BSG_beta * z[i] - BSG_beta * BSG_gamma * temp1[i];
			}

			nev = skal_mult(r, r);
			cout << iter << ": " << nev << endl;
		}
		mult_obr(ggu_new, di_new, q, y_temp);
	}

	void do_smth()
	{
		for (int i = 0; i < q.size(); i++)
			q[i] = 0;
		clean_matrix();
		
		st_time = clock();
		
		write_small_matrix();
		
		make_portrait();
		
		make_global();
		
		add_first();

		if (flag == 0)	LOS_sq();
		if (flag == 1) LU_sol();
		if (flag == 2) BSG_Stab_LU_sq();
		end_time = clock(); // врем€ работы программы
		prog_time = end_time - st_time;

	}

};

int main()
{


	ofstream res("LU10-4.txt");
	SLAE slv = SLAE("net", "cond1", 1);


	//cout << slv.ggl.max_size() << endl;
	//cout << slv.ggu_new.max_size() << endl;


	slv.omega = 1e-4;

	res << "===========lyam=1e+2============" << endl;
	cout << "===========lyam=1e+2============" << endl;
	slv.lyam = 100;
	{
		cout << "__________sigma=0_____________" << endl;
		res << "__________sigma=0_____________" << endl;
		slv.sigma = 0;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl;
	}
	{
		cout << "__________sigma=1e4_____________" << endl;
		res << "__________sigma=1e4_____________" << endl;
		slv.sigma = 1e+4;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl; }
	{
		cout << "__________sigma=1e8_____________" << endl;
		res << "__________sigma=1e8_____________" << endl;
		slv.sigma = 1e+8;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl; }

	cout << "===========lyam=1e+4=========" << endl;
	res << "===========lyam=1e+4=========" << endl;
	slv.lyam = 1e+4;
	{
		cout << "__________sigma=0_____________" << endl;
		res << "__________sigma=0_____________" << endl;
		slv.sigma = 0;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl;
	}
	{
		cout << "__________sigma=1e4_____________" << endl;
		res << "__________sigma=1e4_____________" << endl;
		slv.sigma = 1e+4;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl; }
	{
		cout << "__________sigma=1e8_____________" << endl;
		res << "__________sigma=1e8_____________" << endl;
		slv.sigma = 1e+8;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl; }

	cout << "============lyam=8e+5==============" << endl;
	res << "============lyam=8e+5==============" << endl;
	slv.lyam = 8e+5;
	{
		cout << "__________sigma=0_____________" << endl;
		res << "__________sigma=0_____________" << endl;
		slv.sigma = 0;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl;
	}
	{
		cout << "__________sigma=1e4_____________" << endl;
		res << "__________sigma=1e4_____________" << endl;
		slv.sigma = 1e+4;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl; }
	{
		cout << "__________sigma=1e8_____________" << endl;
		res << "__________sigma=1e8_____________" << endl;
		slv.sigma = 1e+8;
		res << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();

		res << "time=" << slv.prog_time / 1000 << endl << endl;
		res << "nev=" << slv.nev << endl;
		res << endl << "iter=" << slv.iter << endl; }

	res.close();

	/*
	ofstream res2("BCG10+5.txt");
	SLAE slv2 = SLAE("net", "cond1", 2);

	slv2.omega = 1e+5;

	res2 << "===========lyam=1e+2============" << endl;
	cout << "===========lyam=1e+2============" << endl;
	slv2.lyam = 100;
	{
		cout << "__________sigma=0_____________" << endl;
		res2 << "__________sigma=0_____________" << endl;
		slv2.sigma = 0;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl;
	}
	{
		cout << "__________sigma=1e4_____________" << endl;
		res2 << "__________sigma=1e4_____________" << endl;
		slv2.sigma = 1e+4;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl; }
	{
		cout << "__________sigma=1e8_____________" << endl;
		res2 << "__________sigma=1e8_____________" << endl;
		slv2.sigma = 1e+8;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl; }

	cout << "===========lyam=1e+4=========" << endl;
	res2 << "===========lyam=1e+4=========" << endl;
	slv2.lyam = 1e+4;
	{
		cout << "__________sigma=0_____________" << endl;
		res2 << "__________sigma=0_____________" << endl;
		slv2.sigma = 0;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl;
	}
	{
		cout << "__________sigma=1e4_____________" << endl;
		res2 << "__________sigma=1e4_____________" << endl;
		slv2.sigma = 1e+4;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl; }
	{
		cout << "__________sigma=1e8_____________" << endl;
		res2 << "__________sigma=1e8_____________" << endl;
		slv2.sigma = 1e+8;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl; }

	cout << "============lyam=8e+5==============" << endl;
	res2 << "============lyam=8e+5==============" << endl;
	slv2.lyam = 8e+5;
	{
		cout << "__________sigma=0_____________" << endl;
		res2 << "__________sigma=0_____________" << endl;
		slv2.sigma = 0;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl;
	}
	{
		cout << "__________sigma=1e4_____________" << endl;
		res2 << "__________sigma=1e4_____________" << endl;
		slv2.sigma = 1e+4;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl; }
	{
		cout << "__________sigma=1e8_____________" << endl;
		res2 << "__________sigma=1e8_____________" << endl;
		slv2.sigma = 1e+8;
		res2 << endl << "1e-11" << endl;
		slv2.hi = 1e-11;
		slv2.do_smth();

		res2 << "time=" << slv2.prog_time / 1000 << endl << endl;
		res2 << "nev=" << slv2.nev << endl;
		res2 << endl << "iter=" << slv2.iter << endl;
		}
	res2.close();
	//ofstream fout("out.txt");
	//for (int i = 0; i < slv.m * 2; i++)
	//	fout << slv.q[i] << endl;
	//res << "time=" << slv.end_time/1000 ;
	*/
	system("pause");
	return 0;
}