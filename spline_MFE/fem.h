#ifndef FEM_H_INCLUDED
#define FEM_H_INCLUDED

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#else
#include <cmath>
#endif

#include <cstring>
#include <cfloat>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <set>
#include <omp.h>

#include "cgm.h"
#include "telma.h"
#include "bmp24_lib.h"
using namespace std;

// Used to express doubt or disapproval
class grid_generator
{
public:
    telma_sreda tsr;
    set <double> grid_x, grid_y;
    void grid_gen(string filename);
};

// ��������� ������� ���������
static const double Gx[4][4] =
{
    {2.0, -2.0, 1.0, -1.0},
    {-2.0, 2.0, -1.0, 1.0},
    {1.0, -1.0, 2.0, -2.0},
    {-1.0, 1.0, -2.0, 2.0}
};

static const double Gy[4][4] =
{
    {2.0, 1.0, -2.0, -1.0},
    {1.0, 2.0, -1.0, -2.0},
    {-2.0, -1.0, 2.0, 1.0},
    {-1.0, -2.0, 1.0, 2.0}
};

// ��������� ������� �����
static const double M[4][4] =
{
    {4.0, 2.0, 2.0, 1.0},
    {2.0, 4.0, 1.0, 2.0},
    {2.0, 1.0, 4.0, 2.0},
    {1.0, 2.0, 2.0, 4.0}
};

// ��������� ������� ����� ��� ������ �������
static const double Mb[2][2] =
{
    {2.0, 1.0},
    {1.0, 2.0}
};

// �������� �������
class finite_element
{
public:
    unsigned int node_n[4]; // 4 ������ ����� � ������ ��
    double lambda;          // ������ � ��
    double gamma;           // ����� � ��
    double f[4];            // �������� ������ ����� � �����
};

// ����
class node
{
public:
    double x, y;    // ���������� ����
    node() {}
    node(double x, double y) { this->x = x; this->y = y; }
};

// ����
class SLAE
{
public:
    double* gg, * di, * f, * q;
    unsigned int* ig, * jg;
    unsigned int n;
    SLAE();     // �����������
    ~SLAE();    // ����������
    void alloc_all(unsigned int gg_size);   // ��������� ������
    void add(unsigned int i, unsigned int j, double elem);  // ���������� � ������� �������� elem � ������� i,j
    void solve();   // ������� ������� ����
    void clean();   // ��������� gg, di � f
private:
    class CGM cgm;  // �������� ���
};

// ���
class FEM
{
public:
    class SLAE slae;    // ����
    virtual void calc();        // �������� �������, ���������� ��� ���������
    double get_solution(double x, double y);    // ��������� ������� � ������������ �����
    double get_b(double x, double y);           // ��������� �������� B � ������������ �����
    void draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid);  // ��������� ��������
    void draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid, double x0, double y0, double x1, double y1);  // ��������� ��������
    class grid_generator grid;  // ��������� �����
protected:
    class node* nodes; // ����
    class finite_element* finite_elements; // �������� ��������
    unsigned int fe_num;    // ����� ��
    unsigned int node_num;  // ����� �����
    void read();        // ������ ������ �� ����� � ��������� ��������
    virtual void assembling_global();   // ������ ���������� �������
    void applying_bound();      // ���������� ������� �������
};

#endif // FEM_H_INCLUDED
