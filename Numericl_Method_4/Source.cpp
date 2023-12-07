#include <stdio.h>
#include "Newton.h"
#include <vector>
#include <math.h>
#include "Func.h"

int maxiter;
int m, n;

std::vector<cResult> gr;
xfloat *x;

int main()
{
	xfloat *r;
	xfloat  e1, e2, h = 0;
	bool	diff;
	FILE **f;
	
   fopen_s(f, "param.txt", "rt");

	if (f == NULL)
	{
		perror("param.txt");
		return 0;
	}
	fscanf_s(*f, "%lf %lf %d", &e1, &e2, &maxiter);
	fclose(*f);
	printf("1 to calc ChM\n");
	scanf_s("%d", &m);
	diff = (m == 1);
	if (diff)
	{
		printf("Enter h\n");
		scanf_s("%lf", &h);
	}
	printf("Enter m,n\n");
	scanf_s("%d %d", &m, &n);
	x = new xfloat[n];
	printf("Enter x0(%d):\n", n);
	for (int i = 0; i < n; i++)
		scanf_s("%lf", x + i);
	cSol solution(m, n, diff);
	solution.h = h;
	solution.SetEps(e1, e2);
	solution.SetX(x);
	int i = 0;
	cResult	re;
	memcpy(re.x, x, m * sizeof(xfloat));
	re.B = 1;
	re.nr = sqrt(solution.norm);
	gr.push_back(re);
	for (int i = 0; i < maxiter && solution.Iteration(); i++)
	{
		r = solution.GetX();
		memcpy(re.x, r, n * sizeof(xfloat));
		re.B = solution.B;
		re.nr = sqrt(solution.norm);
		printf("%d\t", i + 1);
		for (int i = 0; i < n; i++)
			printf("%lf\t", r[i]);
		printf("\n");
		gr.push_back(re);
	}
	r = solution.GetX();
	memcpy(re.x, r, m * sizeof(xfloat));
	re.B = solution.B;
	re.nr = sqrt(solution.norm);
	for (int i = 0; i < n; i++)
		printf("%lf\t", r[i]);
	printf("\n");
	printf("nr:%.3g\n", re.nr);
	gr.push_back(re);
	delete[]x;
}




#define pi 3.14159265358

double F(int i, double *x)
{
	switch (i)
	{
	case 1: return (x[0] - 2) * (x[0] - 2) + (x[1] - 2) * (x[1] - 2) - 4;
	case 2: return (x[0] - 4.5) * (x[0] - 4.5) + (x[1] - 2) * (x[1] - 2) - 1;
	case 3: return x[0] - 3.85;
	}
	return 0;
}
double A(int i, int j, double *x)
{
	switch (i)
	{
	case 1:
		switch (j)
		{
		case 1: return 2 * (x[0] - 2);
		case 2: return 2 * (x[1] - 2);
		case 3: return 2 * x[2];
		}
		return 0;
	case 2:
		switch (j)
		{
		case 1: return 2 * (x[0] - 4.5);
		case 2: return 2 * (x[1] - 2);
		case 3: return 2 * x[2];
		}
		return 0;
	case 3:
		switch (j)
		{
		case 1: return 1;
		case 2: return 0;
		case 3: return 0;
		}
		return 0;

	}
	return 0;
}


#include "Newton.h"

#include <math.h>
#include <stdio.h>
#include <string.h>


xfloat norma2(xfloat *x, int n, int *number)
{
	xfloat S = 0;
	for (int i = 0; i < n; i++)
		S += x[number[i]] * x[number[i]];
	return S;
}

cSol::cSol(int _m, int _n, bool Diff)
{
	m = _m;
	n = _n;
	for (int i = 0; i < n; i++)
		x[i] = 0;
	for (int i = 0; i < n; i++)
		f[i] = 0;
	diff = Diff;
	h = 1E-3;

}



xfloat *cSol::GetX()
{
	return x;
}

void cSol::SetX(xfloat *_x)
{
	memcpy(x, _x, n * sizeof(xfloat));
	for (int i = n; i < 10; i++)
		x[i] = 0;

	Calc1();
	norm0 = norm = norma2(f, m, number);
	B = 1;
}

int cSol::Iteration()
{
	xfloat nr2;
	Calc1();
	nr2 = norma2(f, m, number);
	Gauss();
	for (int i = 0; i < m; i++)
		x[i] += B * f[number[i]];
	if (nr2 > norm)
		B /= 2;
	if (nr2 < Ef * norm0)
	{
		printf("|f|/|f0|<%.2g\n", Ef);
		return 0;
	}
	norm = nr2;
	if (B < Eb)
	{
		printf("B<%.2g\n", Eb);
		return 0;
	}
	return 1;
}
void cSol::Calc1()
{
	int i, j, t;
	xfloat s;
	xfloat tx[10];
	if (!diff)
		for (i = 0; i < m; i++)
		{
			f[i] = -F(i + 1, x);
			for (j = 0; j < n; j++)
			{
				A[i][j] = A(i + 1, j + 1, x);
			}
		}
	if (diff)
	{
		for (i = 0; i < m; i++)
		{
			f[i] = -F(i + 1, x);
			for (j = 0; j < n; j++)
			{
				s = x[j];
				x[j] += h;
				A[i][j] = F(i + 1, x);
				x[j] -= h;
				A[i][j] -= F(i + 1, x);
				x[j] = s;
				A[i][j] /= 2 * h;
			}
		}

	}
	int r;
	for (i = 0; i < n; i++)
		number[i] = i;
	if (m < n)
	{
		for (i = 0; i < n; i++)
			lmax[i] = fabs(A[0][i]);
		for (i = 1; i < m; i++)
			for (j = 0; j < n; j++)
				if (fabs(A[i][j]) > lmax[j])
					lmax[j] = fabs(A[i][j]);
		for (i = 0; i < n; i++)
			for (j = 0; j < i; j++)
			{
				s = A[i][j];
				A[i][j] = A[j][i];
				A[j][i] = s;
			}
		for (i = 0; i < n - 1; i++)
			for (j = i + 1; j < n; j++)
			{
				if (lmax[i] < lmax[j])
				{
					s = lmax[i];
					lmax[i] = lmax[j];
					lmax[j] = s;
					memcpy(tx, A[i], m * sizeof(xfloat));
					memcpy(A[i], A[j], m * sizeof(xfloat));
					memcpy(A[j], tx, m * sizeof(xfloat));

					r = number[i];
					number[i] = number[j];
					number[j] = r;

					s = f[i];
					f[i] = f[j];
					f[j] = s;
				}
			}

		for (i = 0; i < n; i++)
			for (j = 0; j < i; j++)
			{
				s = A[i][j];
				A[i][j] = A[j][i];
				A[j][i] = s;
			}

	}
}
void cSol::SetEps(xfloat _ef, xfloat _eb)
{
	Ef = _ef * _ef;
	Eb = _eb;
}
//==========================================


//#include "cSalt.h"

//int main()
//{
//	cSold solution;
//	solution.calc();
//	return 0;
//}
//#include <stdio.h>
//#include <math.h>
//#include <conio.h>

class cSold
{
protected:
	int maxiter, N, m;
	double nev, normaf, eps, min, *x, *x1, *delta_x, *ff, beta;
	double **A;

	int Gauss();
	void jacobi();
	void iskl();
	void vvod();
	double norma_f(double *x);
	void summa(double *x1, double *x, double *y, double a);
public:
	void calc();
};
//#include "cSalt.h"

double f(int i, double *x)
{
	switch (i)
	{
	case 0: return x[0] + x[1] - 3;
	case 1: return x[1] - x[0] / 2.0 - 1;
	case 2: return x[1] - x[0] + 1;
	}
	return 0;
}

double funk(int i, int j, double *x)
{
	switch (i)
	{
	case 0:
		switch (j)
		{
		case 0: return 1;
		case 1: return 1;
		case 2: return 0;
		}
		return 0;
	case 1:
		switch (j)
		{
		case 0: return -0.5;
		case 1: return 1;
		case 2: return 0;
		}
		return 0;
	case 2:
		switch (j)
		{
		case 0: return -1;
		case 1: return 1;
		case 2: return 0;
		}
		return 0;

	}
	return 0;
}

int cSold::Gauss()
{
	double Max;
	int Mi;
	for (int nm = 0; nm < m - 1; nm++) 
	{
		Max = A[nm][nm];
		Mi = nm;
		for (int i = nm; i < m; i++)
			if (abs(A[i][nm]) > abs(Max)) 
			{
				Max = A[i][nm];
				Mi = i;
			}
		double buf;
		for (int j = nm; j < N; j++) 
		{ //перставляем строку
			buf = A[nm][j];
			A[nm][j] = A[Mi][j];
			A[Mi][j] = buf;
		}
		buf = f[nm];
		f[nm] = f[Mi];
		f[Mi] = buf;
		for (int k = nm + 1; k < N; k++) //делим текущую строку
			A[nm][k] /= A[nm][nm];
		f[nm] /= A[nm][nm];
		A[nm][nm] = 1;
		double koef;
		for (int i = nm + 1; i < N; i++) 
		{ //отнимаем
			koef = A[i][nm];
			for (int j = nm; j < n; j++)
				A[i][j] -= A[nm][j] * koef;
			f[i] -= f[nm] * koef;
		}
	}
	f[m] /= A[m][m];
	A[m][m] = 1;
	double *X = new double[n];
	double sum;
	for (int i = m; i >= 0; i--) { //обратный обход
		sum = 0;
		for (int j = i + 1; j < n; j++) {
			sum += X[j] * A[i][j];
		}
		X[i] = f[i] - sum;
	}
	memcpy(f, X, sizeof(double) * m);
	return 1;
}

void cSold::jacobi()
{
	A = new double *[m];
	for (int i = 0; i < m; i++)
		A[i] = new double[N];
	for (int i = 0; i < m; i++) 
	{
		for (int j = 0; j < N; j++)
			A[i][j] = funk(i, j, x);
		ff[i] = -F(i, x);
	}
}

void cSold::iskl()
{
	int j, minim;
	for (int i = 0; i < m - N; i++)
	{
		for (j = 1, minim = 0; j < m - i; j++)
		{
			if (abs(ff[j]) < abs(ff[minim]))
				minim = j;
		}
		for (int k = minim; k < m - i - 1; k++)
		{
			for (j = 0; j < N; j++)
				A[k][j] = A[k + 1][j];
			ff[k] = ff[k + 1];
		}
	}
}


void cSold::vvod()
{
	FILE **in;
	int k;
   fopen_s(in, "parametrs.txt", "r");
	fscanf_s(*in, "%i%i%i%lf%lf", &N, &m, &maxiter, &eps, &min);
	fclose(*in);
	x = new double[N];
	x1 = new double[N];
	ff = new double[N];
	delta_x = new double[N];
	fopen_s(in, "nach.txt", "r");
	for (k = 0; k < N; k++)
		fscanf_s(*in, "%lf", &x[k]);
	fclose(*in);
}

double cSold::norma_f(double *x)
{
	double n = 0.;
	for (int i = 0; i < m; i++)
		n += F(i, x) * F(i, x);
	n = sqrt(n);
	return n;
}


void cSold::summa(double *x1, double *x, double *y, double a)
{
	for (int i = 0; i < N; i++)
		x1[i] = x[i] + a * y[i];
}

void cSold::calc()
{
	FILE **v;
	FILE **out;
	fopen_s(v, "table.txt", "w");
	int r;
	int t = 0;
	int i = 1;
	vvod(); //ввод параметров итерационного процесса, начального приближения, размерности СНАУ
	normaf = norma_f(x);
	nev = 1;
	for (r = 0; r<maxiter && t == 0 && nev>eps && i == 1; r++) 
	{
		fprintf(*v, "%.16lf\t%.16lf\n", x[0], x[1]);
		jacobi();
		iskl();
		t = Gauss();
		if (t == 0) {
			for (beta = 1, i = 0; beta > min && i == 0;) 
			{
				summa(x1, x, delta_x, beta);
				if (norma_f(x1) > norma_f(x))
					beta = beta / 2;
				else
					i = 1;
			}
			if (i == 1) {
				nev = norma_f(x1) / normaf;
				summa(x, x1, x1, 0);
			}
		}
		else
			printf("SLAU hasnt result!!!");
	}
	fopen_s(out, "result.txt", "w");
	fprintf(*out, "nev = %.1e\n", nev);
	fprintf(*out, "beta = %f\n", beta);
	fprintf(*out, "kolvo iter = %i", r);
}
