#pragma once
#ifndef GRID_H
#define GRID_H
#include <vector>
#include <functional>
#include <fstream>
#include <algorithm>
#include <iomanip>


struct point
{
   double x;
   double y;
};

class grid
{
public:
   double angle_step(int n, double k);
   double coord_step(double x0, double x1 ,int n, double k);
   void get_grid(std::string file);

   std::vector<point> MeshXY;

private:
   double Pi = 3.1415926535;
   std::vector<point> st;                          // Ключевые точки
   std::vector<point> points;                      // Набор точек сетки
   std::vector<point> g1, g2, g3, g4;              // Набор точек на соответствующих гранях гранях
   double R;                                       // Радиус окружности
   point center;                                   // Центр окружностей
   double k_line, k_circle;                        // Коэффициенты разрядки для линий и дуг окружностей
   int n_line, n_circle;                           // Число отрезков на линиях и дугах окружностей
   std::vector<std::vector<point>> ins_points;     // Структура для хранения точек, находящихся внутри фигуры
};

#endif