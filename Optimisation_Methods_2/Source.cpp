#include "PolytopeMethod.h"
#include "Pirson3Method.h"
#include <fstream>
#include <conio.h>

double func(vector<double> &x, char key)
{
	double A1 = 2, A2 = 3, a1 = 1, b1 = 2, c1 = 1, d1 = 2,  a2 = 1, b2 = 3, c2 = 3, d2 = 3;
	switch (key)
	{
		case '1':
			// квадратичная функция; ответ (1; 1)
			return 100 * (x[1] - x[0]) * (x[1] - x[0]) + (1 - x[0]) * (1 - x[0]);
			break;
		case '2':
			// функция Розенброка; ответ (1; 1)
			return 100 * (x[1] - x[0] * x[0]) * (x[1] - x[0] * x[0]) + (1 - x[0]) * (1 - x[0]);
			break;
		case '3':
			// заданная f(x)
			// т.к. максимизируем f(x), то минимизировать будем -f(x)
			// тут задано -f(x) нечетных вариантов
			// константы варианта 7
			// ответ
			return -(A1 * exp(-((x[0] - a1) / b1) * ((x[0] - a1) / b1) - ((x[1] - c1) / d1) * ((x[1] - c1) / d1)) + A2 * exp(-((x[0] - a2) / b2) * ((x[0] - a2) / b2) - ((x[1] - c2) / d2) * ((x[1] - c2) / d2)));
			break;
		case '4':
			// простейшая функция; ответ (7; 1)
			return (x[0] - 7) * (x[0] - 7) + (x[1] - 1) * (x[1] - 1);
			break;
		default:
			return 0;
			break;
	}

}

int main()
{
	cout << "Choose the function: 1 Quadratic, 2 Rosenbrock, 3 Defined, 4 Simple." << endl;
	char key = _getch();
	cout.precision(10);
	ofstream out("out.txt");
	out.precision(10);
	out.imbue(locale("Russian"));
	PolytopeMethod t;
	Pirson3Method tt;
	vector<double> v(2);
	v[0] = 5;
	v[1] = -5;
	cout << "Choose the method: 1 Deformable polyhedron, 2 Pirson" << endl;
	char key_main = _getch();
	int iter;
	switch (key_main)
	{
		case '1':
			// метод деформируемого многогранника
			t.key = key;
			iter = t.Algorithm(v);
			
			out << "Polytope method" << endl;
			out << "Iterations: " << iter << endl;
			out << "Result: " << v[0] << " " << v[1] << endl;
			out << "FUNC: " << func(v, key) << endl;
			break;
		case '2':
			//метод Пирсона(3)
			tt.key = key;
			iter = tt.Algorithm(v);
			
			out << iter << "\t \t" << v[0] << "\t" << v[1] << "\t" << func(v, key) << endl;
			out << "Pirson method" << endl;
			out << "Iterations: " << iter << endl;
			out << "Result: " << v[0] << "\t" << v[1] << endl;
			out << "FUNC: " << func(v, key) << endl;
			break;
		default:
			break;
	}
	cout << "Check your txt file for data" << endl;
	
	/*cout << "Pirson method" << endl;
	cout << "Iterations: " << iter << endl;
	cout << "Result: " << v[0] << " " << v[1] << endl;
	cout << "FUNC: " << func(v) << endl;*/
	system("pause");
	return 0;
}