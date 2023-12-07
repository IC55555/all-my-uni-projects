#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <set>
#include <ctime>

using namespace std;

class SLAE {
public:
	//параметры
	double lyam = 100;
	double omega = 1e+4;
	double hi = 1e-11;
	double sigma = 1;



	int n_x, n_y, n_z; //узлы областей
	int count_x, count_y, count_z; //узлы областей + внутренние

	int max_it = 5000;
	double err = 1e-10;

	vector<double> ax, ay, az; //узлы областей
	vector<double> x, y, z; //узлы-просто все уже узлы
	vector<int> nx, ny, nz;//количество разбиений области
	vector<double> qx, qy, qz;//коэффициент разбиений по области
	vector<int> oxn, oyn, ozn;//еще массивчики соответствий узлов и номера в х,y,z

	int m; //количество глобальных узлов

//	double w = 1;//параметр релаксации

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
	vector<vector<double>> p_loc, c_loc;//для блочного
	vector<vector<double>> local;
	vector<double> b_loc;
	vector<double> fc;//локальный вектор
	vector<double> fs;//локальный вектор
	vector<double> f;//вспомогательный, значения функции в координатах

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
	vector<double> b; //глобальная пр часть
	vector<double> q; //истинное решение

	//slae
	vector<double> r, p, s, temp1, temp2;

	double st_time, end_time, prog_time;

	int flag;
	//vector<int> global_num;


	SLAE(string net_file, string cond_file_1, string cond_file_2, int _flag) {
		flag = _flag;
		read_net(net_file);
		init_net();
		read_cond(cond_file_1, cond_file_2);
		resize_func();
	}

	//СДЕЛАТЬ ДЕСТРУКТОР
	~SLAE() {
	}

	void read_net(string net_file) {
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
		for (int i = 1; i < n_x; i++) {
			oxn[i] = oxn[i - 1] + nx[i - 1];
		}

		oyn[0] = 0;
		for (int i = 1; i < n_y; i++) {
			oyn[i] = oyn[i - 1] + ny[i - 1];
		}
		ozn[0] = 0;
		for (int i = 1; i < n_z; i++) {
			ozn[i] = ozn[i - 1] + nz[i - 1];
		}


		fin.close();





		m = count_x * count_y * count_z;
	}

	void init_net() {
		double hx, hy, hz, tx, ty, tz;

		//обработка по х
		for (int j = 0; j < n_x - 1; j++) {
			if (qx[j] != 1)
				hx = (ax[j + 1] - ax[j]) * (1. - qx[j]) / (1. - pow(qx[j], nx[j]));
			else hx = (ax[j + 1] - ax[j]) / nx[j];

			tx = ax[j];

			for (int k = 0; k < nx[j]; k++) {
				x.push_back(tx);
				tx += hx;
				hx *= qx[j];
			}
		}
		x.push_back(tx);

		//обработка по y
		for (int j = 0; j < n_y - 1; j++) {
			if (qy[j] != 1)
				hy = (ay[j + 1] - ay[j]) * (1. - qy[j]) / (1. - pow(qy[j], ny[j]));
			else hy = (ay[j + 1] - ay[j]) / ny[j];

			ty = ay[j];

			for (int k = 0; k < ny[j]; k++) {
				y.push_back(ty);
				ty += hy;
				hy *= qy[j];
			}
		}
		y.push_back(ty);

		//обработка по z
		for (int j = 0; j < n_z - 1; j++) {
			if (qz[j] != 1)
				hz = (az[j + 1] - az[j]) * (1. - qz[j]) / (1. - pow(qz[j], nz[j]));
			else hz = (az[j + 1] - az[j]) / nz[j];

			tz = az[j];

			for (int k = 0; k < nz[j]; k++) {
				z.push_back(tz);
				tz += hz;
				hz *= qz[j];
			}
		}
		z.push_back(tz);
	}
	void resize_func() {
		G.resize(8);
		M.resize(8);
		C.resize(8);
		local.resize(8);
		p_loc.resize(8);
		c_loc.resize(8);

		for (int i = 0; i < 8; i++) {
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
		for (int i = 0; i < 2; i++) {
			G_small[i].resize(2);
			M_small[i].resize(2);
		}

		//для Слау
		r.resize(2 * m);
		z.resize(2 * m);
		p.resize(2 * m);
		temp1.resize(2 * m);
		temp2.resize(2 * m);


		/*
		local_cond.resize(3);
		local_cond_vl.resize(3);*/

	}

	void read_cond(string cond_file_1, string cond_file_2) {
		ifstream fin1(cond_file_1 + ".txt");
		fin1 >> ncond1;
		cond1.resize(ncond1);
		for (int i = 0; i < ncond1; i++) {
			cond1[i].resize(6);
			for (int j = 0; j < 6; j++)
				fin1 >> cond1[i][j];
		}
		fin1.close();

	}


	void write_small_matrix() {
		G_small[0][0] = 1;
		G_small[0][1] = -1;
		G_small[1][0] = -1;
		G_small[1][1] = 1;

		M_small[0][0] = 2;
		M_small[0][1] = 1;
		M_small[1][0] = 1;
		M_small[1][1] = 2;
	}

	int get_global(int r, int s, int p, int j) { //получение глобального номера по локальному
		int k;
		switch (j) {
		case 0: {
			k = r * count_x * count_y + s * count_x + p;
			break;
		}
		case 1: {
			k = r * count_x * count_y + s * count_x + p + 1;
			break;
		}
		case 2: {
			k = r * count_x * count_y + (s + 1) * count_x + p;
			break;
		}

		case 3: {
			k = r * count_x * count_y + (s + 1) * count_x + p + 1;
			break;
		}
		case 4: {
			k = (r + 1) * count_x * count_y + s * count_x + p;
			break;
		}
		case 5: {
			k = (r + 1) * count_x * count_y + s * count_x + p + 1;
			break;
		}
		case 6: {
			k = (r + 1) * count_x * count_y + (s + 1) * count_x + p;
			break;
		}
		case 7: {
			k = (r + 1) * count_x * count_y + (s + 1) * count_x + p + 1;
			break;
		}
		}
		return k;
	}

	double get_Ugs(double x, double y, double z) {
		return x + y + z;
	}
	double get_Ugc(double x, double y, double z) {
		return x - y - z;
	}

	double func_s(int num_area, double x, double y, double z) {
		double func_loc;
		switch (num_area) {
		case 1: {
			func_loc = -omega * sigma * get_Ugc(x, y, z) - omega * omega * hi * get_Ugs(x, y, z);
			break;
		}
		}
		return func_loc;
	}

	double func_c(int num_area, double x, double y, double z) {
		double func_loc;
		switch (num_area) {
		case 1: {
			func_loc = omega * sigma * get_Ugs(x, y, z) - omega * omega * hi * get_Ugc(x, y, z);
			//func_loc = -6 * x*y*y*y - 6 * x*x*x*y + x * x*x*y*y*y;
			break;
		}
		}
		return func_loc;
	}
	void make_portrait() {
		//списочки для всех глоб узлов
		vector<set<int>> list(m);
		for (int r = 0; r < count_z - 1; r++) {
			for (int s = 0; s < count_y - 1; s++) {
				for (int p = 0; p < count_x - 1; p++) {
					for (int i = 0; i < 8; i++) {

						for (int j = i + 1; j < 8; j++) {
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

		for (int i = 1; i < m; i++) {

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


		for (int i = 1, k = 1; i < m; i++) {
			for (int j : list[i]) {

				jg[k] = 2 * j;
				jg[k + 1] = 2 * j + 1;
				k += 2;
			}
			for (int j : list[i]) {

				jg[k] = 2 * j;
				jg[k + 1] = 2 * j + 1;
				k += 2;
			}
			jg[k] = 2 * i;
			k++;
		}
	}

	void make_local_coor(int r, int s, int p) {
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

	void make_local_matrix(double hx, double hy, double hz) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j <= i; j++) {
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

				if (i != j) {
					G[j][i] = G[i][j];
					M[j][i] = M[i][j];
					p_loc[j][i] = p_loc[i][j];
					c_loc[j][i] = c_loc[i][j];
				}
			}
		}

	}

	void make_local_vec(int num_area) {
		for (int i = 0; i < 8; i++) {
			f[i] = func_c(num_area, x_loc[i], y_loc[i], z_loc[i]);
			fc[i] = 0;
		}

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				fc[i] += f[j] * M[i][j];
			}
		}


		for (int i = 0; i < 8; i++) {
			f[i] = func_s(num_area, x_loc[i], y_loc[i], z_loc[i]);
			fs[i] = 0;
		}

		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				fs[i] += f[j] * M[i][j];
			}
		}
	}

	void clean_matrix() {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				G[i][j] = 0;
				M[i][j] = 0;
				p_loc[i][j] = 0;
				c_loc[i][j] = 0;
			}
			fc[i] = 0;
			fs[i] = 0;
		}
	}

	void make_global() {
		double hx, hy, hz;
		double lyam, gamma;
		for (int r = 0; r < count_z - 1; r++) {
			for (int s = 0; s < count_y - 1; s++) {
				for (int p = 0; p < count_x - 1; p++) {

					hx = x[p + 1] - x[p];
					hy = y[s + 1] - y[s];
					hz = z[r + 1] - z[r];
					int num_area = 1;
					//int num_area=get_area(s,p);
					make_local_coor(r, s, p);
					//get_params(num_area);
					make_local_matrix(hx, hy, hz);

					make_local_vec(num_area);
					add_local(r, s, p);
					clean_matrix();
				}
			}
		}

	}

	void add_first() {
		int r, s, p;
		for (int i = 0; i < ncond1; i++) {
			int x_num_beg = cond1[i][0];
			int x_num_end = cond1[i][1];
			int y_num_beg = cond1[i][2];
			int y_num_end = cond1[i][3];
			int z_num_beg = cond1[i][4];
			int z_num_end = cond1[i][5];
			long B = 1e+5;

			int flag;
			if (z_num_beg == z_num_end) flag = 0;//нижняя и верхняя
			else {
				if (y_num_beg == y_num_end) flag = 1;//передняя и задняя
				else flag = 2; //боковые
			}
			//все работает по номерам в массивах х,y,z
			if (flag == 0) {//нижняя или верхняя				
				r = z_num_beg;
				for (s = oyn[y_num_beg]; s < oyn[y_num_end]; s++) {
					for (p = oxn[x_num_beg]; p < oxn[x_num_end]; p++) {
						//ставим в p и c для 0-3(нижних) узлов
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

			if (flag == 1) {//передняя или задняя				
				s = y_num_beg;
				for (r = ozn[z_num_beg]; r < ozn[z_num_end]; r++) {
					for (p = oxn[x_num_beg]; p < oxn[x_num_end]; p++) {
						//ставим в p и c для 0 1 4 5(передних) узлов
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

			if (flag == 2) {//левая или правая				
				p = x_num_beg;
				for (r = ozn[z_num_beg]; r < ozn[z_num_end]; r++) {
					for (s = oyn[y_num_beg]; s < oyn[y_num_end]; s++) {
						//ставим в p и c для 0 2 4 6(левых) узлов
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

	void add_local(int r, int s, int p) {
		vector<int> global_num(8);
		for (int i = 0; i < 8; i++)
			global_num[i] = 2 * get_global(r, s, p, i);

		for (int i = 0; i < 8; i++) {
			di[global_num[i]] += p_loc[i][i];
			di[global_num[i] + 1] += p_loc[i][i];


			int end0 = ig[global_num[i] + 2] - 1;
			int ind0 = end0;
			ggl[ind0] += c_loc[i][i]; //-c11
			ggu[ind0] -= c_loc[i][i];


			int beg = ig[global_num[i]];
			for (int j = 0; j < i; j++, beg++) {
				int end = ig[global_num[i] + 1] - 1;
				int ind = beg;
				while (jg[ind] != global_num[j]) {
					ind++;
				}
				ggl[ind] += p_loc[i][j]; //p21
				ggl[ind + 1] -= c_loc[i][j]; //-c21
			}

			int beg2 = ig[global_num[i] + 1];
			for (int j = 0; j < i; j++, beg++) {
				int end2 = ig[global_num[i] + 2] - 1;
				int ind2 = beg2;
				while (jg[ind2] != global_num[j]) {
					ind2++;
				}
				ggl[ind2] += c_loc[i][j]; //c21
				ggl[ind2 + 1] += p_loc[i][j]; //p21
			}

			//FOR GGU
			beg = ig[global_num[i]];
			for (int j = 0; j < i; j++, beg++) {
				int end = ig[global_num[i] + 1] - 1;
				int ind = beg;
				while (jg[ind] != global_num[j]) {
					ind++;
				}
				ggu[ind] += p_loc[i][j]; //p21
				ggu[ind + 1] = c_loc[i][j]; //-c21
			}

			beg2 = ig[global_num[i] + 1];
			for (int j = 0; j < i; j++, beg++) {
				int end2 = ig[global_num[i] + 2] - 1;
				int ind2 = beg2;
				while (jg[ind2] != global_num[j]) {
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

	void LU_sq() {
		//копирование-инициализация
		for (int i = 0; i < di.size(); i++) {
			di_new[i] = di[i];
		}
		for (int i = 0; i < ggl.size(); i++) {
			ggl_new[i] = ggl[i];
			ggu_new[i] = ggu[i];
		}

		for (int i = 0; i < di.size(); i++) {
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
	void mult(vector<double> &MV, vector<double> vec) {
		for (int i = 0; i < 2 * m; i++) {
			int k0 = ig[i];
			int k1 = ig[i + 1];
			MV[i] = di[i] * vec[i];
			for (int k = k0; k < k1; k++) {
				int j = jg[k];
				MV[i] += vec[j] * ggl[k];
				MV[j] += vec[i] * ggu[k];
			}
		}
	}
	double skal_mult(vector<double> vec1, vector<double> vec2) {
		double s = 0;
		for (int i = 0; i < vec1.size(); i++) {
			s += vec1[i] * vec2[i];
		}
		return s;
	}
	long double norm(vector<double> vec) {
		double sum = 0;
		for (int i = 0; i < vec.size(); i++)
			sum += vec[i] * vec[i];
		return sqrt(sum);
	}
	void mult_pr(vector<double> aa, vector<double> &y, vector<double> bb) {

		for (int i = 0; i < y.size(); i++) {

			double s = 0; //переменные суммирования

			int i0 = ig[i];//индекс 1го элемента в iтой строке
			int i1 = ig[i + 1];

			for (int k = i0; k < i1; k++) {
				int j = jg[k];
				s += y[j] * aa[k];
			}
			y[i] = (bb[i] - s) / di[i];
		}
	}
	void mult_obr(vector<double> aa, vector<double> &y, vector<double> bb) {
		for (int i = 0; i < y.size(); i++)
			y[i] = bb[i];
		for (int i = y.size() - 1; i >= 0; i--) {
			int i0 = ig[i];//индекс 1го элемента в iтой строке
			int i1 = ig[i + 1];

			y[i] /= di[i];

			for (int k = i1 - 1; k >= i0; k--) {
				int j = jg[k];
				y[j] -= y[i] * aa[k];
			}
		}
	}
	void multU(vector<double> &MV, vector<double>vec) {
		for (int i = 0; i < vec.size(); i++) {
			int k0 = ig[i];
			int k1 = ig[i + 1];
			MV[i] = di[i] * vec[i];
			for (int k = k0; k < k1; k++) {
				int j = jg[k];
				//MV[i] += vec[j] * al[k];
				MV[j] += vec[i] * ggu_new[k];
			}
		}
	}
	void mult_tr(vector<double> &MV, vector<double> vec) {
		for (int i = 0; i < vec.size(); i++) {
			int k0 = ig[i];
			int k1 = ig[i + 1];
			MV[i] = di[i] * vec[i];
			for (int k = k0; k < k1; k++) {
				int j = jg[k];
				MV[i] += vec[j] * ggu[k];
				MV[j] += vec[i] * ggl[k];
			}
		}
	}

	void LOS_sq() {
		double skal1, skal2;
		LU_sq();

		//	инициализация
		mult(temp1, q);
		for (int i = 0; i < 2 * m; i++) {
			temp2[i] = b[i] - temp1[i];
		}
		mult_pr(ggl_new, r, temp2);

		mult_obr(ggu_new, z, r);

		mult(temp1, z);
		mult_pr(ggl_new, p, temp1);

		//iteration
		double nev = skal_mult(r, r);
		int k;
		for (k = 0; k < max_it && nev > err; k++) {


			skal1 = skal_mult(p, r);
			skal2 = skal_mult(p, p);

			double alfa = skal1 / skal2;
			for (int i = 0; i < 2 * m; i++) {

				q[i] += alfa * z[i];
				r[i] -= alfa * p[i];
			}
			mult_obr(ggu_new, temp1, r);
			mult(temp2, temp1);
			mult_pr(ggl_new, temp1, temp2);

			skal1 = skal_mult(p, temp1);

			double beta = -skal1 / skal2;

			mult_obr(ggu_new, temp2, r);

			for (int i = 0; i < 2 * m; i++) {
				z[i] = temp2[i] + beta * z[i];
			}

			for (int i = 0; i < 2 * m; i++) {
				p[i] = temp1[i] + beta * p[i];
			}

			nev = skal_mult(r, r);
			cout << "nev=" << nev << endl;
		}
		cout << "nev=" << nev << endl;
		cout << endl << "iter=" << k << endl;
	};

	void generate_profil() {
		ig_pr.resize(ig.size());
		di_pr.resize(ig.size() - 1);
		ig_pr[0] = 0;
		ig_pr[1] = 0;
		di_pr[0] = di[0];

		ggl_pr.resize(0);
		ggu_pr.resize(0);


		for (int i = 1; i < ig.size() - 1; i++) {
			di_pr[i] = di[i];


			int j0 = jg[ig[i]];
			int j1 = jg[ig[i + 1] - 1];
			int k_r = ig[i + 1] - 1 - ig[i];
			int k = i - j0;//количество элементов в строке профильного
			ig_pr[i + 1] = ig_pr[i] + k;
			for (int j = 0; j < k; j++) {
				ggl_pr.push_back(0);
				ggu_pr.push_back(0);
			}

			int i0 = ig[i];
			int i0_pr = ig_pr[i];
			for (int j = 0; j <= k_r; j++) {
				ggl_pr[i0_pr + jg[i0 + j] - j0] = ggl[i0 + j];
				ggu_pr[i0_pr + jg[i0 + j] - j0] = ggu[i0 + j];
			}

		}

	}

	void LU_mult_pr() {

		for (int i = 0; i < q.size(); i++) {
			double s = 0; //переменная суммирования
			int j0 = i - (ig_pr[i + 1] - ig_pr[i]); //индекс 1го элемента в iтой строке
			int k = ig_pr[i];
			for (int j = j0; j < i; j++, k++) {
				s += temp1[j] * ggl_pr[k];
			}
			temp1[i] = b[i] - s;
		}
	}
	void LU_mult_obr() {
		for (int i = 0; i < q.size(); i++)
			q[i] = temp1[i];
		for (int i = q.size() - 1; i >= 0; i--) { //i-номер столбца
			int j0 = i - (ig_pr[i + 1] - ig_pr[i]); //индекс 1го элемента в iтом столбце
			int k = ig_pr[i + 1] - 1;
			q[i] /= di_pr[i];
			for (int j = i - 1; j >= j0; j--, k--) {

				q[j] -= q[i] * ggu_pr[k];
			}
		}
	}
	void LU() {
		for (int i = 0; i < q.size(); i++) {
			double s1 = 0, s2 = 0, s3 = 0; //переменные суммирования
			int j0 = i - (ig_pr[i + 1] - ig_pr[i]);
			int k = ig_pr[i];
			int j, p0;
			for (j = j0; j < i; j++, k++) {
				s1 = 0, s2 = 0;
				int i0 = j - (ig_pr[j + 1] - ig_pr[j]);
				if (i0 > j0) p0 = i0;
				else p0 = j0;
				for (int p = p0; p < j; p++) {
					s1 += ggu_pr[ig_pr[j] + p - i0] * ggl_pr[ig_pr[i] + p - j0];
					s2 += ggl_pr[ig_pr[j] + p - i0] * ggu_pr[ig_pr[i] + p - j0];
				}
				ggl_pr[k] -= s1;
				ggu_pr[k] -= s2;
				ggl_pr[k] /= di_pr[j];
			}
			for (int p = j0; p < j; p++) {
				s3 += ggu_pr[ig_pr[i] + p - j0] * ggl_pr[ig_pr[i] + p - j0];
			}
			di_pr[i] -= s3;
		}
	}


	void mult_L_pr_on_vec(vector<double> vec, vector<double> &res)
	{
		for (int i = 0; i < res.size(); i++)
		{
			res[i] = vec[i];
			int N = ig_pr[i + 1] - ig_pr[i]; // количество элементов в строке
			int col_one_elem = i - N; // номер столбца первого элемента
			for (int j = 0; j < N; j++)
				res[i] += ggl_pr[ig_pr[i] + j] * vec[col_one_elem + j];
		}
	}
	void mult_U_pr_on_vec(vector<double> vec, vector<double> &res)
	{
		for (int i = 0; i < res.size(); i++)
		{
			res[i] = di_pr[i] * vec[i];
			int N = ig_pr[i + 1] - ig_pr[i]; // количество элеметов в столбце
			int row_one_elem = i - N; // номер строки первого элемента
			for (int j = 0; j < N; j++)
				res[row_one_elem + j] += ggu_pr[ig_pr[i] + j] * vec[i];
		}
	}

	void LU_sol() {
		//перегенерация в профильный
		generate_profil();
		//ЛУ-разложение для профильного
		LU();
		//decompositionLU();
		LU_mult_pr();
		LU_mult_obr();
		mult_U_pr_on_vec(q, temp1);
		mult_L_pr_on_vec(temp1, temp2);
		for (int i = 0; i < q.size(); i++)
			temp2[i] = temp2[i] - b[i];
		//cout << q[i] << "\n";
		long double nev = (norm(temp2)) / norm(b);
		cout << "nev=" << nev << endl;
		ofstream fout("results.csv", ios_base::app); // открываем файл для добавления информации к концу файла
		fout << nev << "\n";
		fout.close();
	}

	void do_smth() {
		for (int i = 0; i < q.size(); i++)
			q[i] = 0;
		clean_matrix();
		st_time = clock();
		write_small_matrix();
		make_portrait();
		make_global();

		add_first();
		if (flag == 0)
			LOS_sq();
		else LU_sol();
		end_time = clock(); // время работы программы
		prog_time = end_time - st_time;
		cout << "time=" << prog_time / 1000 << endl << endl;
	}
};

int main() {
	SLAE slv = SLAE("net", "cond1", "cond2", 0);


	slv.omega = 1e+4;



	cout << "===========lyam=1e+2============" << endl;
	slv.lyam = 100;
	{
		cout << "__________sigma=0_____________" << endl;
		slv.sigma = 10;
		cout << endl << "8.81e-12" << endl;
		slv.hi = 1e-10;
		slv.do_smth();
		cout << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();
		cout << endl << "1e-10" << endl;
		slv.hi = 1e-10;
		slv.do_smth(); }
	{cout << "__________sigma=1e4_____________" << endl;
	slv.sigma = 1e+4;
	cout << endl << "8.81e-12" << endl;
	slv.hi = 8.81e-12;
	slv.do_smth();
	cout << endl << "1e-11" << endl;
	slv.hi = 1e-11;
	slv.do_smth();
	cout << endl << "1e-10" << endl;
	slv.hi = 1e-10;
	slv.do_smth(); }
	{cout << "__________sigma=1e8_____________" << endl;
	slv.sigma = 1e+8;
	cout << endl << "8.81e-12" << endl;
	slv.hi = 8.81e-12;
	slv.do_smth();
	cout << endl << "1e-11" << endl;
	slv.hi = 1e-11;
	slv.do_smth();
	cout << endl << "1e-10" << endl;
	slv.hi = 1e-10;
	slv.do_smth(); }


	cout << "===========lyam=1e+4=========" << endl;
	slv.lyam = 1e+4;
	{
		cout << "__________sigma=0_____________" << endl;
		slv.sigma = 0;
		cout << endl << "8.81e-12" << endl;
		slv.hi = 8.81e-12;
		slv.do_smth();
		cout << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();
		cout << endl << "1e-10" << endl;
		slv.hi = 1e-10;
		slv.do_smth(); }
	{cout << "__________sigma=1e4_____________" << endl;
	slv.sigma = 1e+4;
	cout << endl << "8.81e-12" << endl;
	slv.hi = 8.81e-12;
	slv.do_smth();
	cout << endl << "1e-11" << endl;
	slv.hi = 1e-11;
	slv.do_smth();
	cout << endl << "1e-10" << endl;
	slv.hi = 1e-10;
	slv.do_smth(); }
	{cout << "__________sigma=1e8_____________" << endl;
	slv.sigma = 1e+8;
	cout << endl << "8.81e-12" << endl;
	slv.hi = 8.81e-12;
	slv.do_smth();
	cout << endl << "1e-11" << endl;
	slv.hi = 1e-11;
	slv.do_smth();
	cout << endl << "1e-10" << endl;
	slv.hi = 1e-10;
	slv.do_smth(); }

	cout << "============lyam=8e+5==============" << endl;
	slv.lyam = 8e+5;
	{
		cout << "__________sigma=0_____________" << endl;
		slv.sigma = 0;
		cout << endl << "8.81e-12" << endl;
		slv.hi = 8.81e-12;
		slv.do_smth();
		cout << endl << "1e-11" << endl;
		slv.hi = 1e-11;
		slv.do_smth();
		cout << endl << "1e-10" << endl;
		slv.hi = 1e-10;
		slv.do_smth(); }
	{cout << "__________sigma=1e4_____________" << endl;
	slv.sigma = 1e+4;
	cout << endl << "8.81e-12" << endl;
	slv.hi = 8.81e-12;
	slv.do_smth();
	cout << endl << "1e-11" << endl;
	slv.hi = 1e-11;
	slv.do_smth();
	cout << endl << "1e-10" << endl;
	slv.hi = 1e-10;
	slv.do_smth(); }
	{cout << "__________sigma=1e8_____________" << endl;
	slv.sigma = 1e+8;
	cout << endl << "8.81e-12" << endl;
	slv.hi = 8.81e-12;
	slv.do_smth();
	cout << endl << "1e-11" << endl;
	slv.hi = 1e-11;
	slv.do_smth();
	cout << endl << "1e-10" << endl;
	slv.hi = 1e-10;
	slv.do_smth(); }



	ofstream fout("out.txt");
	for (int i = 0; i < slv.m * 2; i++)
		fout << slv.q[i] << endl;
	//cout << "time=" << slv.end_time/1000 ;
	system("pause");
	return 0;

}
