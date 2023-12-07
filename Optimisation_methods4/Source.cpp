#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <clocale>
#include <Windows.h>

using namespace std;

fstream fout;
double eps = 1E-1;
double Peps = eps * eps / 400;
double P = 0.7;
int N = log(1 - P) / log(1 - Peps);
const int maxiter = 10000;
int funcComp = 0;
UCHAR alg_key;

struct Point
{
	double x;
	double y;
};

Point operator - (Point &v1, Point &v2)
{
	Point res;
	res.x = v1.x - v2.x;
	res.y = v1.y - v2.y;
	return res;
}

Point operator + (Point &v1, Point &v2)
{
	Point res;
	res.x = v1.x + v2.x;
	res.y = v1.y + v2.y;
	return res;
}
Point operator * (double k, Point &v2)
{
	Point res;
	res.x = k * v2.x;
	res.y = k * v2.y;
	return res;
}

double GetFunc(Point &CurPoint)
{
	vector <double> C = { 2, 3, 8, 3, 2, 8 };
	vector <double> a = { 3, -5, 0, 3, -4, 6 };
	vector <double> b = { -4, -6, -1, 7, 0, 5 };
	double func = 0;
	for (int i = 0; i < 6; i++)
	{
		func += C[i] / (1 + (CurPoint.x - a[i]) * (CurPoint.x - a[i]) + (CurPoint.y - b[i]) * (CurPoint.y - b[i]));
	}
	return func;
}

void MakeRandomPoint(Point &cur, double a1, double b1, double a2, double b2)
{
	cur.x = a1 + rand() * (b1 - a1) / RAND_MAX;
	cur.y = a2 + rand() * (b2 - a2) / RAND_MAX;
}

void SimpleRandomSearch()
{
	fout.open("out.txt");
	fout.precision(9);
	fout.imbue(locale("Russian"));
	Point curPoint, maxPoint;
	double curFunc, maxFunc;

	MakeRandomPoint(maxPoint, -10, 10, -10, 10);
	maxFunc = GetFunc(maxPoint);
	cout << N << '\t';
	for (int i = 0; i < N; i++)
	{
		MakeRandomPoint(curPoint, -10, 10, -10, 10);
		curFunc = GetFunc(curPoint);
		if (curFunc > maxFunc)
		{
			maxFunc = curFunc;
			maxPoint = curPoint;
		}
	}
	fout << N << '\t' << maxPoint.x << '\t' << maxPoint.y << '\t' << maxFunc;
	fout << "N = " << N << endl;
	fout << "x = " << maxPoint.x << endl;
	fout << "y = " << maxPoint.y << endl;
	fout << "f = " << maxFunc << endl;
}

double HyperSquareSearch(Point &startPoint)
{
	double a1 = startPoint.x - 5, b1 = startPoint.x + 5, a2 = startPoint.y - 5, b2 = startPoint.y + 5;
	if (a1 < -10) a1 = -10;
	if (b1 > 10) b1 = 10;
	if (a2 < -10) a2 = -10;
	if (b2 > 10) b2 = 10;

	double alpha = 2;
	int m = 100;//число точек
	vector<Point> points(m);
	double maxFunc, tempFunc, predmaxFunc = 0;
	Point maxPoint = startPoint;
	N = 0;

	maxFunc = GetFunc(maxPoint);
	do 
	{
		N++;
		//выброс 100 случайных точек
		for (int i = 0; i < m; i++)
			MakeRandomPoint(points[i], a1, b1, a2, b2);

		//выбор наилучшей точки - центра гиперквадрата
		for (int i = 0; i < m; i++)
		{
			tempFunc = GetFunc(points[i]);
			if (maxFunc < tempFunc)
			{
				predmaxFunc = maxFunc;
				maxFunc = tempFunc;
				maxPoint = points[i];
			}
		}

		//изменение гиперквадрата
		double a1new, b1new, a2new, b2new;
		double tmp;
		tmp = maxPoint.x - (b1 - a1) / (2 * alpha);
		if (tmp > -10)
			a1new = tmp;
		else
			a1new = -10;
		tmp = maxPoint.x + (b1 - a1) / (2 * alpha);
		if (tmp < 10)
			b1new = tmp;
		else
			b1new = 10;
		tmp = maxPoint.y - (b2 - a2) / (2 * alpha);
		if (tmp > -10)
			a2new = tmp;
		else
			a2new = -10;
		tmp = maxPoint.y + (b2 - a2) / (2 * alpha);
		if (tmp < 10)
			b2new = tmp;
		else
			b2new = 10;

		a1 = a1new;
		b1 = b1new;
		a2 = a2new;
		b2 = b2new;

	} while (abs(predmaxFunc - maxFunc) > eps && N < maxiter);

	startPoint = maxPoint;
	return maxFunc;
}

void Algorithm1(int mCount)
{
	fout.open("out.txt");
	fout.precision(9);
	fout.imbue(locale("Russian"));
	Point startPoint, tempPoint;
	funcComp = 0;
	MakeRandomPoint(startPoint, -10, 10, -10, 10);
	double maxFunc = HyperSquareSearch(startPoint);
	int m = 0;
	do
	{
		MakeRandomPoint(tempPoint, -10, 10, -10, 10);
		double tempFunc = HyperSquareSearch(tempPoint);
		funcComp += N;
		m++;
		if (maxFunc < tempFunc)
		{
			m = 0;
			maxFunc = tempFunc;
			startPoint = tempPoint;
		}
	} while (m != mCount);

	fout << funcComp << '\t' << startPoint.x << '\t' << startPoint.y << '\t' << maxFunc;
	cout << "fComp= " << funcComp << endl;
	cout << "x    = " << startPoint.x << endl;
	cout << "y    = " << startPoint.y << endl;
	cout << "func = " << maxFunc << endl;
}

void Algorithm2(int mCount)
{
	fout.open("out.txt", ios_base::app);
	fout.precision(9);
	fout.imbue(locale("Russian"));
	int m;
	Point localPoint, curPoint;
	double curFunc, localFunc;
	funcComp = 0;
	MakeRandomPoint(curPoint, -10, 10, -10, 10);
	do {
		//локальный максимум из точки
		localPoint = curPoint;
		localFunc = HyperSquareSearch(localPoint);
		funcComp += N;
		m = 0;
		//ненаправленный поиск
		do 
		{
			MakeRandomPoint(curPoint, -10, 10, -10, 10);
			curFunc = GetFunc(curPoint);
			m++;
			funcComp++;
		} while (curFunc <= localFunc && m < mCount);
	} while (m < mCount);

	fout << funcComp << '\t' << localPoint.x << '\t' << localPoint.y << '\t' << localFunc;
	cout << "fComp= " << funcComp << endl;
	cout << "x   = " << localPoint.x << endl;
	cout << "y   = " << localPoint.y << endl;
	cout << "func= " << localFunc << endl;
	fout << endl;
}

void Algorithm3(int oCount)
{
	fout.open("out.txt", ios_base::app);
	fout.precision(9);
	fout.imbue(locale("Russian"));
	double lyam = 2;
	int m, o;
	Point localPoint, localPoint1, curPoint, distance, tempPoint, tempPoint1;
	double curFunc, localFunc, localFunc1, tempFunc, tempFunc1;
	funcComp = 0;
	MakeRandomPoint(curPoint, -10, 10, -10, 10);
	bool flag = true;

	//локальный максимум из точки
	localPoint = curPoint;
	localFunc = HyperSquareSearch(localPoint);

	//цикл по областям притяжения
	while (flag) 
	{
		distance = localPoint - curPoint; //x*1-x01
		double df = 0;
		//проверим направление x*1-x01
		tempPoint = localPoint;
		tempFunc = GetFunc(tempPoint);
		funcComp++;
		do 
		{
			Point tmp = lyam * distance;
			tempPoint1 = tempPoint + tmp;
			tempFunc1 = GetFunc(tempPoint1);
			funcComp++;
			df = tempFunc1 - tempFunc;
			tempPoint = tempPoint1;
			tempFunc = tempFunc1;
		} while (df <= 0 && tempPoint.x<10 && tempPoint.x>-10 && tempPoint.y<10 && tempPoint.y>-10);

		if (df > 0)
		{
			localPoint1 = tempPoint;
			localFunc1 = HyperSquareSearch(localPoint1);
			funcComp += N;
		}
		//если нашли в данном направлении
		if (df > 0 && localFunc1 - localFunc > eps)
		{
			localPoint = localPoint1;
			localFunc = localFunc1;
		}
		else
		{
			o = 0;
			//выбросить несколько случайных направлений и пройти по ним
			do 
			{
				//сгенерировать случайное направление
				MakeRandomPoint(distance, -10, 10, -10, 10);
				tempPoint = localPoint;
				o++;
				//пройти по этому направлению
				do 
				{
					Point tmp = lyam * distance;
					tempPoint1 = tempPoint + tmp;
					tempFunc1 = GetFunc(tempPoint1);
					funcComp++;
					df = tempFunc1 - tempFunc;
					tempPoint = tempPoint1;
					tempFunc = tempFunc1;
				} while (df <= 0 && tempPoint.x<10 && tempPoint.x>-10 && tempPoint.y<10 && tempPoint.y>-10);
				if (df > 0)
				{
					localPoint1 = tempPoint;
					localFunc1 = HyperSquareSearch(localPoint1);
					funcComp += N;
				}
			} while ((df <= 0 || localFunc1 - localFunc < eps) && o < oCount);

			if (df > 0 && localFunc1 - localFunc > eps)
			{
				localPoint = localPoint1;
				localFunc = localFunc1;
			}
			else
				flag = false;
		}
	}

	fout << funcComp << '\t' << localPoint.x << '\t' << localPoint.y << '\t' << localFunc;

	cout << endl << endl << endl;
	cout << "fComp= " << funcComp << endl;
	cout << "x    = " << localPoint.x << endl;
	cout << "y    = " << localPoint.y << endl;
	cout << "func = " << localFunc << endl;

	fout << endl;
}

int main()
{
	setlocale(LC_ALL, "");
	cout << "Выберите алгоритм: " << endl
		<< "Первый алгоритм(код ввода - аргумент): 11-1(1), 12-1(5), 13-1(15), 14-1(40), 15-1(100), 16-1(150)" << endl
		<< "Второй алгоритм(код ввода - аргумент): 21-2(1), 22-2(5), 23-2(15), 24-2(40), 25-1(100), 26-2(150)" << endl
		<< "Третий алгоритм(код ввода - аргумент): 31-3(1), 32-3(5), 33-3(15), 34-3(40), 35-3(100), 36-3(150)" << endl;
	cin >> alg_key;

	cout << scientific << setprecision(6);
	srand(10);
	SimpleRandomSearch();

	switch (alg_key)
	{
		case 11:
			Algorithm1(1);
			break;
		case 12:
			Algorithm1(5);
			break;
		case 13:
			Algorithm1(15);
			break;
		case 14:
			Algorithm1(40);
			break;
		case 15:
			Algorithm1(100);
			break;
		case 16:
			Algorithm1(150);
			break;

		case 21:
			Algorithm2(1);
			break;
		case 22:
			Algorithm2(5);
			break;
		case 23:
			Algorithm2(15);
			break;
		case 24:
			Algorithm2(40);
			break;
		case 25:
			Algorithm2(100);
			break;
		case 26:
			Algorithm2(150);
			break;

		case 31:
			Algorithm3(1);
			break;
		case 32:
			Algorithm3(5);
			break;
		case 33:
			Algorithm3(15);
			break;
		case 34:
			Algorithm3(40);
			break;
		case 35:
			Algorithm3(100);
			break;
		case 36:
			Algorithm3(150);
			break;

		default:
			return -1;
	}
	system("pause");
	return 0;
}
