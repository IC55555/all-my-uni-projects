#ifndef _newton_h_
#define _newton_h_

#include "Func.h"
typedef double	xfloat;

class cSol
{
protected:
	int	m, n;
	xfloat	a[10][10];
	xfloat	f[10];
	xfloat	x[10];
	void Calc1();
	void Calc();
	int	number[10];
	xfloat lmax[10];
	xfloat Ef, Eb;
	xfloat norm0;
	int	fact();
	bool diff;

public:
	xfloat	B;
	xfloat	norm;
	xfloat	h;
	cSol(int _m, int _n, bool Diff = false);
	void SetX(xfloat *_x);
	xfloat *GetX();
	void	SetEps(xfloat _ef, xfloat _eb);
	int		Iteration();
	~cSol();
};

class cResult
{
public:
	cResult()
	{
		x[0] = x[1] = x[2] = 0;
	}
	xfloat x[10];
	xfloat nr;
	xfloat B;
};

#endif //_newton_h_