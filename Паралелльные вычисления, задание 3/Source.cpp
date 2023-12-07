#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <ctime>
#include <windows.h>
#include <iomanip>
#include <conio.h>
#include <fstream>

using namespace std;

LARGE_INTEGER frequency;
LARGE_INTEGER t1, t2, t3, t4;
double elapsedTime1, elapsedTime2;

double Result = 0;
const int N = 10000;
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

double norma(int n)
{
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);

	for (int i = 0; i < n; i++)
		Result += A[i] * A[i];
	Result = sqrt(Result);

	QueryPerformanceCounter(&t2);
	elapsedTime1 = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	return Result;
}