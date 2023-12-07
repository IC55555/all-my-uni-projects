#include "PolytopeMethod.h"
#include <conio.h>

double PolytopeMethod::func(vector<double> x, char key)
{
	double A1 = 2, A2 = 3, a1 = 1, b1 = 2, c1 = 1, d1 = 2, a2 = 1, b2 = 3, c2 = 3, d2 = 3;
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
			return -(A1 * exp( - ((x[0] - a1) / b1) * ((x[0] - a1) / b1) - ((x[1] - c1) / d1) * ((x[1] - c1) / d1)) + A2 * exp(-((x[0] - a2) / b2) * ((x[0] - a2) / b2) - ((x[1] - c2) / d2) * ((x[1] - c2) / d2)));
			break;
		case '4':
			// простейшая функция; ответ (7; 1)
			return (x[0] - 7) * (x[0] - 7) + (x[1] - 1) * (x[1] - 1);
			break;
		default:
			break;
	}

}

int PolytopeMethod::Prepare(vector<double> &x0)
{
	D.resize(N + 1);
	xMiddle.resize(N);
	xNew.resize(N);
	double t = 13; // расстояние между вершинами
	double d1 = (sqrt(N + 1) + N - 1) * t / (N * sqrt(2)),
		d2 = t / ((sqrt(N + 1) - 1) * N * sqrt(2));
	D[0].resize(N);
	D[0] = x0;
	for (int i = 1; i < N + 1; i++)
	{
		D[i].resize(N);
		for (int j = 0; j < N; j++)
		{
			D[i][j] = D[0][j] + d2;
		}
		D[i][i - 1] = D[0][i - 1] + d1;
	}
	return 0;

}

int PolytopeMethod::FindMiddle()
{
	for (int j = 0; j < N; j++)
	{
		double sum = 0;
		for (int i = 0; i < N + 1; i++)
		{
			sum += D[i][j];
		}
		sum -= D[xMax][j];
		xMiddle[j] = sum / N;
	}
	return 0;
}

double PolytopeMethod::FindMinMax()
{
	double ftemp = func(D[0], key),
		fmiddle = func(xMiddle, key);
	fMax = ftemp;
	fMin = ftemp;
	xMax = 0;
	xMin = 0;
	double sum = 0;
	for (int i = 1; i < N + 1; i++)
	{
		ftemp = func(D[i], key);
		sum += (ftemp - fmiddle) * (ftemp - fmiddle);
		if (ftemp > fMax)
		{
			fMax = ftemp;
			xMax = i;
		}
		else if (ftemp < fMin)
		{
			fMin = ftemp;
			xMin = i;
		}
	}
	return sum;
}


int PolytopeMethod::Reflection() // отражение
{
	for (int i = 0; i < N; i++)
	{
		xNew[i] = xMiddle[i] + alpha * (xMiddle[i] - D[xMax][i]);
	}
	return 0;
}

int PolytopeMethod::Expansion(double fTemp) // растяжение
{
	vector<double> xTemp(N);
	for (int i = 0; i < N; i++)
	{
		xTemp[i] = xMiddle[i] + gamma * (xNew[i] - xMiddle[i]);
	}
	if (func(xTemp, key) < fTemp)
	{
		D[xMax] = xTemp;
	}
	else
	{
		D[xMax] = xNew;
	}
	return 0;

}

int PolytopeMethod::Contraction(double fTemp) // сжатие
{
	vector<double> xTemp(N);
	for (int i = 0; i < N; i++)
	{
		xTemp[i] = xMiddle[i] + beta * (D[xMax][i] - xMiddle[i]);
	}
	double fTemp2 = func(xTemp, key);
	if (fTemp2 < fTemp)
		D[xMax] = xTemp;
	else
		D[xMax] = xNew;
	return 0;

}

int PolytopeMethod::Shrink() // редукция
{
	for (int i = 0; i < N + 1; i++)
	{
		if (i != xMin)
		{
			for (int j = 0; j < N; j++)
			{
				D[i][j] = D[xMin][j] + 0.5 * (D[i][j] - D[xMin][j]);
			}
		}
	}
	return 0;

}

int PolytopeMethod::Algorithm(vector<double> &x0)
{
	Prepare(x0);
	int iter;
	double stop = 10;
	FindMinMax();

	for (iter = 1; iter < MAXITER && stop > EPS; iter++)
	{
		FindMiddle();

		Reflection();
		double fTemp = func(xNew, key);
		if (fTemp > fMax)
		{
			Shrink();
		}
		else if (fTemp < fMax)
		{
			if (fTemp < fMin)
			{
				Expansion(fTemp);
			}
			else
			{
				Contraction(fTemp);
			}
		}

		stop = FindMinMax();
		stop = sqrt(stop);
		stop /= (N + 1);

	}

	x0 = D[xMin];
	cout << "minimal f(x) = " << func(x0, key) << endl;
	return iter;
}