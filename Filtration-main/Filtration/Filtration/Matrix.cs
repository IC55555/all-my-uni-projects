using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using System.Threading.Tasks;

namespace Filtration
{
   public class Matrix
   {
      public double[][] Mat { get; set; }
      public Matrix(int n, int m)
      {
         Mat = new double[n][];
         for (int i = 0; i < n; i++)
         {
            Mat[i] = new double[m];
            for (int k = 0; k < m; k++)
               Mat[i][k] = 0;
         }
      }
      public Matrix() => Mat = new double[0][];
      public Matrix(double[][] mat) => Mat = mat;

      public double[] this[int index] => Mat[index];
      public void PrintMatrix()
      {
         for (int i = 0; i < Mat.Length; i++)
         {
            for (int j = 0; j < Mat[0].Length; j++)
               Console.Write(Mat[i][j].ToString("E3") + " ");
            Console.WriteLine();
         }
      }
      public Matrix Transpose()
      {
         Matrix result = new Matrix(Mat[0].Length, Mat.Length);
         for (int i = 0; i < Mat.Length; i++)
            for (int j = 0; j < Mat[0].Length; j++)
               result[j][i] = Mat[i][j];
         return result;
      }
      public Matrix LLTDecomposition()
      {
         Matrix result = new Matrix(Mat.Length, Mat[0].Length);
         result[0][0] = Math.Sqrt(Mat[0][0]);
         for (int j = 1; j < Mat.Length; j++)
            result[j][0] = Mat[j][0] / result[0][0];
         for (int i = 1; i < Mat.Length; i++)
         {
            double sum = 0;
            for (int p = 0; p < i; p++)
               sum += result[i][p] * result[i][p];
            result[i][i] = Math.Sqrt(Mat[i][i] - sum);
            sum = 0;
            for (int j = 0; j < Mat[0].Length; j++)
            {
               for (int p = 0; p < i; p++)
                  sum += result[i][p] * result[j][p];
               result[j][i] = 1.0 / result[i][i] * (Mat[j][i] - sum);
            }
         }
         return result;
      }
      public static Matrix operator +(Matrix a, Matrix b)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = a[i][j] + b[i][j];
         return result;
      }
      public static Matrix operator -(Matrix a, Matrix b)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = a[i][j] - b[i][j];
         return result;
      }
      public static Matrix operator -(Matrix a)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = -a[i][j];
         return result;
      }
      public static Matrix operator *(Matrix a, Matrix b)
      {
         var result = new Matrix(a.Mat.Length, b.Mat[0].Length);
         for (int i = 0; i < a.Mat.Length; i++)
            for (int j = 0; j < b.Mat[0].Length; j++)
               for (int k = 0; k < b.Mat.Length;k++)
               result[i][j] += a[i][k] * b[k][j];
         return result;
      }
      public static Matrix operator *(Matrix a, double b)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = a[i][j] * b;
         return result;
      }
      public static Matrix operator *(double b, Matrix a)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = a[i][j] * b;
         return result;
      }

      public static Matrix operator +(Matrix a, double b)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = (i == j) ? (a[i][j] + b) : a[i][j];  
         return result;
      }
      public static Matrix operator +(double b, Matrix a)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = (i == j) ? (a[i][j] + b) : a[i][j];
         return result;
      }
      public static Matrix operator -(Matrix a, double b)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = (i == j) ? (a[i][j] - b) : a[i][j];
         return result;
      }
      public static Matrix operator -(double b, Matrix a)
      {
         var result = new Matrix(a.Mat.Length, a.Mat[0].Length);
         for (int i = 0; i < result.Mat.Length; i++)
            for (int j = 0; j < result.Mat[0].Length; j++)
               result[i][j] = (i == j) ? (b - a[i][j]) : a[i][j];
         return result;
      }
   }
}
