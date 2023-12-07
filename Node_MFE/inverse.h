#ifndef INVERSE_H
#define INVERSE_H
#include "fem.h"
#include <cassert>
class sensor
{
public:
	point position_N;
	point position_M;
	double value;
	double trust;
};
class parameter_power_2
{
public:
	inline double get_value();
	inline void set_value(double val);
	inline static double get_mid();
	double power;
	point center;
};
template<typename param_type>
class inverse
{
public:
	FEM fem;
	inverse();
	~inverse();
	void calc();
private:
	size_t sens_num;
	sensor* sens;
	size_t param_num;
	param_type* param;
	void precond();
	void read_sensors();
	void read_parameters();
	double** A;
	double* b;
	double* c;
	void solve_gauss();
	void solve_fem();
	double get_line_values(size_t line_n) const;
	double get_line_values(size_t line_n, size_t param_n) const;
	double get_line_values_real(size_t line_num) const;
	double rand_gaussian(double mu, double sigma2);
};
// ============================================================================
const double SLAE_EPSILON = 1e-40;
const double INV_EPSILON = 1e-16;
const double DELTA = 0.05;
const double POWER_MID = 1.0;
// ============================================================================
inline double parameter_power_2::get_value()
{
	return power;
}
inline void parameter_power_2::set_value(double val)
{
	power = val;
}
inline double parameter_power_2::get_mid()
{
	return POWER_MID;
}
// Gaussian (normal) distribution generator
// x ~ N (mu, sigma^2)
// mu - mean (location)
// sigma^2 - variance (squared scale)
template<typename param_type>
double inverse<param_type>::rand_gaussian(double mu, double sigma2)
{
	double alpha1 = (double)rand() / (double)RAND_MAX;
	double alpha2 = (double)rand() / (double)RAND_MAX;
	double ksi = sqrt(-2.0 * log(alpha1)) * cos(2.0 * 3.1415926535897932 * alpha2);
	return mu + sqrt(sigma2) * ksi;
}
// Получение данных с датчиков
template<typename param_type>
void inverse<param_type>::read_sensors()
{
	sens_num = 6; //количество приемников менять тут для тестов
	sens = new sensor[sens_num];
	sens[0].position_M = point(45.0, 60.0);//координаты приемников x и y
	sens[0].position_N = point(75.0, 60.0);
	if (sens_num > 1) sens[1].position_M = point(-360.0, 245.0);
	if (sens_num > 1) sens[1].position_N = point(-360.0, 275.0);
	if (sens_num > 2) sens[2].position_M = point(-180.0, -165.0);
	if (sens_num > 2) sens[2].position_N = point(-180.0, -195.0);
	if (sens_num > 3) sens[3].position_M = point(240.0, -225.0);
	if (sens_num > 3) sens[3].position_N = point(240.0, -255.0);
	if (sens_num > 4) sens[4].position_M = point(-120.0, 105.0);
	if (sens_num > 4) sens[4].position_N = point(-120.0, 135.0);
	if (sens_num > 5) sens[5].position_M = point(300.0, 285.0);
	if (sens_num > 5) sens[5].position_N = point(300.0, 315.0);
	sens[0].trust = 1.0;
	if (sens_num > 1) sens[1].trust = 1.0;
	if (sens_num > 2) sens[2].trust = 1.0;
	if (sens_num > 3) sens[3].trust = 1.0;
	if (sens_num > 4) sens[4].trust = 1.0;
	if (sens_num > 5) sens[5].trust = 1.0;
	for (size_t i = 0; i < sens_num; i++)
		sens[i].value = get_line_values_real(i);
	size_t noise_num = 1;
	sens[noise_num].trust = 0.7;
	sens[noise_num + 1].trust = 1;
	srand(time(0));  // Initialize random number generator.
	double noise_value = ((rand() % 10) + 1) / 100;
	sens[noise_num].value += rand_gaussian(0.0, fabs(noise_value * sens[noise_num].value) * fabs(noise_value * sens[noise_num].value));
	sens[noise_num + 1].value += rand_gaussian(0.0, fabs(noise_value * sens[noise_num + 1].value) * fabs(noise_value * sens[noise_num + 1].value));
}
// Получение неизвестных параметров мощности
template<>
void inverse<parameter_power_2>::read_parameters()
{
	param_num = 10;
	param = new parameter_power_2[param_num];
	double sources_position[10][2] =
	{ // координаты источников x и y
	{0, 0}, // 0
	{0, -15}, // 1
	{0, 15}, // 2
	{0, -90}, // 3
	{0, -60}, // 4
	{0, 60}, // 5
	{0, -45}, // 6
	{0, -30}, // 7
	{0, 30}, // 8
	{0, 45} // 9
	};
	for (size_t i = 0; i < param_num; i++)
	{
		param[i].center.x = sources_position[i][0];
		param[i].center.y = sources_position[i][1];
		param[i].power = POWER_MID;
	}
	fem.pss[0].second = 1.0 / (2.0 * M_PI * fem.pss[0].first.r);
	fem.pss[1].second = -1.0 / (2.0 * M_PI * fem.pss[1].first.r);
}
template<>
double inverse<parameter_power_2>::get_line_values(size_t line_n, size_t param_n) const
{
	double xM = sens[line_n].position_M.x - param[param_n].center.x;
	double xN = sens[line_n].position_N.x - param[param_n].center.x;
	double yM = sens[line_n].position_M.y - param[param_n].center.y;
	double yN = sens[line_n].position_N.y - param[param_n].center.y;
	double rM = sqrt(xM * xM + yM * yM);
	double rN = sqrt(xN * xN + yN * yN);
	double VM = fem.get_solution(point(rM, 1e-5));
	double VN = fem.get_solution(point(rN, 1e-5));
	return param[param_n].power * (VN - VM);//(VM - VN);
}
template<>
double inverse<parameter_power_2>::get_line_values(size_t line_n) const
{
	double solution = 0.0;
	for (size_t i = 0; i < param_num; i++)
		solution += get_line_values(line_n, i);
	return solution;
}
// ============================================================================
template<typename param_type>
double inverse<param_type>::get_line_values_real(size_t line_num) const
{
	double rM = sqrt(sens[line_num].position_M.x * sens[line_num].position_M.x +
		sens[line_num].position_M.y * sens[line_num].position_M.y);
	double rN = sqrt(sens[line_num].position_N.x * sens[line_num].position_N.x +
		sens[line_num].position_N.y * sens[line_num].position_N.y);
	double VM = fem.get_solution(point(rM, 1e-5));
	double VN = fem.get_solution(point(rN, 1e-5));
	return VN - VM;//VM - VN;
}
template<typename param_type>
inverse<param_type>::inverse()
{
	sens_num = 0;
	sens = NULL;
	param_num = 0;
	param = NULL;
	A = NULL;
	b = NULL;
	c = NULL;
}
template<typename param_type>
inverse<param_type>::~inverse()
{
	if (sens) delete[] sens;
	if (param) delete[] param;
	if (b) delete[] b;
	if (c) delete[] c;
	if (A)
	{
		for (size_t i = 0; i < param_num; i++)
			delete[] A[i];
		delete[] A;
	}
}
// Решение обратной задачи
template<typename param_type>
void inverse<param_type>::calc()
{
	precond();
	read_sensors();
	read_parameters();
	solve_fem();
	cout.precision(17);
	for (size_t j = 0; j < sens_num; j++)
		cout << get_line_values_real(j) << " ";
	cout << endl;
	for (size_t j = 0; j < sens_num; j++)
		cout << sens[j].value << " ";
	cout << endl;
	for (size_t i = 0; i < param_num; i++)
	{
		for (size_t j = 0; j < sens_num; j++)
			cout << get_line_values(j, i) << " ";
		cout << endl;
	}
	// Матрица для обратной задачи
	A = new double* [param_num];
	for (size_t i = 0; i < param_num; i++)
		A[i] = new double[param_num];
	b = new double[param_num];
	c = new double[param_num];
	// Заполнение основной матрицы СЛАУ и правой части (без альфы)
	memset(b, 0, sizeof(double) * param_num);
	for (size_t i = 0; i < param_num; i++)
	{
		memset(A[i], 0, sizeof(double) * param_num);
		for (size_t j = 0; j < param_num; j++)
		{
			for (size_t k = 0; k < sens_num; k++)
			{
				double w2 = (sens[k].trust * sens[k].trust) / (sens[k].value * sens[k].value);
				A[i][j] += w2 * get_line_values(k, i) * get_line_values(k, j);
			}
		}
		for (size_t k = 0; k < sens_num; k++)
		{
			double w2 = (sens[k].trust * sens[k].trust) / (sens[k].value * sens[k].value);
			b[i] += w2 * sens[k].value * get_line_values(k, i);
		}
	}
	// Magic-magic
	double alpha = 1e-20;
	while (alpha < 1.0)
	{
		// Учет альфы
		for (size_t i = 0; i < param_num; i++)
		{
			A[i][i] += alpha;
		}
		// Решаем СЛАУ
		solve_gauss();
		//cout << get_residual() << endl;
		for (size_t i = 0; i < param_num; i++)
		{
			cout << "param[" << i << "] = " << c[i] << endl;
			param[i].set_value(c[i]);
		}
		double functional = 0.0;
		for (size_t i = 0; i < sens_num; i++)
		{
			double w2 = (sens[i].trust * sens[i].trust) / (sens[i].value * sens[i].value);
			double sum = (sens[i].value - get_line_values(i));
			functional += w2 * sum * sum;
		}
		cout << "Functional (" << alpha << "): " << functional << endl;
		alpha *= 1.1;
	}
}
// Инициализация
template<typename param_type>
void inverse<param_type>::precond()
{
	fem.input();
	fem.make_portrait();
	solve_fem();
}
// Решение прямой задачи
template<typename param_type>
void inverse<param_type>::solve_fem()
{
	fem.assembling_global();
	fem.applying_sources();
	fem.applying_bound();
	fem.slae.solve(SLAE_EPSILON);
}
// Метод Гаусса
template<typename param_type>
void inverse<param_type>::solve_gauss()
{
	int n = (int)param_num;
	double** A = new double* [param_num];
	for (size_t i = 0; i < param_num; i++)
	{
		A[i] = new double[param_num];
		memcpy(A[i], this->A[i], param_num * sizeof(double));
	}
	double* b = new double[param_num];
	memcpy(b, this->b, param_num * sizeof(double));
	//верхний треугольный вид
	for (int i = 0; i < n; i++)
	{
		if (!A[i][i])
		{
			bool flag = false;
			for (int j = i + 1; j < n && !flag; j++)
				if (A[j][i])
				{
					for (int k = i; k < n; k++)
					{
						double tmp = A[i][k];
						A[i][k] = A[j][k];
						A[j][k] = tmp;
					}
					double tmp = b[i];
					b[i] = b[j];
					b[j] = tmp;
					flag = true;
				}
		}
		b[i] = b[i] / A[i][i];
		for (int j = n - 1; j >= i; j--)
			A[i][j] = A[i][j] / A[i][i];
		for (int j = i + 1; j < n; j++)
		{
			b[j] -= b[i] * A[j][i];
			for (int k = n - 1; k >= i; k--)
				A[j][k] -= A[i][k] * A[j][i];
		}
	}
	//диагональный вид
	
		for (int i = n - 1; i > 0; i--)
			for (int j = i - 1; j >= 0; j--)
				b[j] -= A[j][i] * b[i];
	for (int i = 0; i < n; i++)
		c[i] = b[i];
	delete[] b;
	for (size_t i = 0; i < param_num; i++)
		delete[] A[i];
	delete[] A;
}
#endif // INVERSE_H