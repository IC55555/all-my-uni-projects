#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <ctime>
#include <windows.h>
#include <iomanip>
#include <conio.h>
#include <fstream>

using namespace std;

const int N = 10;
float t;
double *A = new double[N];
double *B = new double[N];
double C = 0;

void input_random(int n)
{
	srand((int)time(0));
		for (int i = 0; i < n; i++)
			A[i] = rand() % 100;
		for (int j = 0; j < n; j++)
			B[j] = rand() % 100;
}

void process_parallel_2(int n)
{
#pragma omp parallel num_threads(2)
	{
#pragma omp for reduction(+: Ñ)
				for (int k = 0; k < n; k++)
					C += A[k] * B[k];
	}
}

void process_parallel_12(int n)
{
#pragma omp parallel num_threads(12)
	{
#pragma omp for reduction(+: Ñ)
		for (int k = 0; k < n; k++)
			C += A[k] * B[k];
	}
}


void process_normal(int n)
{
		for (int k = 0; k < n; k++)
			C += A[k] * B[k];
}


void clear(int n)
{
	delete[] A;
	delete[] B;
}


int main()
{
	/*input_N();*/
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	input_random(N);

	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	double elapsedTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	//process_parallel_2(N);
	process_parallel_12(N);
	//process_normal(N);

	QueryPerformanceCounter(&t2);
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	printf_s("C = %lf \n", C);
	printf_s("Time = %.100f \n", elapsedTime);

	SetConsoleTextAttribute(handle, FOREGROUND_GREEN);
	cout << "Processing is finished... \nPress any key to exit.";
	SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	clear(N);
	_getch();
	return 0;
}