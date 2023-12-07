#pragma once
#include <stdio.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <fstream>
#include <iostream>
#include <functional>
#include <iomanip>

using namespace std;

class Pirson3Method
{
	
	const int N = 2,
		MAXITER = 1000;
	const double EPS = 1E-7,
		eps_find = 1E-8,
		delta = 1E-5;
	vector<vector<double>> directionMatrix;
	vector<double> x, xNew, mult;
	double lambda;

	double func(vector<double> &x, char key);
	double f(double lambda);
	int d_func(vector<double> &x, vector<double> &grad_f); // численно или аналит.?

	int Search_Interval_With_Minimum(double xprev, double *res_a, double *res_b);
	double parabola_method(double a, double b);
	double Fibonacci(double a, double b);
public:
	int Algorithm(vector<double> &x);
	char key = 0;
};