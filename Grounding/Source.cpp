#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <vector>
#include <set>
#include <ctime>
#include <string>


using namespace std;

int N = 8; //количество электродов
int m = 7; //
double ro; //удельное сопротивление
double **A; //матрица концов электродов и диаметров
int k = 5; //количество точек заземления
double **ground; //матрица точек заземления
double Ivvod = 0;//введеная сила тока

class Electrod //основной класс
{
public:
	Electrod();
	~Electrod();
	Electrod(double P1[3], double P2[3], double d);
	double* Center();
	void Rod_length();
	void Rod_height();
	void testing();
	double Resistance_Calculate();

private:
	double d = 0;
	double Rod_1[3] = { 0 }; //края стержней
	double Rod_2[3] = { 0 };
	double l = 0; //длина стержня
	int type = 0;
	double h= 0; //глубина или высота стержня
};

Electrod::Electrod() {} //конструктор по умолчанию

Electrod::Electrod(double P1[3], double P2[3], double d) //конструктор
{
	//this ->X = X;
	//this ->Y = Y;
	//this ->Z = Z;
	for (int i = 0; i < 3; i++)
	{
		Rod_1[i] = P1[i];//края электродов
		Rod_2[i] = P2[i];
	}
	this ->d = d;
}

Electrod::~Electrod() {}//деструктор

double* Electrod::Center()
{
	double C[3];
	for (int i = 0; i < 3; i++)
		C[i] = (Rod_1[i] + Rod_2[i]) / 2.0;

	return C;
}

void Electrod::Rod_length()
{
	double C[3];
	l = sqrt(pow(Rod_2[0] - Rod_1[0], 2) + pow(Rod_2[1] - Rod_1[1], 2) + pow(Rod_2[2] - Rod_1[2], 2));
}

void Electrod::Rod_height()
{
	double C[3];
	switch (type)
	{
	case(1):
		h = max(Rod_1[2], Rod_2[2]);
		break;
	case(2):
		h = min(abs(Rod_1[2]), abs(Rod_2[2]));
		break;
	case(3):
		h = min(abs(Rod_1[2]), abs(Rod_2[2]));
	case(4):
		h = min(abs(Rod_1[2]), abs(Rod_2[2]));
	default:
		break;
	}
}

double Electrod::Resistance_Calculate()
{
	testing();
	Rod_length();
	Rod_height();

	double R = 0;
	double h_ = l / 2.0 + h;
	switch (type)
	{
		case(1):
			R = (ro / (2.0 * M_PI * l)) * log(4.0 * l / d);
			break;
		case(2):
			R = (ro / (2.0 * M_PI * l)) *(log(2.0 * l / d) + 0.5 * log((4.0 * h_ + l) / (4.0 * h_ - l)));
			break;
		case(3):
			R = ro / (2.0 * M_PI * l) * (log((2.0 * l * l) / (d * h)));
		case(4):
			R = ro / (2.0 * M_PI * l) * (log((2.6 * l * l) / (d * h)));
	default:
		break;
	} 
	return R;
}


void input() //ввод
{
	string record;
	ifstream in("input.csv");
	setprecision(15);
	in >> record;
	N = stoi(record.substr(0, record.find(';'))); record.erase(0, record.find(';') + 1);
	m = stoi(record.substr(0, record.find(';'))); record.erase(0, record.find(';') + 1);
	Ivvod = stod(record.substr(0, record.find(';'))); record.erase(0, record.find(';') + 1);
	ro = stod(record.substr(0, record.find(';'))); record.erase(0, record.find(';') + 1);

	//in >> A1 >> N >> ro;
	//cout <<ro << endl;
	A = new double *[N];

	for (int i = 0; i < N; i++)//массив краев элетродов и диаметров
	{
		A[i] = new double[m];
		in >> record;
		for (int j = 0; j < m; j++)
		{
			A[i][j] = stod(record.substr(0, record.find(';'))); record.erase(0, record.find(';') + 1);
			/*cout << A[i][j];*/
		}
		//cout << endl;
	}

	in >> record; //количество точек заземления
	k = stoi(record.substr(0, record.find(';'))); record.erase(0, record.find(';') + 1);
	ground = new double*[k];

	for (int i = 0; i < k; i++)
	{
		in >> record;
		ground[i] = new double[2];
		
		ground[i][0] = stod(record.substr(0, record.find(';'))); record.erase(0, record.find(';') + 1);
		ground[i][1] = stod(record.substr(0, record.find(';'))); record.erase(0, record.find(';') + 1);//ввод точек заземления
	}
}

void Electrod::testing() //функция для теста на ошибки, путем проверки направляющего вектора на параллельность одной из осей
{
	if (Rod_2[2] - Rod_1[2] != 0 && Rod_1[2] * Rod_2[2] < 0)
		type = 1;
	if (Rod_2[2] - Rod_1[2] != 0 && Rod_1[2] * Rod_2[2] > 0)
		type = 2;
	if (Rod_2[2] - Rod_1[2] == 0)
		type = 3;
	if (Rod_2[0] - Rod_1[0] == 0 && Rod_2[1] - Rod_1[1] == 0 && Rod_2[2] - Rod_1[2] == 0)
		type = 4;
}

//int gauss(vector<vector<double>> M, vector<double> U, int N, vector<double> &I) //гаус для подсчета
int gauss(double **M, double* U, int N, double* I) //гаус для подсчета
{
	double max;
	int k, index;
	const double eps = 1e-15;  // точность 
	k = 0;
	while (k < N)
	{
		//Поиск строки с максимальным M[i][k] 
		max = abs(M[k][k]);
		index = k;
		for (int i = k + 1; i < N; i++)
		{
			if (abs(M[i][k]) > max)
			{
				max = abs(M[i][k]);
				index = i;
			}
		}

		//Проверка на наличие ненулевых элементов в столбце ? ? 
		if (max <= eps)
		{
			cout << "Решение получить невозможно из-за нулевого столбца ";
			cout << index << " матрицы A" << endl;
			/*system("pause");
			exit(-1);*/
		}

		//Перестановка строк 
		for (int j = 0; j < N; j++)
		{
			double temp = M[k][j];
			M[k][j] = M[index][j];
			M[index][j] = temp;
		}

		double temp = U[k];
		U[k] = U[index];
		U[index] = temp;
		//Нормализация уравнений 
		for (int i = k; i < N; i++)
		{
			double temp = M[i][k];
			if (abs(temp) > eps) // для нулевого коэффициента пропустить 
			{ 
				for (int j = 0; j < N; j++)
				{
					M[i][j] = M[i][j] / temp;
				}

				U[i] = U[i] / temp;
				if (i != k) { // уравнение не вычитать само из себя 
					for (int j = 0; j < N; j++)
					{
						M[i][j] = M[i][j] - M[k][j];
					}
					U[i] = U[i] - U[k];
				}
			}
		}
		k++;
	}
	//cout << setprecision(15) << "y; " << U[0] << "  " << U[1] << "  " << U[2] << endl;
	//обратный ход 
	for (k = N - 1; k >= 0; k--)
	{
		I[k] = U[k];
		for (int i = 0; i < k; i++)
		{
			U[i] = U[i] - M[i][k] * I[k];
		}
	}
	return 0;
}


void Electrical_Safety(double *I, double **MA, Electrod *E)
{
	double Rzu = 0;
	double Uz = 0;
	double SI = 0;
	double *fi = new double[N];
	double L = 0;
	double C[3];
	double *Upr = new double[k];
	double *Tok = new double[N];
	double *U = new double[k];
	for (int i = 0; i < N; i++)
	{
		 SI += I[i];
	}
	for (int i = 0; i < N; i++)
	{
		Tok[i] = Ivvod * I[i] / SI;
	}
	for(int j = 0; j < N; j++)
		Uz += (MA[1][j] * Tok[j]);

	Rzu = Uz / Ivvod;

	for (int j = 0; j < k; j++)
	{
		U[j] = 0;
		for (int i = 0; i < N; i++)
		{
			L = sqrt(pow(E[i].Center()[0] - ground[j][0], 2) + pow(E[i].Center()[1] - ground[j][1], 2) + pow(E[i].Center()[2], 2)) ;
			fi[i] = (I[i] * ro) / (2 * M_PI) * (1 / L);

			U[j] += fi[i];
		}
	}

	cout << "Upr = ( ";
	for (int j = 0; j < k; j++)
	{
		Upr[j] = Uz - U[j];
		cout << Upr[j] << "; ";
	}
	cout << ")" << endl;


}

void make_diag(Electrod *E, double **MA)//построение диагонали
{

	for (int i = 0; i < N; i++)
	{

		MA[i][i] = E[i].Resistance_Calculate();
	}
}



double L_ij(Electrod E1, Electrod E2) //расстояние между центрами электродов
{
	double C1[3], C2[3], res = 0;
	for (int i = 0; i < 3; i++)
	{
		C1[i] = E1.Center()[i];
		C2[i] = E2.Center()[i];
		res += pow(C2[i] - C1[i], 2);
		
	}
	
	res = sqrt(res);
	return res;
}

void make_LU(Electrod *E, double **MA) //построение верхнего/нижнего треугольника матрицы
{
	for (int i = 0; i < N; i++)
	{
		for (int j = i + 1; j < N; j++)
		{
			MA[i][j] = L_ij(E[i], E[j]);
			MA[j][i] = MA[i][j];
		}
	}
}

void output(double *I)
{
	cout << "I = ( ";
	for (int i = 0; i < N; i++)
	{
		cout << I[i] << "; ";
	}
	cout << ")" << endl;
}

int main() //головная программа
{
	setlocale(LC_ALL, "rus");
	input();
	double **MA = new double*[N];
	for (int i = 0; i < N; i++)
		MA[i] = new double[N];
	
	Electrod *E = new Electrod[N]; //массив электродов
	double Rod_1[3] = { 0 }, Rod_2[3] = { 0 }, d;

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < 3; j++)
			Rod_1[j] = A[i][j];
		for (int j = 3; j < 6; j++)
			Rod_2[j - 3] = A[i][j];
		d = A[i][6];
		E[i] = Electrod(Rod_1, Rod_2, d);//создание электродов
	}
	double *I = new double[N]; //столбец решения
	double *U = new double[N]; //столбец правой части
	for (int i = 0; i < N; i++)
		U[i] = 1.0;
	make_diag(E, MA);
	make_LU(E, MA);

	/*for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			cout << MA[i][j] << "  ";
		cout << endl;
	}

	cout << endl << endl;

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < m; j++)
			cout << A[i][j] << "  ";
		cout << endl;
	}*/
	gauss(MA, U, N, I);
	output(I);
	Electrical_Safety(I, MA, E);
	system("pause");
}