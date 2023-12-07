#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

float *AL, //нижний (левый) треугольник
		*AU, //верхний (правый) треугольник
		*DI, //диагональ
		*X, *Y, *Z, //для промежуточных вычислений
		*B; //свободные коэффициенты
int *IA, //индексы
		N; //размерность

void input(string file, int n, float *mas)
{
	ifstream in;
	in.open(file + ".txt");
	for (int i = 0; i < n; i++)
		in >> mas[i];
	in.close();
}

void input_all()
{
	ifstream fn;
	fn.open("n.txt");
	fn >> N;
	fn.close();
	fn.open("ia.txt");
	IA = new int[N + 1];
	for (int i = 0; i <= N; i++)
		fn >> IA[i];
	if (IA[0])
		for (int i = 0; i <= N; i++)
			IA[i]--;

	fn.close();
	B = new float[N]; X = Y = Z = B;
	DI = new float[N];
	input("b", N, B);
	input("di", N, DI);
	int size = (N * N - N) / 2;
	AL = new float[size];
	AU = new float[size];
	input("al", size, AL);
	input("au", size, AU);
}

void output(float *x, int n)
{
	ofstream out;
	out.open("x.txt");
	for (int i = 0; i < n; i++)
		out << fixed << setprecision(7) << x[i] << endl;
	out.close();
}

void LDU(void)
{
	for (int i = 0; i < N; i++)
	{
		float sumd = 0;
		int i0 = IA[i];
		int i1 = IA[i + 1];
		int j = i - (i1 - i0);
		for (int ij = i0; ij < i1; ij++, j++)
		{
			float suml = 0;
			float sumu = 0;
			int j0 = IA[j];
			int j1 = IA[j + 1];
			int ik = i0;
			int kj = j0;
			int k = i - (i1 - i0);
			int kol_i = ij - i0;
			int kol_j = j1 - j0;
			int kol_umn = kol_i - kol_j;
			if (kol_umn > 0)
			{
				ik += kol_umn;
				k += kol_umn;
			}
			else kj -= kol_umn;

			for (; ik < ij; ik++, kj++, k++)
			{
				suml += AL[ik] * AU[kj] * DI[k];
				sumu += AU[ik] * AL[kj] * DI[k];
			}

			AL[ij] -= suml;
			AL[ij] /= DI[j];
			AU[ij] -= sumu;
			AU[ij] /= DI[j];
			sumd += AU[ij] * AL[ij] * DI[j];
		}

		DI[i] -= sumd;
	}
}

void Lyb(void)
{
	for (int i = 0; i < N; i++)
	{
		float sum = 0;
		int i0 = IA[i];
		int i1 = IA[i + 1];
		int j = i - (i1 - i0);
		for (int k = i0; k < i1; k++, j++)
			sum += AL[k] * Y[j];
		Y[i] = B[i] - sum;
	}
}

void yDz(void)
{
	for (int i = 0; i < N; i++)
		Z[i] = Y[i] / DI[i];
}

void zUx(void)
{
	for (int i = N - 1; i >= 0; i--)
	{
		int j0 = IA[i];
		int j1 = IA[i + 1];
		int j = i - 1;
		float xi = Z[i];
		for (int k = j1 - 1; k >= j0; k--, j--)
			Z[j] -= xi * AU[k];
		X[i] = xi;
	}
}

int main()
{
	input_all();
	LDU();
	Lyb();
	yDz();
	zUx();
	output(X, N);
	system("pause");
}
