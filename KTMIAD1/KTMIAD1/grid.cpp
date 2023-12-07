#include "grid.h"
#include <iostream>

double grid::angle_step(int n, double k)
{
   if (k != 1)
      return 90.0 * (1 - k) / (1 - pow(k, n));
   else
      return 90.0 / n;
}

double grid::coord_step(double x0, double x1, int n, double k)
{
   if (k != 1)
      return (x1 - x0) * (1 - k) / (1 - pow(k, n));
   else
      return (x1 - x0) / n;
}

void grid::get_grid(std::string file)
{
   st.resize(4);
   std::ifstream inGrid(file);
   if (inGrid.is_open())
   {
      inGrid >> center.x >> center.y;

      for (int i = 0; i < 4; i++)
      {
         inGrid >> st[i].x >> st[i].y;
      }

      inGrid >> n_line >> k_line
             >> n_circle >> k_circle;
   }

   // Нижняя и верхняя дуги окружности
   double ha = angle_step(n_circle, k_circle);
   g2.push_back(st[0]);
   g4.push_back(st[1]);

   double tmp_h = ha;
   double tmp_angle = 90.0;
   point tmp_point1;
   point tmp_point2;
   double R1 = g2[0].y - center.y;
   double R2 = g4[0].y - center.y;
   for (int i = 1; i <= n_circle; i++)
   {
      tmp_angle -= tmp_h;
      tmp_point1.x = center.x + R1 * cos(Pi * tmp_angle / 180);
      tmp_point1.y = center.y + R1 * sin(Pi * tmp_angle / 180);
      tmp_point2.x = center.x + R2 * cos(Pi * tmp_angle / 180);
      tmp_point2.y = center.y + R2 * sin(Pi * tmp_angle / 180);
      g2.push_back(tmp_point1);
      g4.push_back(tmp_point2);
      tmp_h *= k_circle;
   }

   // Вертикаль и горизонталь
   g1.push_back(st[0]);
   g3.push_back(st[2]);

   double h = coord_step(st[2].x, st[3].x, n_line, k_line);

   for (int i = 1; i <= n_line; i++)
   {
      tmp_point1.x = g1[0].x;
      tmp_point1.y = g1[i-1].y + h;
      tmp_point2.x = g3[i - 1].x + h;
      tmp_point2.y = g3[0].y;
      g1.push_back(tmp_point1);
      g3.push_back(tmp_point2);
      h *= k_line;
   }


   //Внутреннее разбиение
   int size = n_line - 1;
   ins_points.resize(size);
   for (int i = 0; i < size; i++)
   {
      double R = g1[i + 1].y - center.y;
      tmp_h = ha;
      tmp_angle = 90.0;
      ins_points[i].push_back(g1[i + 1]);
      for (int j = 1; j <= n_circle; j++)
      {
         tmp_angle -= tmp_h;
         tmp_point1.x = center.x + R * cos(Pi * tmp_angle / 180);
         tmp_point1.y = center.y + R * sin(Pi * tmp_angle / 180);
         ins_points[i].push_back(tmp_point1);
         tmp_h *= k_circle;
      }
   }


   //Сборка всех значений в массив сетки
   // Узлы идут в порядке увеличения радиусов окружностей по часовой стрелке

   for (int i = 0; i < g2.size(); i++)
   {
      MeshXY.push_back(g2[i]);
   }

   for (int i = 0; i < size; i++)
   {
      for (int j = 0; j < ins_points[i].size(); j++)
      {
         MeshXY.push_back(ins_points[i][j]);
      }
   }

   for (int i = 0; i < g4.size(); i++)
   {
      MeshXY.push_back(g4[i]);
   }
}

