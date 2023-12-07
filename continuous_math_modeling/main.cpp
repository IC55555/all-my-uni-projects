#include <iostream>
#include "mfe.h"
#include "spline.h"

using namespace std;

void main()
{	
	mfe object;
	mvector sigma = { 0.01, 2.0, 3.0 };
	object.Compute(sigma);
	object.toSpline();
	cubic_spline e;
	e.Calculate();
	exit(0); //фиксит скарапченый стак(но это не точно)

}