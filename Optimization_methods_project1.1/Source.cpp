#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <iomanip>
#include <conio.h>

using namespace std;
typedef double real;
const real c1 = 1.6180339888;
const real c2 = -0.618033988;
const real c3 = 2.2360679775;
fstream fo("out.txt");
const real a_start = -2;
const real b_start = 20;

real eps, delta = 1e-9;

function<real(real)> f = { [](real x) {return (x - 7) * (x - 7); } };

int Dichotomy(real &a, real &b, int &countf)
{

	fo << setprecision(15);
	a = a_start;
	b = b_start;
	int iter = 0;
	while (abs(b - a) > eps)
	{
		real x1 = (a + b - delta) / 2;
		real x2 = (a + b + delta) / 2;

		real fx1 = f(x1);
		countf += 2;
		real fx2 = f(x2);

		if (fx1 < fx2)//"корень пренадлежет [a,x1]"
			b = x2;
		else a = x1;
		iter++;
		fo << "\n" << iter << "\t" << a << "\t" << b << "\t" << x1 << "\t" << x2;
	}
	fo << "\n" << "\n";
	return iter;
}

int GoldenRatio(real &a, real &b, int &countf)
{

	fo << setprecision(15);
	a = a_start;
	b = b_start;
	real x1 = a + 0.381966011 * (b - a);
	real x2 = b - 0.381966011 * (b - a);
	real fx1 = f(x1);
	real fx2 = f(x2);
	countf += 2;
	int iter = 0;
	while (abs(b - a) > eps)
	{
		if (fx1 < fx2)//"корень пренадлежит [a,x2]"
		{
			b = x2;
			x2 = x1;
			x1 = a + 0.381966011 * (b - a);
			//x1 = a + b - x2;
			fx2 = fx1;
			fx1 = f(x1);
			countf++;
		}
		else //"корень пренадлежит [x1,b]"
		{
			a = x1;
			x1 = x2;
			x2 = b - 0.381966011 * (b - a);
			//x2 = a + b - x1;
			fx1 = fx2;
			fx2 = f(x2);
			countf++;
		}
		iter++;
		fo << "\n" << iter << "\t" << a << "\t" << b << "\t" << x1 << "\t" << x2 << "\t";

	}
	fo << "\n" << "\n";
	return iter;
}

real SearchIntervalWithMinimum(real &xpred, real &xnext, int &countf)
{
	ofstream fo("out.txt");
	cout << setprecision(15);
	real fxpred = f(xpred);
	real h, fxk;
	bool flag = true;
	real xt = xpred;
	if (fxpred > f(xpred + delta))
	{
		xnext = xpred + delta; h = delta;
		fxk = f(xpred + delta);
		countf++;
		flag = true;

	}
	else if (fxpred > f(xpred - delta))
	{
		xnext = xpred - delta; h = -delta;
		fxk = f(xpred - delta);
		countf++;
		flag = false;
	}
	else
	{
		xt = xpred;
		xpred = xpred - delta;
		xnext = xt + delta;
		cout << "\n" << 0 << "\t" << xpred << "\t" << xt << "\t" << xnext;
		return 0;
	}

	int iter = 1;

	do
	{
		xpred = xt;
		h = 2 * h;
		xt = xnext;
		xnext = xpred + h;
		fxpred = fxk;
		fxk = f(xnext);
		countf++;
		iter++;

		if (!flag)
			cout << "\n" << iter << "\t" << xnext << "\t" << xpred;
		else
			cout << "\n" << iter << "\t" << xpred << "\t" << xnext;
	} while (fxpred > fxk);
	return iter;
}

int Fibonachi1(real &a, real &b, int &countf)
{

	fo << setprecision(15);
	a = a_start;
	b = b_start;
	vector<real> F;
	F.push_back(1);
	F.push_back(1);
	F.push_back(2);//n=2
	int n = 1;
	while ((b - a) / eps >= F[n + 1])
	{
		F.push_back(F[n] + F[n + 1]);
		n++;
	}

	real x1 = a + (F[n - 1] / F[n + 1]) * (b - a);
	real x2 = a + (F[n] / F[n + 1]) * (b - a);
	real fx1 = f(x1);
	real fx2 = f(x2);
	countf += 2;

	int iter = 1;
	for (int k = 2; k <= n; k++)
	{
		fo << "\n" << iter << "\t" << a << "\t" << b << "\t" << x1 << "\t" << x2;
		if (fx1 < fx2)//"корень пренадлежит [a,x2]"
		{
			b = x2;
			x2 = x1;
			x1 = a + F[n - k] / F[n - k + 2] * (b - a);
			//x1 = a + b - x2;
			fx2 = fx1;
			fx1 = f(x1);
			countf++;
		}
		else
		{
			a = x1;
			x1 = x2;
			x2 = a + F[n - k + 1] / F[n - k + 2] * (b - a);
			//x2 = a + b - x1;
			fx1 = fx2;
			fx2 = f(x2);
			countf++;
		}
		iter++;
	}
	fo << "\n" << iter << "\t" << a << "\t" << b << "\t" << x1 << "\t" << x2 << "\t" << abs(x2 - 2) << countf;
	fo << "\n" << "\n";
	return iter;
}

int main()
{
	real a, b, xpred, xk;
	int countf = 0;
	eps = 1e-1;
	cout << setprecision(15);
	cout << "press 1 for Dichotomy, 2 for GoldRatio, 3 for Fibonachi, 4 for Interval Search" << endl;
	char key;
	key = _getch();

	switch (key)
	{
		case '1':
			for (int i = 1; i <= 7; i++)
			{
				int iter = Dichotomy(a, b, countf);
				cout << "\n" << "Dichotomy\t" << countf << " " << iter;
				eps *= 0.1;;
			}
			cout << endl;
			break;
		case '2':
			for (int i = 1; i <= 7; i++)
			{
				countf = 0;
				int iter = GoldenRatio(a, b, countf);
				cout << "\n" << "GoldenRatio\t" << "Number of calculation f(x): " << countf << "  Number of iteration:  " << iter << "  Left border: " << a << "  Right border: " << b;
				eps *= 0.1;;
			}
			cout << endl;
			break;
		case '3':
			cout << "\n";
			eps = 1e-1;
			for (int i = 1; i <= 7; i++)
			{
				countf = 0;
				int iter = Fibonachi1(a, b, countf);
				cout << "\n" << "Fibona4i\t" << "Number of calculation f(x): " << countf << "  Number of iteration:  " << iter << "  Left border: " << a << "  Right border:  " << b;
				eps *= 0.1;;
			}
			cout << endl;
			break;
		case '4':
			cout << " Enter x0 and delta \n";
			cin >> xpred >> delta;
			while (xpred != 150)
			{
				countf = 0;
				xk = 0;
				real x = SearchIntervalWithMinimum(xpred, xk, countf);
				cout << " \nEnter x0 and delta";
				cin >> xpred >> delta;
			}
			cout << endl;
			break;
		default:
			cout << "Miss click" << endl;
			system("pause");
			return 0;
	}

	system("pause");
	return 0;
}