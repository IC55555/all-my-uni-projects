namespace Filtration
{
   internal class Program
   {
      static void Main(string[] args)
      {
         //var a = new Matrix(new double[][]
         //{
         //   new double[] {1,0,0},
         //   new double[] {0,5,0},
         //   new double[] {0,0,9},
         //});
         //var b = a.LLTDecomposition();
         //b.PrintMatrix();
         var a = new PotterAlgorithm();
         a.Iteration();

      }
   }
}