using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Filtration
{
   public static class Parameters
   {
      //константы
      public static Matrix E = new Matrix(new double[][]
      {
         new double[] {1, 0, 0, 0, 0},
         new double[] {0, 1, 0, 0, 0},
         new double[] {0, 0, 1, 0, 0},
         new double[] {0, 0, 0, 1, 0},
         new double[] {0, 0, 0, 0, 1},
      });
      //Значения задачи
      public readonly static double omega = Math.PI / 2.0;
      public readonly static double beta = 10; //расход топлива
      public readonly static double c = 1;
      public readonly static double g = 9.8;
      public readonly static double m0 = 9.8;
      public readonly static double a = c*beta*Math.Cos(omega); //нач. полож по x
      public readonly static double b = c * beta * Math.Sin(omega);
      public readonly static double c1 = -g;
      public readonly static double d = -1.0/beta; // масса ракеты + топлива
       
      
      public static Matrix GetF(double tk1, double tk)
      {
         double T = tk1 - tk;
         var result = new Matrix();
         result.Mat = new double[][]
         {
            new double[] {1.0, 0.0, T, 0.0, a*T*T/2},
            new double[] {0, 1, 0, T, b*T*T/2},
            new double[] {0, 0, 1, 0, a*T},
            new double[] {0, 0, 0, 1, b*T},
            new double[] {0, 0, 0, 0, 1},
         };
         return result;
      }
      public static Matrix GetQ(double tk1, double tk)
      {
         double T = tk1 - tk;
         var result = new Matrix();
         result.Mat = new double[][]
         {
            new double[] { a*c1*T*T*T/6.0},
            new double[] {c1*T*T/2 + b*d*T*T*T/6, },
            new double[] {a*d*T*T/2.0, },
            new double[] {b*d*T*T/2 + c1*T, },
            new double[] {d*T, },
         };
         return result;
      }
      public static Matrix H = new Matrix(new double[][]
      {
         new double[] {0, 1, 0, 0, 0},
      });
      //Параметры фильтра
      public readonly static double sigma = 2; // нач. отклон ошибок
      public static Matrix x0 = new Matrix(new double[][] { new double[] { 0, 0, 0, 0, m0 } });



      public static double R = 1e-106;
      public static double k = 1e-106;
      public static Matrix P0 = new Matrix(new double[][]
      {
         new double[] {k, 0, 0, 0, 0},
         new double[] {0, k, 0, 0, 0},
         new double[] {0, 0,k, 0, 0},
         new double[] {0, 0, 0, k, 0},
         new double[] {0, 0, 0, 0, k },
      });
      
      public static Matrix vk = new Matrix(new double[][] { new double[] { 1, 1, 0, 0, 0 } });
   }
}
