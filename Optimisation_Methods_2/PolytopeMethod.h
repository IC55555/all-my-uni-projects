#pragma once
#include <stdio.h>
#include <vector>
#include <iostream>

using namespace std;

class PolytopeMethod
{
	
	
	const int N = 2,
		MAXITER = 100;
	const double EPS = 1E-7;
	vector<vector<double>> D; // координаты вершин симплекса
	vector<double> xMiddle, xNew;
	double fMin, fMax;
	int xMin, xMax;
	double alpha = 1,
		gamma = 2.5,
		beta = 0.5;


	double func(vector<double> x, char key);

	int Prepare(vector<double> &x0);
	double FindMinMax();
	int FindMiddle();
	int Reflection();  // отражение
	int Expansion(double fTemp);   // растяжение
	int Contraction(double fTemp); // сжатие
	int Shrink();      // редукция

public:
	int Algorithm(vector<double> &x0);
	char key = 0;
};