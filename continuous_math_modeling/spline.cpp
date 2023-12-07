#include <cstdlib> 
#include <cmath> 
#include <limits> 
#include <fstream>
#include <iostream>
#include "mfe.h"
#include <vector>

using namespace std;

class cubic_spline
{
public:
	// Структура, описывающая сплайн на каждом сегменте сетки 
	struct spline_tuple
	{
		double a, b, c, d, x;
	};

	//spline_tuple* splines; // Сплайн 
	vector<spline_tuple> splines;
	size_t n = 0; // Количество узлов сетки 
	double* M; //производные
	double left_b, right_b; //область оценки сплайна
	
	double* x, *y;

	void free_mem(); // Освобождение памяти 


	cubic_spline(); //конструктор 
	~cubic_spline(); //деструктор 

	void input();
	// Построение сплайна 
	// x - узлы сетки, должны быть упорядочены по возрастанию, кратные узлы запрещены 
	// y - значения функции в узлах сетки 
	// n - количество узлов сетки 
	void build_spline_Hermit(const double* x, const double* y, size_t n, double* M);
	void gap_check();
	void build_spline(const double* x, const double* y, size_t n);
	void output();
	// Вычисление значения интерполированной функции в произвольной точке 
	double f(double x) const;
	double f_Hermit(double x) const;
	void Calculate();
};

//cubic_spline::cubic_spline() /*: splines(NULL)*/
//{
//
//}
//
//cubic_spline::~cubic_spline()
//{
//	free_mem();
//}

void cubic_spline::input()
{
	size_t temp;
	ifstream in("input.txt");

	in >> temp;
	/*double t1, t2;
	
	while (!in.eof())
	{
		in >> t1 >> t2;
		cout << t1 << "\t" << t2 << endl;
	}*/

	n = temp;
	x = new double[n];
	y = new double[n];
	for (int i = 0; i < n; i++)
	{
		in >> x[i] >> y[i];
	}

	/*for (int i = 0; i < n; i++)
	{
		y[i] = mfe::FuncInPoint(x[i], y[i]);
	}*/

	in.close();
	M = new double[n];
	in.open("d_input.txt");
	for (int i = 0; i < n; i++)
	{
		in >> M[i];
	}

}

void cubic_spline::build_spline_Hermit(const double* x, const double* y, size_t n, double *M)
{
	free_mem();
	//this->n = n;
	n = this->n;
	x = this->x;
	y = this->y;

	//splines = new spline_tuple[n];
	splines.resize(n);
	for (size_t i = 0; i < n - 1; ++i)
	{
		splines[i].x = x[i];
		splines[i].a = y[i];
		splines[i].b = M[i];
		splines[i].c = (3 * y[i + 1] - 3 * y[i] - M[i + 1] * (x[i + 1] - x[i]) - 2 * M[i] * (x[i + 1] - x[i])) / pow(x[i + 1] - x[i], 2);
		splines[i].d = (2 * y[i] - 2 * y[i + 1] + M[i + 1] * (x[i + 1] - x[i]) + M[i] * (x[i + 1] - x[i])) / pow(x[i + 1] - x[i], 3);
	}
	splines[n - 1].x = x[n - 1];
	splines[n - 1].a = y[n - 1];
	splines[n - 1].b = M[n - 1];
	splines[n - 1].c = splines[n - 2].c;
	splines[n - 1].d = splines[n - 2].d;

}

void cubic_spline::gap_check()
{
	double eps = 1e-5;
	double delta = 1e-15;
	//cout << splines[0].x;
	splines.clear();
	//splines.erase(splines.begin() + 1);
	/*for (int i = 0; i < n; i++)
	{
		if (i >= splines.size())
			return;
		if (fabs(f_Hermit(x[i] - delta) - f_Hermit(x[i] + delta)) > eps)
		{
			cout << i << endl;
			splines.erase(splines.begin() + i);
		}
	}*/
}

double cubic_spline::f_Hermit(double x) const
{
	
	if (splines.size() == 0)
		return numeric_limits<double>::quiet_NaN(); // Если сплайны ещё не построены - возвращаем NaN 
	spline_tuple s;
	if (x <= splines[0].x) // Если x меньше точки сетки x[0] - пользуемся первым эл-том массива 
		s = splines[0];
	//s = splines + 1;
	else if (x >= splines[splines.size() - 1].x) // Если x больше точки сетки x[n - 1] - пользуемся последним эл-том массива
		s = splines[splines.size() - 1];
		//s = splines + n - 1;
	else // Иначе x лежит между граничными точками сетки - производим бинарный поиск нужного эл-та массива 
	{
		size_t i = 0, j = splines.size() - 1;
		while (i + 1 < j)
		{
			size_t k = i + (j - i) / 2;
			if (x <= splines[k].x)
				j = k;
			else
				i = k;
		}
		s = splines[j];
		//s = splines + j;
	}
	double dx = (x - s.x);
	return s.a + s.b * dx + s.c * pow(dx, 2) + s.d * pow(dx, 3);
	/*double dx = (x - s->x);
	return s->a + s->b * dx + s->c * pow(dx, 2) + s->d * pow(dx, 3);*/
	//return s->a + (s->b + (s->c / 2. + s->d * dx / 6.) * dx) * dx; // Вычисляем значение сплайна в заданной точке. 
}


void cubic_spline::build_spline(const double* x, const double* y, size_t n)
{
	free_mem();
	//this->n = n;
	n = this->n;
	x = this->x;
	y = this->y;
	// Инициализация массива сплайнов 
	//cout << n;
	splines.resize(n);
	//splines = new spline_tuple[n];
	
	for (size_t i = 0; i < n; ++i)
	{
		splines[i].x = x[i];
		splines[i].a = y[i];
	}
	splines[0].c = 0.;

	// Решение СЛАУ относительно коэффициентов сплайнов c[i] методом прогонки для трехдиагональных матриц 
	// Вычисление прогоночных коэффициентов - прямой ход метода прогонки 
	double* alpha = new double[n - 1];
	double* beta = new double[n - 1];
	double A, B, C, F, h_i, h_i1, z;
	alpha[0] = beta[0] = 0.;
	for (size_t i = 1; i < n - 1; ++i)
	{
		h_i = x[i] - x[i - 1], h_i1 = x[i + 1] - x[i];
		A = h_i;
		C = 2. * (h_i + h_i1);
		B = h_i1;
		F = 6. * ((y[i + 1] - y[i]) / h_i1 - (y[i] - y[i - 1]) / h_i);
		z = (A * alpha[i - 1] + C);
		alpha[i] = -B / z;
		beta[i] = (F - A * beta[i - 1]) / z;
	}

	splines[n - 1].c = (F - A * beta[n - 2]) / (C + A * alpha[n - 2]);

	// Нахождение решения - обратный ход метода прогонки 
	for (size_t i = n - 2; i > 0; --i)
		splines[i].c = alpha[i] * splines[i + 1].c + beta[i];

	// Освобождение памяти, занимаемой прогоночными коэффициентами 
	delete[] beta;
	delete[] alpha;

	// По известным коэффициентам c[i] находим значения b[i] и d[i] 
	for (size_t i = n - 1; i > 0; --i)
	{
		double h_i = x[i] - x[i - 1];
		splines[i].d = (splines[i].c - splines[i - 1].c) / h_i;
		splines[i].b = h_i * (2. * splines[i].c + splines[i - 1].c) / 6. + (y[i] - y[i - 1]) / h_i;
	}
	//cout << n << "   " << this->n;
}

double cubic_spline::f(double x) const
{
	//if (!splines)
	if(splines.size() == 0)
		return numeric_limits<double>::quiet_NaN(); // Если сплайны ещё не построены - возвращаем NaN 

	spline_tuple s;
	//vector<spline_tuple> s;
	if (x <= splines[0].x) // Если x меньше точки сетки x[0] - пользуемся первым эл-том массива 
		s = splines[0];
	//s = splines + 1;
	else if (x >= splines[n - 1].x) // Если x больше точки сетки x[n - 1] - пользуемся последним эл-том массива
		s = splines[n - 1];
		//s = splines + n - 1;
	else // Иначе x лежит между граничными точками сетки - производим бинарный поиск нужного эл-та массива 
	{
		size_t i = 0, j = n - 1;
		while (i + 1 < j)
		{
			size_t k = i + (j - i) / 2;
			if (x <= splines[k].x)
				j = k;
			else
				i = k;
		}
		s = splines[j];
		//s = splines + j;
	}
	double dx = (x - s.x);
	return s.a + (s.b + (s.c / 2. + s.d * dx / 6.) * dx) * dx;
	/*double dx = (x - s->x);
	return s->a + (s->b + (s->c / 2. + s->d * dx / 6.) * dx) * dx;*/ // Вычисляем значение сплайна в заданной точке. 
}

void cubic_spline::Calculate()
{
	input();
	//double h = (right_b - left_b) / (double)(100. * n - 1);
	build_spline_Hermit(x, y, n, M);
	//build_spline(x, y, n);
	gap_check();
	//double temp = left_b;
	//cout << (right - left);
	//for (int i = 0; i < n * 100; i++)
	//{
	//	//cout << temp << "    " << f(temp) << endl;
	//	temp += h;
	//}
	output();
	//free_mem();
	//~cubic_spline();
}

void cubic_spline::output()
{
	//size_t temp;
	left_b = 0.0;
	right_b = 5.0;
	double h = (right_b - left_b) / (double)(100. * splines.size() - 1);
	ofstream out("output.csv");
	double temp = left_b;
	//out << temp;
	//n = temp;
	//x = new double[n];
	//y = new double[n];
	//out << n;
	//cout << right << "     " << left << endl;
	for (int i = 0; i < splines.size() * 100; i++)
	{
		//cout << (right_b - left_b) << endl;
		out << temp << ";" << f_Hermit(temp) /*<< ";"*/ << endl;
		temp += h;
	}
	out.close();
}

void cubic_spline::free_mem()
{
	splines.resize(0);
	//delete[] splines;
	//splines = NULL;
}