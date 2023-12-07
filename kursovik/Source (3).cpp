#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <Windows.h>
#include <array>

using namespace std;

int N; //количество узлов
int elems_count; //количество конечных элементов
int edges_count; //количество рёбер с краевым условием
double** A; //глобальная матрица
double** M; //глобальная матрица массы
double* q; //вектор решения
double* b; //глобальный вектор правой части
double* d; //вектор для времени (правая часть)
vector<double> t; //вектор временных слоёв

double** L;
double** U;

double* y; //прямой ход гауса
double* x; //обратный ход гауса

//double f(double t, double x)
//{
//    return 20 * x * t + 4;
//}
//
//double u(double t, double x)
//{
//    return 5 * x * t * t + 2 * t;
//}
//
//double sigma()
//{
//    return 2;
//}

class node //узел
{
public:
    double x = 0;
    double y = 0;    
    node() {};
    node(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
};
vector<node> nodes; //все узлы сетки, считываемые из файла

class elem //прямоугольный конечный элемент
{
public:
    int nodes_num[4]; //глобальная нумерация узлов текущего элемента, определяет 4 вершины

    double x0, x1; //границы элемента по x
    double y0, y1; //границы элемента по y
    double hx, hy; //длина и высота элемента    
    double x[4]; //значения х в узлах элемента
    double lambda = 0; //усреднённое значение лямбда на текущем элементе
    double gamma = 0; //усреднённое значение гамма на текущем элементе
    double c0 = 0, c1 = 0; //коэффициенты функции на текущем элементе

    double f(double x)
    {
        return c0 + c1 * x;
    }

    //Матрицы для сборки локальной матрицы жёсткости и массы:
    double Gx[2][2]; double Gy[2][2];
    double Mx[2][2]; double My[2][2];

    double G[4][4]; //Локальная матрица жёсткости
    double _M[4][4]; //Локальная матрица массы
    double C[4][4]; //Для получения вектора _b правой части

    double _A[4][4]; //локальная матрица
    double _b[4]; //локальный вектор правой части

public:
    elem() //конструктор по умолчанию
    {
        x0 = x1 = 0;
        y0 = y1 = 0;
        hx = hy = 0;

        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
            {
                Gx[i][j] = Gy[i][j] = 0;
                Mx[i][j] = My[i][j] = 0;
            }

        for (int i = 0; i < 4; i++)
        {
            nodes_num[i] = 0;
            x[i] = 0;
            _b[i] = 0;
            for (int j = 0; j < 4; j++)
            {
                _A[i][j] = 0;
                G[i][j] = _M[i][j] = C[i][j] = 0;                
            }
        }
    }

    elem(int nodes_num[4], double lambda, double gamma, double c0, double c1) //основной конструктор
    {
        for (int i = 0; i < 4; i++)
        {
            this->nodes_num[i] = nodes_num[i];
        }
        double x0, x1; //границы элемента по x
        double y0, y1; //границы элемента по y
        x0 = nodes[nodes_num[0]].x;
        x1 = nodes[nodes_num[1]].x;
        y0 = nodes[nodes_num[0]].y;
        y1 = nodes[nodes_num[2]].y;

        this->x0 = x0; this->x1 = x1;
        this->y0 = y0; this->y1 = y1;
        hx = fabs(x0 - x1);
        hy = fabs(y0 - y1);

        x[0] = x0; x[1] = x1;
        x[2] = x0; x[3] = x1;

        this->lambda = lambda;
        this->gamma = gamma;
        this->c0 = c0;
        this->c1 = c1;

        //================ Подготовка сборки ================================
        {
            Gx[0][0] = 1 / hx; Gx[0][1] = -1 / hx;
            Gx[1][0] = -1 / hx; Gx[1][1] = 1 / hx;

            Gy[0][0] = 1 / hy; Gy[0][1] = -1 / hy;
            Gy[1][0] = -1 / hy; Gy[1][1] = 1 / hy;

            Mx[0][0] = hx / 3; Mx[0][1] = hx / 6;
            Mx[1][0] = hx / 6; Mx[1][1] = hx / 3;

            My[0][0] = hy / 3; My[0][1] = hy / 6;
            My[1][0] = hy / 6; My[1][1] = hy / 3;
        }
        //===================================================================

        //=============== Cборка локальной матрицы жёсткости ================
        {
            G[0][0] = lambda * (Gx[0][0] * My[0][0] + Mx[0][0] * Gy[0][0]);
            G[0][1] = lambda * (Gx[0][1] * My[0][0] + Mx[0][1] * Gy[0][0]);
            G[0][2] = lambda * (Gx[0][1] * My[0][0] + Mx[0][0] * Gy[0][1]);
            G[0][3] = lambda * (Gx[0][1] * My[0][1] + Mx[0][1] * Gy[0][1]);

            G[1][0] = G[0][1];
            G[1][1] = lambda * (Gx[1][1] * My[0][0] + Mx[1][1] * Gy[0][0]);
            G[1][2] = lambda * (Gx[1][0] * My[0][1] + Mx[1][0] * Gy[0][1]);
            G[1][3] = lambda * (Gx[1][1] * My[0][1] + Mx[1][1] * Gy[0][1]);

            G[2][0] = G[0][2];
            G[2][1] = G[1][2];
            G[2][2] = lambda * (Gx[0][0] * My[1][1] + Mx[0][0] * Gy[1][1]);
            G[2][3] = lambda * (Gx[0][1] * My[1][1] + Mx[0][1] * Gy[1][1]);

            G[3][0] = G[0][3];
            G[3][1] = G[1][3];
            G[3][2] = G[2][3];
            G[3][3] = lambda * (Gx[1][1] * My[1][1] + Mx[1][1] * Gy[1][1]);
        }
        //===================================================================


        //============ Сборка локальной матрицы массы =======================
        {
            _M[0][0] = gamma * Mx[0][0] * My[0][0];
            _M[0][1] = gamma * Mx[0][1] * My[0][0];
            _M[0][2] = gamma * Mx[0][0] * My[0][1];
            _M[0][3] = gamma * Mx[0][1] * My[0][1];

            _M[1][0] = _M[0][1];
            _M[1][1] = gamma * Mx[1][1] * My[0][0];
            _M[1][2] = gamma * Mx[1][0] * My[0][1];
            _M[1][3] = gamma * Mx[1][1] * My[0][1];

            _M[2][0] = _M[0][2];
            _M[2][1] = _M[1][2];
            _M[2][2] = gamma * Mx[0][0] * My[1][1];
            _M[2][3] = gamma * Mx[0][1] * My[1][1];

            _M[3][0] = _M[0][3];
            _M[3][1] = _M[1][3];
            _M[3][2] = _M[2][3];
            _M[3][3] = gamma * Mx[1][1] * My[1][1];
        }
        //===================================================================

        //============ Создание матрицы C ===================================
        {
            C[0][0] = 1 * Mx[0][0] * My[0][0];
            C[0][1] = 1 * Mx[0][1] * My[0][0];
            C[0][2] = 1 * Mx[0][0] * My[0][1];
            C[0][3] = 1 * Mx[0][1] * My[0][1];

            C[1][0] = C[0][1];
            C[1][1] = 1 * Mx[1][1] * My[0][0];
            C[1][2] = 1 * Mx[1][0] * My[0][1];
            C[1][3] = 1 * Mx[1][1] * My[0][1];

            C[2][0] = C[0][2];
            C[2][1] = C[1][2];
            C[2][2] = 1 * Mx[0][0] * My[1][1];
            C[2][3] = 1 * Mx[0][1] * My[1][1];

            C[3][0] = C[0][3];
            C[3][1] = C[1][3];
            C[3][2] = C[2][3];
            C[3][3] = 1 * Mx[1][1] * My[1][1];
        }
        //===================================================================

        //============ Заполнение матрицы _A ================================
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                _A[i][j] = G[i][j];
            }
        }
        //===================================================================

        //============ Нахождение вектора _b правой части ===================
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                _b[i] += C[i][j] * f(x[j]);
            }
        }
        //===================================================================
    }

   /* double X1(double x)
    {
        return (x1 - x) / hx;
    }
    double X2(double x)
    {
        return (x - x0) / hx;
    }
    double Y1(double y)
    {
        return (y1 - y) / hy;
    }
    double Y2(double y)
    {
        return (y - y0) / hy;
    }

    double psi_1(double x, double y)
    {
        return X1(x) * Y1(y);
    }
    double psi_2(double x, double y)
    {
        return X2(x) * Y1(y);
    }
    double psi_3(double x, double y)
    {
        return X1(x) * Y2(y);
    }
    double psi_4(double x, double y)
    {
        return X2(x) * Y2(y);
    }*/

    void send_to_global() //отправка в глобальную матрицу и вектор правой части
    {         
        for (int i = 0; i < 4; i++)
        {
            b[nodes_num[i]] += _b[i];
            for (int j = 0; j < 4; j++)
            {
                A[nodes_num[i]][nodes_num[j]] += _A[i][j];
                M[nodes_num[i]][nodes_num[j]] += _M[i][j];
            }
        }
    }

};
vector<elem> elems; //все конечные элементы рабочей области

class edge //краевое ребро
{
private:
    int nodes_num[2]; //2 вершины ребра
    double h = 0; //длина ребра
    double teta = 0; //2-е краевое условие (константа)

    double _b[2]; //локальный вектор правой части
    double _A[2][2]; //локальная матрица

public:
    edge() //конструктор по умолчанию
    {
        for (int i = 0; i < 2; i++)
        {
            nodes_num[i] = 0;
            _b[i] = 0;
            for (int j = 0; j < 2; j++)
            {
                _A[i][j] = 0;
            }
        }
    }

    edge(int nodes_num[2], double condition) //основной конструктор
    {
        for (int i = 0; i < 2; i++)
        {
            this->nodes_num[i] = nodes_num[i];
        }
            
        teta = condition;
        h = max(fabs(nodes[nodes_num[0]].x - nodes[nodes_num[1]].x), fabs(nodes[nodes_num[0]].y - nodes[nodes_num[1]].y));
        _b[0] = h / 6 * (2 * teta + teta);
        _b[1] = h / 6 * (teta + 2 * teta);

        for (int i = 0; i < 2; i++)
        {                
            for (int j = 0; j < 2; j++)
            {
                _A[i][j] = 0;
            }
        }        
    }
    

public:
    void send_to_global()
    {
        for (int i = 0; i < 2; i++)
        {
            b[nodes_num[i]] += _b[i];
            for (int j = 0; j < 2; j++)
            {
                A[nodes_num[i]][nodes_num[j]] += _A[i][j];
            }
        }
    }
};
vector<edge> edges;

void input() //ввод из файла
{
    ifstream in("nodes.txt"); //заполнение вектора узлов
    in >> N;
    for (int i = 0; i < N; i++)
    {
        node temp;
        in >> temp.x;
        in >> temp.y;
        nodes.push_back(temp);
    }
    in.close();

    /*for (int i = 0; i < N; i++)
    {
        cout << nodes[i].x << ' ' << nodes[i].y;
        cout << endl;
    }*/

    in.open("elems.txt"); //заполнение вектора конечных элементов
    int nodes_num[4]; double lambda; double gamma; double c0; double c1;
    while (!in.eof())
    {
        for (int i = 0; i < 4; i++)
        {
            in >> nodes_num[i];            
        }
        in >> lambda >> gamma >> c0 >> c1;
        elem temp(nodes_num, lambda, gamma, c0, c1);    
        elems.push_back(temp);

        /*for (int i = 0; i < 4; i++)
            cout << elems[elems.size() - 1].nodes_num[i] << ' ';
        cout << endl;*/
    }
    in.close();

    in.open("edges.txt"); //заполнение вектора рёбер
    int nodes_num_edge[2]; double condition;
    while (!in.eof())
    {
        for (int i = 0; i < 2; i++)
        {
            in >> nodes_num_edge[i];
        }
        in >> condition;
        edge temp(nodes_num_edge, condition);
        edges.push_back(temp);
    }
    in.close();

    in.open("time.txt");
    while (!in.eof())
    {
        double temp;
        in >> temp;
        t.push_back(temp);
    }
    in.close();
}



void build() //построение глобальной матрицы и правой части
{
    A = new double* [N]; //глобальная матрица
    M = new double* [N]; //глобальная матрица массы
    for (int i = 0; i < N; i++)
    {
        A[i] = new double[N]();
        M[i] = new double[N]();
    }

    b = new double[N](); //глобальный вектор правой части

    /*for (int i = 0; i < N; i++)
    {
        cout << b[i] << ' ';
    }
    cout << endl;*/

    for (int i = 0; i < elems.size(); i++)
    {
        /*for (int i = 0; i < 4; i++)
            cout << elems[i].nodes_num[i] << ' ';
        cout << endl;*/

        elems[i].send_to_global();
    }

    /*for (int i = 0; i < N; i++)
    {
        cout << b[i] << ' ';
    }
    cout << endl;*/
}

void S2() //Применение вторых краевых условий
{
    for (int i = 0; i < edges.size(); i++)
    {
        edges[i].send_to_global();
    }
}

double ug(double x, double c0, double c1)
{
    return c0 + c1 * x;
}

void S1() //Применение первых краевых условий
{
    ifstream in("S1.txt");
    int num;
    double c0, c1;
    while (!in.eof())
    {
        in >> num >> c0 >> c1;
        for (int j = 0; j < N; j++)
        {
            A[num][j] = 0;
        }
        A[num][num] = 1;
        b[num] = ug(nodes[num].x, c0, c1);
    }
    in.close();
}



void output() //вывод результатов
{
    ofstream out("q.txt");
    for (int i = 0; i < N; i++)
    {
        out << q[i] << endl;
    }
    out.close();
}

void LU()
{
    L = new double* [N];
    U = new double* [N];

    for (int k = 0; k < N; k++)
    {
        L[k] = new double[N]();
        U[k] = new double[N]();
        U[k][k] = 1;
    }

    L[0][0] = A[0][0];
    for (int i = 1; i < N; i++)
    {
        for (int j = 0; j <= i - 1; j++)
        {
            L[i][j] = A[i][j];
            for (int k = 0; k <= j - 1; k++)
            {
                L[i][j] -= L[i][k] * U[k][j];
            }
        }

        for (int j = 0; j <= i - 1; j++)
        {
            U[j][i] = 1.0 / L[j][j] * A[j][i];
            for (int k = 0; k <= j - 1; k++)
            {
                U[j][i] -= 1.0 / L[j][j] * L[j][k] * U[k][i];
            }
        }

        L[i][i] = A[i][i];
        for (int k = 0; k <= i - 1; k++)
        {
            L[i][i] -= L[i][k] * U[k][i];
        }
    }
}

double gauss(double** A, double* q, double* b, int N) //Метод Гаусса
{
    double max;
    int k, index;
    const double eps = 1e-15;  //точность 
    k = 0;
    while (k < N)
    {
        //Поиск строки с максимальным A[i][k] 
        max = abs(A[k][k]);
        index = k;
        for (int i = k + 1; i < N; i++)
        {
            if (abs(A[i][k]) > max)
            {
                max = abs(A[i][k]);
                index = i;
            }
        }

        //Проверка на наличие ненулевых элементов в столбце
        if (max <= eps)
        {
            cout << "Обнаружен нулевой столбец с номером (индексом) " << index << endl;
        }

        //Перестановка строк 
        for (int j = 0; j < N; j++)
        {
            double temp = A[k][j];
            A[k][j] = A[index][j];
            A[index][j] = temp;
        }

        double temp = b[k];
        b[k] = b[index];
        b[index] = temp;

        //Нормализация уравнений 
        for (int i = k; i < N; i++)
        {
            double temp = A[i][k];
            if (abs(temp) > eps) //Для нулевого коэффициента пропустить 
            {
                for (int j = 0; j < N; j++)
                {
                    A[i][j] = A[i][j] / temp;
                }

                b[i] = b[i] / temp;
                if (i != k) { //Уравнение не вычитать само из себя 
                    for (int j = 0; j < N; j++)
                    {
                        A[i][j] = A[i][j] - A[k][j];
                    }
                    b[i] = b[i] - b[k];
                }
            }
        }
        k++;
    }
    
    //Обратный ход 
    for (k = N - 1; k >= 0; k--)
    {
        q[k] = b[k];
        for (int i = 0; i < k; i++)
        {
            b[i] = b[i] - A[i][k] * q[k];
        }
    }
    return 0;
}

void time()
{
    double* q0 = q;
    double* q1 = q;
    d = new double[N]();
    for (int i = 0; i < N; i++)
    {
        d[i] = b[i];
        for (int j = 0; j < N; j++)
        {
            A[i][j] = (25.0 / 3.0 * M[i][j] + A[i][j] + M[i][j]);
            d[i] -= 15 * M[i][j] * q[i];
            d[i] += 20 / 3 * M[i][j] * q[i];
        }
    }

    double* q2 = new double[N];
    gauss(A, q2, d, N);

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i][j] = (25.0 / 12.0 * M[i][j] + A[i][j] + M[i][j]);
            d[i] -= 25 / 4 * M[i][j] * q[i];
            d[i] += 5 / 3 * M[i][j] * q[i];
        }
    }
    double* q3 = new double[N];
    gauss(A, q3, d, N);

    ofstream out("q3.txt");
    for (int i = 0; i < N; i++)
    {
        out << q[i] << endl;
    }
    out.close();
}



int main()
{
    setlocale(LC_ALL, "rus");
    input();
    build();
    S2();
    S1();

    LU();
    //gauss(L, y, b, N);
    //gauss(U, x, y, N);

    /*for (int i = 0; i < elems.size(); i++)
    {
        for (int j = 0; j < 4; j++)
            cout << elems[i].nodes_num[j] << ' ';
        cout << endl;
    }*/
    
    //system("pause");
    q = new double[N];
    gauss(A, q, b, N);
    time();
    output();
    system("pause");
}
