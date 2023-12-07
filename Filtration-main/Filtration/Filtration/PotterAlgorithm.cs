using Microsoft.VisualBasic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Filtration
{
   internal class PotterAlgorithm
   {
      Matrix x_kk;
      Matrix s_kk;
      Matrix p_kk;
      Matrix x_k1k1;
      Matrix s_k1k1;
      Matrix p_k1k1;
      Matrix x_k1k;
      Matrix s_k1k;
      Matrix p_k1k;
      Matrix yk1;
      void Extrapolation(double tk1, double tk)
      {
         var F_k1k = Parameters.GetF(tk1, tk);
         x_k1k = F_k1k * x_kk + Parameters.GetQ(tk1, tk);
         s_k1k = F_k1k * s_kk;
         p_k1k = s_k1k * s_k1k.Transpose();
      }
      void Filtration(double tk1, double tk)
      {
         var Fk1 = s_k1k.Transpose() * Parameters.H.Transpose();
         double a = 1.0 / (Fk1.Transpose() * Fk1 + Parameters.R).Mat[0][0];
         var K = a * s_k1k * Fk1;
         x_k1k1 = x_k1k + K * (yk1 - Parameters.H * x_k1k);
         double gamma = 1.0 / (1 + Math.Sqrt(Parameters.R * a));
         s_k1k1 = s_k1k * (1 - a * gamma * Fk1 * Fk1.Transpose());
         p_k1k1 = s_k1k1 * s_k1k1.Transpose();
      }
      double Norm(Matrix x)
      {
         double sum = 0;
         for (int i = 0; i < x[0].Length; i++)
            sum += x[0][i] * x[0][i];
         return Math.Sqrt(sum);
      }
      List<double> CreateTimeMesh(double t0, double T, int k)
      {
         var time = new List<double>();
         double h = (T - t0) / k;
         for (int i = 0; i < k; i++)
         {
            time.Add(t0 + h * i);
         }
         return time;
      }
      public void Iteration()
      {
         var S0 = Parameters.P0.LLTDecomposition();
         p_kk = S0 * S0.Transpose();
         x_kk = Parameters.x0.Transpose();
         s_kk = S0;
         var timeMesh = CreateTimeMesh(0, 3, 400);
         List<double> results = new List<double>();
         List<Matrix> y_trList = new List<Matrix>();
         List<Matrix> y_List = new List<Matrix>();
         Matrix x_tr = Parameters.x0.Transpose();
         Matrix y_tr;
         Matrix y;
         for (int i = 1; i < timeMesh.Count; i++)
         {
            x_tr = Parameters.GetF(timeMesh[i], timeMesh[i - 1]) * x_tr + Parameters.GetQ(timeMesh[i], timeMesh[i - 1]);
            yk1 = Parameters.H * x_tr + Parameters.vk;
            Extrapolation(timeMesh[i], timeMesh[i - 1]);
            Filtration(timeMesh[i], timeMesh[i - 1]);

            y_tr = Parameters.H * x_tr;
            y = Parameters.H * x_k1k1;
            y_trList.Add(y_tr);
            y_List.Add(y);
            var a = y_tr - y;
            var b = Norm(y_tr);
            results.Add(Norm(a) / b);
            p_kk = p_k1k1;
            x_kk = x_k1k1;
            s_kk = s_k1k1;
         }
         using (StreamWriter sw = new StreamWriter(@"out.txt"))
         {
            foreach (var item in results)
               sw.WriteLine(item.ToString("E5"));
         }
         using (StreamWriter sw = new StreamWriter(@"y.txt"))
         {
            foreach (var item in y_List)
               sw.WriteLine(item[0][0].ToString("E5"));
         }
         using (StreamWriter sw = new StreamWriter(@"y_tr.txt"))
         {
            foreach (var item in y_trList)
               sw.WriteLine(item[0][0].ToString("E5"));
         }
      }
   }
}
