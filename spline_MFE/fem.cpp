#include "fem.h"

// �������� �������, ���������� ��� ���������
void FEM::calc()
{
    read();         // ������ ������ � ��������� �������
    assembling_global();    // �������� ���������� �������
    applying_bound();       // ��������� ������� �������
    slae.solve();   // ������ ����

    // ������� ��������� � ����
    ofstream solution_s;
    solution_s.open("solution.txt", ios::out);
    for (unsigned int i = 0; i < slae.n; i++)
    {
        solution_s << nodes[i].x << " \t"
            << nodes[i].y << " \t"
            << slae.q[i] << endl;
    }
    solution_s.flush();
    solution_s.close();
}

// ������ ������ �� ����� � ��������� ��������
void FEM::read()
{
    cout << "Reading data..." << endl;
    ifstream node_s, fe_s;
    node_s.open("nodes.txt", ios::in);
    fe_s.open("finite_el.txt", ios::in);

    // �������� ����� �����
    node_s >> node_num;
    cout << " > Detected " << node_num << " nodes." << endl;
    nodes = new class node[node_num];

    // ���������� ��������� �����
    for (unsigned int i = 0; i < node_num; i++)
    {
        node_s >> nodes[i].x >> nodes[i].y;
    }
    node_s.close();

    // ���������� ����� ��
    fe_s >> fe_num;
    cout << " > Detected " << fe_num << " finite elements." << endl;
    finite_elements = new class finite_element[fe_num];
    // ������ ���������� �������� ���������
    for (unsigned int i = 0; i < fe_num; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            fe_s >> finite_elements[i].node_n[j];
        }
        fe_s >> finite_elements[i].lambda >> finite_elements[i].gamma;
        for (unsigned int j = 0; j < 4; j++)
        {
            fe_s >> finite_elements[i].f[j];
        }
    }
    fe_s.close();

    // ������������ ������� (��������)
    cout << "Generating profile..." << endl;
    unsigned int gg_size = 0;
    slae.n = node_num;
    // ������� ������ ������� ��� �������� ������
    set<unsigned int>* profile = new set<unsigned int>[node_num];

    // ����� ����, ���� ���� ����������� ������ ��
    // ������� ������� �������� �������� � ��������� � ������ ����� �������
    for (unsigned int i = 0; i < fe_num; i++)
        for (unsigned int j = 0; j < 4; j++)
            for (unsigned int k = 0; k < j; k++)
                profile[finite_elements[i].node_n[k]].insert(finite_elements[i].node_n[j]);

    // ������� ������������� ������ � ������ � ��������� ��, ������� ������ ������ �������
    for (unsigned int i = 0; i < node_num; i++)
    {
        gg_size += profile[i].size();
    }
    slae.alloc_all(gg_size);

    // ���������� ������� (��������)
    slae.ig[0] = 0;
    slae.ig[1] = 0;
    unsigned int tmp = 0;
    for (unsigned int i = 0; i < slae.n; i++)
    {
        unsigned int k = 0;
        for (unsigned int j = 0; j <= i; j++)
        {
            // ���� ���� ����� ����� i � j, ������ � ���� ����� ������� ����� ��������� �������
            // ������� ���������� �� ���� � jg
            if (profile[j].find(i) != profile[j].end())
            {
                slae.jg[tmp] = j;
                tmp++;
                k++;
            }
        }
        // � � ig ������� ���������� � ���������� ��������� ��������� � ������
        slae.ig[i + 1] = slae.ig[i] + k;
    }

    // ������� �������
    for (unsigned int i = 0; i < node_num; i++)
        profile[i].clear();
    delete[] profile;
}

// ������ ���������� �������
void FEM::assembling_global()
{
    // ���������� ���������� ������� (��� ����� �������)
    cout << "Generating global matrix..." << endl;
    for (unsigned int i = 0; i < fe_num; i++)
    {
        // ���������� ����
        double hx = fabs(nodes[finite_elements[i].node_n[1]].x - nodes[finite_elements[i].node_n[0]].x);
        double hy = fabs(nodes[finite_elements[i].node_n[2]].y - nodes[finite_elements[i].node_n[0]].y);

        // ���������� ��������, ��������� �� ���������� ��, �� ������� ����� ���������� ��������� �������
        double Gx_c = finite_elements[i].lambda / 6.0 * hy / hx;
        double Gy_c = finite_elements[i].lambda / 6.0 * hx / hy;
        double M1_c = hx * hy / 36.0;
        double M_c = finite_elements[i].gamma * M1_c;

        // ������ �����
        // ���������� � ��������� �������� �������� ������ ����� � ������
        double fr[4];
        for (int j = 0; j < 4; j++)
        {
            fr[j] = 0.0;
            for (int k = 0; k < 4; k++)
            {
                fr[j] += M[j][k] * finite_elements[i].f[k];
            }
        }

        // �������
        for (unsigned int j = 0; j < 4; j++)
        {
            // ������ ��� ��������������� ���������
            for (unsigned int k = 0; k < j; k++)
            {
                // ������ �������
                double a = M_c * M[j][k] + Gx_c * Gx[j][k] + Gy_c * Gy[j][k];
                // � ������� ��
                if (finite_elements[i].node_n[j] > finite_elements[i].node_n[k])
                    slae.add(finite_elements[i].node_n[j], finite_elements[i].node_n[k], a);
                else
                    slae.add(finite_elements[i].node_n[k], finite_elements[i].node_n[j], a);
            }
            // ��������� ������� � ��������� � ������� ��
            double a = M_c * M[j][j] + Gx_c * Gx[j][j] + Gy_c * Gy[j][j];
            slae.di[finite_elements[i].node_n[j]] += a;
            // ������� ������� � ������ �����
            slae.f[finite_elements[i].node_n[j]] += M1_c * fr[j];
        }
    }
}

// ���������� ������� �������
void FEM::applying_bound()
{
    ifstream bound1_s, bound2_s;
    bound1_s.open("bound1.txt", ios::in);
    bound2_s.open("bound2.txt", ios::in);

    // �������, ������� ������� ���������
    unsigned int counter = 0;

    // ���������� ������ ������� �������
    while (bound2_s.good())
    {
        // ������ ����� �����, � ������� ������ ������ �������
        unsigned int node_beg, node_end;
        bound2_s >> node_beg >> node_end;
        // �������� ���� � ���� �����
        double theta[2];
        bound2_s >> theta[0] >> theta[1];
        if (!bound2_s.good())
            break;
        // ���������� ����
        double hx = fabs(nodes[node_beg].x - nodes[node_end].x);
        double hy = fabs(nodes[node_beg].y - nodes[node_end].y);
        // �����������, � ����� ��������� ������ �������
        double h;
        if (hx > hy)
            h = hx;
        else
            h = hy;
        // ���������� ���������, �� ������� ����� ���������� ��������� �������
        double Mb_c = h / 6.0;
        // ���������� ���� � ��������� ��������
        double fr[2];
        for (int j = 0; j < 2; j++)
        {
            fr[j] = 0.0;
            for (int k = 0; k < 2; k++)
            {
                fr[j] += Mb[j][k] * theta[k];
            }
        }
        // ������� � ������ �����
        slae.f[node_beg] += Mb_c * fr[0];
        slae.f[node_end] += Mb_c * fr[1];

        // �������� �������
        counter++;
    }
    bound2_s.close();
    cout << " > Applied " << counter << " boundary value of type II." << endl;

    // �������, ������� ������� ���������
    counter = 0;

    // ���������� ������ ������� �������
    while (bound1_s.good())
    {
        // ����, � ������� ������ �������
        unsigned int node_b1;
        // �������� ��������
        double value_b1;
        bound1_s >> node_b1 >> value_b1;
        if (!bound1_s.good())
            break;

        // ���� ������ ������� "��-�������"
        /*const double big_number = 1e30;
        // � ��������� ����� ������� �����
        slae.di[node_b1] = big_number;
        // � ������ ����� ��� ��, ���������� �� �������� �������� �������
        slae.f[node_b1] = big_number * value_b1;*/

        // ���� ������ ������� "��-��������"
        // � ��������� ����� 1
        slae.di[node_b1] = 1.0;
        // � ������ ����� ����� �������� ��������
        slae.f[node_b1] = value_b1;
        // � ��� ��� ��� �������
        // ��� ���� �������� ������, � � ��� ������������ �������
        // ������� ����� ������ �� �������, �������� �����
        // � ��, ��� ���� � �������� - ���������� � ������ �����
        unsigned int i_s = slae.ig[node_b1], i_e = slae.ig[node_b1 + 1];
        for (unsigned int i = i_s; i < i_e; i++)
        {
            slae.f[slae.jg[i]] -= slae.gg[i] * value_b1;
            slae.gg[i] = 0.0;
        }
        for (unsigned int p = node_b1 + 1; p < node_num; p++)
        {
            unsigned int i_s = slae.ig[p], i_e = slae.ig[p + 1];
            for (unsigned int i = i_s; i < i_e; i++)
            {
                if (slae.jg[i] == node_b1)
                {
                    slae.f[p] -= slae.gg[i] * value_b1;
                    slae.gg[i] = 0.0;
                }
            }
        }

        // �������� �������
        counter++;
    }
    bound1_s.close();
    cout << " > Applied " << counter << " boundary value of type I." << endl;
}

// ��������� ������� � ������������ �����
double FEM::get_solution(double x, double y)
{
    // ����������� ��, � ������� ������ �����
    bool finded = false;
    unsigned int fe_sol = 0;
    for (unsigned int i = 0; i < fe_num && !finded; i++)
    {
        if (x >= nodes[finite_elements[i].node_n[0]].x && x <= nodes[finite_elements[i].node_n[1]].x &&
            y >= nodes[finite_elements[i].node_n[0]].y && y <= nodes[finite_elements[i].node_n[2]].y)
        {
            finded = true;
            fe_sol = i;
        }
    }

    // ���� �� �����, ������ ����� �� ��������� �������
    if (!finded)
    {
        cerr << "Error: Target point is outside area!" << endl;
        return 0.0;
    }

    // ���� �����, �� ������� ����� �������� ����������� �������� ������� �� ��������������� ����

    // ���������� ����
    double hx = fabs(nodes[finite_elements[fe_sol].node_n[1]].x - nodes[finite_elements[fe_sol].node_n[0]].x);
    double hy = fabs(nodes[finite_elements[fe_sol].node_n[2]].y - nodes[finite_elements[fe_sol].node_n[0]].y);

    // ������� �������� ���������� �������
    double X1 = (nodes[finite_elements[fe_sol].node_n[1]].x - x) / hx;
    double X2 = (x - nodes[finite_elements[fe_sol].node_n[0]].x) / hx;
    double Y1 = (nodes[finite_elements[fe_sol].node_n[2]].y - y) / hy;
    double Y2 = (y - nodes[finite_elements[fe_sol].node_n[0]].y) / hy;

    // ������� �������� ���������� �������� �������
    double psi[4];
    psi[0] = X1 * Y1;
    psi[1] = X2 * Y1;
    psi[2] = X1 * Y2;
    psi[3] = X2 * Y2;

    // �������� ���������� �������� ������� �� ����
    double result = 0.0;
    for (unsigned int i = 0; i < 4; i++)
        result += slae.q[finite_elements[fe_sol].node_n[i]] * psi[i];

    return result;
}

// ��������� �������� B � ������������ �����
double FEM::get_b(double x, double y)
{
    // ����������� ��, � ������� ������ �����
    bool finded = false;
    unsigned int fe_sol = 0;
    for (unsigned int i = 0; i < fe_num && !finded; i++)
    {
        if (x >= nodes[finite_elements[i].node_n[0]].x && x <= nodes[finite_elements[i].node_n[1]].x &&
            y >= nodes[finite_elements[i].node_n[0]].y && y <= nodes[finite_elements[i].node_n[2]].y)
        {
            finded = true;
            fe_sol = i;
        }
    }

    // ���� �� �����, ������ ����� �� ��������� �������
    if (!finded)
    {
        cerr << "Error: Target point is outside area!" << endl;
        return 0.0;
    }

    // ���������� ����
    double hx = fabs(nodes[finite_elements[fe_sol].node_n[1]].x - nodes[finite_elements[fe_sol].node_n[0]].x);
    double hy = fabs(nodes[finite_elements[fe_sol].node_n[2]].y - nodes[finite_elements[fe_sol].node_n[0]].y);

    double Gx1_c = 1.0 / 6.0 * hy / hx;
    double Gx2_c = 1.0 / 6.0 * hx / hy;

    double mes = hx * hy;
    // B^2 = 1/mes * sumi sumj �[i][j]*qi*qj
    double B2 = 0.0;
    for (unsigned int k = 0; k < 4; k++)
        for (unsigned int j = 0; j < 4; j++)
            B2 += (Gx1_c * Gx[k][j] + Gx2_c * Gy[k][j]) * slae.q[finite_elements[fe_sol].node_n[k]] * slae.q[finite_elements[fe_sol].node_n[j]];
    B2 = B2 / mes;

    return sqrt(B2);
}

// ��������� ������
void SLAE::alloc_all(unsigned int gg_size)
{
    // ��������� ������
    ig = new unsigned int[n + 1];
    jg = new unsigned int[gg_size];
    di = new double[n];
    gg = new double[gg_size];
    f = new double[n];
    q = new double[n];
    // ��������� ���� ��������
    memset(di, 0, sizeof(double) * n);
    memset(f, 0, sizeof(double) * n);
    memset(q, 0, sizeof(double) * n);
    memset(gg, 0, sizeof(double) * gg_size);
}

// ��������� gg, di � f
void SLAE::clean()
{
    memset(di, 0, sizeof(double) * n);
    memset(f, 0, sizeof(double) * n);
    memset(gg, 0, sizeof(double) * ig[n]);
}

// �����������
SLAE::SLAE()
{
    // ��������� ���������� (�� ������ ������)
    ig = jg = NULL;
    di = gg = f = q = NULL;
}

// ����������
SLAE::~SLAE()
{
    // ������������ ������
    if (ig) delete[] ig;
    if (jg) delete[] jg;
    if (di) delete[] di;
    if (gg) delete[] gg;
    if (f) delete[] f;
    if (q) delete[] q;
}

// ���������� � ������� �������� elem � ������� i,j
void SLAE::add(unsigned int i, unsigned int j, double elem)
{
    unsigned int ind = 0;
    bool flag = false;
    // �������� �� ������ ������
    for (unsigned int k = ig[i]; k < ig[i + 1] && !flag; k++)
    {
        // ���� ������ �������
        if (jg[k] == j)
        {
            ind = k;
            flag = true;
        }
    }
    // ���������
    gg[ind] += elem;
}

// ������� ������� ����
void SLAE::solve()
{
    cout << "Solving SLAE..." << endl;
    cgm.init(ig, jg, di, gg, f, n);
    cgm.solve(q);
}
