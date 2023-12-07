#pragma once
#include <vector>
#include <functional>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

class cubic_spline
{
public:
	double* x, * y;
	double left_b, right_b;
	double* M;
	size_t n = 0;
	void free_mem();
	//cubic_spline();
	//~cubic_spline();
	void input();
	void build_spline_Hermit(const double* x, const double* y, size_t n, double *M);
	void gap_check();
	void build_spline(const double* x, const double* y, size_t n);
	double f(double x) const;
	double f_Hermit(double x) const;
	void Calculate();
	void output();
};