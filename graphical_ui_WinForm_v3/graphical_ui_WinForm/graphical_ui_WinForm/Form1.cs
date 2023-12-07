using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Collections;

namespace graphical_ui_WinForm
{
    public partial class Form1 : Form
    {
        //variable definitions
        List<List<List<double>>> Curves = new List<List<List<double>>>();
        int Regime = 0;
        bool opened = false;
        double maxX = Double.MinValue, maxY = Double.MinValue,
            minX = Double.MaxValue, minY = Double.MaxValue;
        double zoomX = 1, zoomY = 1, xsign_off, ysign_off;
        //======================

        public Form1()
        {
            InitializeComponent();
            comboBox1.SelectedIndex = 0;
            label1.Text = "0";
            label2.Text = "0";
        }

        private void button1_Click(object sender, EventArgs e)
        {
            List<List<double>> Curve = new List<List<double>>();
            Stream myStream = null;
            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            openFileDialog1.InitialDirectory = ".";
            openFileDialog1.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            openFileDialog1.FilterIndex = 2;
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    if ((myStream = openFileDialog1.OpenFile()) != null)
                    {
                        using (myStream)
                        {
                            var stream = new StreamReader(myStream);

                            while (!stream.EndOfStream)
                            {
                                string s = stream.ReadLine();
                                var strarray = s.Split(' '); // превращает строку в набор строк, разделитель в параметрах

                                double xx = double.Parse(strarray[0]), yy = double.Parse(strarray[1]);
                                List<double> tmp = new List<double>();
                                tmp.Add(xx);
                                tmp.Add(yy);
                                Curve.Add(tmp);

                                if (xx < minX) minX = xx;
                                if (xx > maxX) maxX = xx;
                                if (yy < minY) minY = yy;
                                if (yy > maxY) maxY = yy;
                            }
                        }

                        Curves.Add(Curve);

                        zoomX = (this.panel1.Size.Width) / ((Math.Abs(maxX) - minX)); //2
                        zoomY = (this.panel1.Size.Height) / ((Math.Abs(maxY) - minY));
                        this.panel1.Invalidate();  // вызвать функцию Paint

                        if (minX < 0) xsign_off = minX;
                        else xsign_off = 0;

                        if (minY < 0) ysign_off = minY;
                        else ysign_off = 0;

                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error: Could't read file. Message: " + ex.Message);
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Curves.Clear();
            maxX = Double.MinValue;
            maxY = Double.MinValue;
            minX = Double.MaxValue;
            minY = Double.MaxValue;
            this.panel1.Invalidate();
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            if (Regime == 2 && Curves.Count < 2) return;
            if (Curves.Count != 0)
            {
                //===Просчитаем данные для всех режимов
                var curvesInterp = new List<List<List<double>>>();
                var axisX = new List<double>();
                for (int i = 0; i < Curves.Count; ++i)
                {
                    for (int j = 0; j < Curves[i].Count; ++j)
                    {
                        axisX.Add(Curves[i][j][0]);
                    }
                }
                axisX.Sort();
                axisX = axisX.Distinct().ToList();

                for (int j = 0; j < Curves.Count; ++j)
                {
                    var tmp2 = new List<List<double>>();
                    for (int i = 0; i < axisX.Count; ++i)
                    {
                        var tmp = new List<double>();
                        tmp.Add(axisX[i]);
                        tmp.Add(0);
                        tmp2.Add(tmp);
                    }
                    curvesInterp.Add(tmp2);
                }

                for (int i = 0; i < Curves.Count; ++i)
                {
                    var currCurve = Curves[i];
                    var currCurveI = curvesInterp[i];
                    int ji = 0, prev_ji;

                    while (currCurveI[ji][0] != currCurve[0][0]) ji++;
                    currCurveI[ji][1] = currCurve[0][1];
                    prev_ji = ji;
                    ji++;

                    for (int j = 1; j < currCurve.Count; ++j)
                    {
                        while (currCurveI[ji][0] != currCurve[j][0]) ji++;
                        currCurveI[ji][1] = currCurve[j][1];
                        double a = (currCurveI[ji][0] - currCurveI[prev_ji][0]) / (currCurveI[ji][1] - currCurveI[prev_ji][1]);
                        if (a == Double.PositiveInfinity) a = 0;
                        double b = currCurveI[ji][1] - a * currCurveI[ji][0];
                        for (int k = prev_ji + 1; k < ji; k++)
                        {
                            currCurveI[k][1] = a * currCurveI[k][0] + b;
                        }
                        prev_ji = ji;
                    }
                }

                //Regime 1
                double minR1 = Double.MaxValue, maxR1 = Double.MinValue;
                for (int i = 0; i < Curves.Count; i++)
                {
                    var Curve = Curves[i];
                    for (int j = 0; j < Curve.Count; j++)
                    {
                        if (Curve[j][1] > maxR1) maxR1 = Curve[j][1];
                        if (Curve[j][1] < minR1) minR1 = Curve[j][1];
                    }
                }

                //Regime 2
                var resCurve2 = new List<List<double>>();
                for (int j = 0; j < curvesInterp[0].Count; ++j)
                {
                    double y = 0;
                    for (int i = 0; i < curvesInterp.Count; i++)
                    {
                        y += curvesInterp[i][j][1];
                    }
                    var tmp = new List<double>();
                    tmp.Add(curvesInterp[0][j][0]);
                    tmp.Add(y);
                    resCurve2.Add(tmp);
                }
                double minR2 = Double.MaxValue, maxR2 = Double.MinValue;
                for(int i = 0; i < resCurve2.Count; i++)
                {
                    if (resCurve2[i][1] > maxR2) maxR2 = resCurve2[i][1];
                    if (resCurve2[i][1] < minR2) minR2 = resCurve2[i][1];
                }

                //Regime 3
                var resCurves3 = new List<List<List<double>>>();
                for (int i = 1; i < curvesInterp.Count; i++)
                {
                    var Curve = curvesInterp[i];
                    var CurveNew = new List<List<double>>();
                    for (int j = 0; j < Curve.Count; j++)
                    {
                        double y0 = (Curve[j][1] - curvesInterp[0][j][1]) / curvesInterp[0][j][1];
                        if (y0 == Double.PositiveInfinity || y0 != y0) y0 = 0;
                        var tmp = new List<double>();
                        tmp.Add(Curve[j][0]);
                        tmp.Add(y0);
                        CurveNew.Add(tmp);             
                    }
                    resCurves3.Add(CurveNew);
                }
                double minR3 = Double.MaxValue, maxR3 = Double.MinValue;
                for(int i = 0; i < resCurves3.Count; i++)
                {
                    var Curve = resCurves3[i];
                    for(int j = 0; j < Curve.Count; j++)
                    {
                        if (Curve[j][1] > maxR3) maxR3 = Curve[j][1];
                        if (Curve[j][1] < minR3) minR3 = Curve[j][1];
                    }
                }
                //===========
                if (Regime == 0) { maxY = maxR1; minY = minR1; }
                if (Regime == 1) { maxY = maxR2; minY = minR2; }
                if (Regime == 2) { maxY = maxR3; minY = minR3; }

                maxY += 4;
                minY -= 4;

                zoomY = (this.panel1.Size.Height) / ((Math.Abs(maxY) - minY));

                if (minX < 0) xsign_off = minX;
                else xsign_off = 0;

                if (minY < 0) ysign_off = minY;
                else ysign_off = 0;

                // введем смещения:
                double xoff = xsign_off;
                double yoff = ysign_off;

                System.Drawing.Pen myPen;
                myPen = new System.Drawing.Pen(System.Drawing.Color.LightGray, 1);
                System.Drawing.Graphics panelGraphics = this.panel1.CreateGraphics();

                int Height = this.panel1.Size.Height;

                // выведем оси:
                double xdiv = (maxX - minX) / Convert.ToDouble(10);

                for (int i = 0; i < 10; ++i)
                {
                    int x_cur = Convert.ToInt32(zoomX * (minX + i * xdiv - xoff));
                    panelGraphics.DrawLine(myPen, x_cur, Height - Convert.ToInt32(zoomY * (minY - yoff)), x_cur, Height - Convert.ToInt32(zoomY * (maxY - yoff)));
                }

                double ydiv = (maxY - minY) / Convert.ToDouble(10);

                for (int i = 0; i < 10; ++i)
                {
                    int y_cur = Convert.ToInt32(Height - zoomY * (minY + i * ydiv - yoff));
                    panelGraphics.DrawLine(myPen, Convert.ToInt32(zoomX * (minX - xoff)), y_cur, Convert.ToInt32(zoomX * (maxX - xoff)), y_cur);
                }


                //====== Отрисовка кривых по режимам
                if (Regime == 0)
                {
                    myPen.Color = Color.Red;
                    myPen.Width = 3;

                    for (int i = 0; i < Curves.Count; ++i)
                    {
                        List<List<double>> Curve = Curves[i];
                        if (i == 1) myPen.Color = Color.Orange; 
                        if (i == 2) myPen.Color = Color.Yellow;
                        if (i == 3) myPen.Color = Color.Green; 
                        if (i == 4) myPen.Color = Color.Blue; 
                        if (i >= 5) myPen.Color = Color.Black;
                        for (int j = 0; j < (Curve.Count - 1); ++j)
                        {
                            panelGraphics.DrawLine(myPen,
                                Convert.ToInt32(zoomX * (Curve[j][0] - xoff)),
                                Height - Convert.ToInt32(zoomY * (Curve[j][1] - yoff)),
                                Convert.ToInt32(zoomX * (Curve[j + 1][0] - xoff)),
                                Height - Convert.ToInt32(zoomY * (Curve[j + 1][1] - yoff)));
                        }
                    }
                }

                if (Regime == 1)
                {
                    myPen.Color = Color.Red;
                    myPen.Width = 3;

                    for (int j = 0; j < (resCurve2.Count - 1); ++j)
                    {
                        panelGraphics.DrawLine(myPen,
                             Convert.ToInt32(zoomX * (resCurve2[j][0] - xoff)),
                             Height - Convert.ToInt32(zoomY * (resCurve2[j][1] - yoff)),
                             Convert.ToInt32(zoomX * (resCurve2[j + 1][0] - xoff)),
                             Height - Convert.ToInt32(zoomY * (resCurve2[j + 1][1] - yoff)));
                    }
                }

                if(Regime == 2 && Curves.Count > 1)
                {
                    myPen.Color = Color.Red;
                    myPen.Width = 3;

                    for(int i = 0; i < resCurves3.Count; i++)
                    {
                        if (i == 1) myPen.Color = Color.Orange;
                        if (i == 2) myPen.Color = Color.Yellow;
                        if (i == 3) myPen.Color = Color.Green;
                        if (i == 4) myPen.Color = Color.Blue;
                        if (i >= 5) myPen.Color = Color.Black;

                        var Curve = resCurves3[i];
                        for(int j = 0; j < (Curve.Count - 1); j++)
                        {
                            panelGraphics.DrawLine(myPen,
                                Convert.ToInt32(zoomX * (Curve[j][0] - xoff)),
                                Height - Convert.ToInt32(zoomY * (Curve[j][1] - yoff)),
                                Convert.ToInt32(zoomX * (Curve[j + 1][0] - xoff)),
                                Height - Convert.ToInt32(zoomY * (Curve[j + 1][1] - yoff)));
                        }
                    }
                }

            }

            panel2.Refresh();
            panel3.Refresh();
        }

        private List<List<double>> List<T1>()
        {
            throw new NotImplementedException();
        }

        private void panel2_Paint(object sender, PaintEventArgs e)
        {
            // подписи осей по х

            if (Curves.Count > 0)
            {
                double xoff = Math.Abs(-minX + minX) + xsign_off;

                // выведем оси:
                double xdiv = (maxX - minX) / Convert.ToDouble(10);

                int axeW = panel2.Size.Width;
                int axeH = panel2.Size.Height;

                int w10 = axeW / 10;

                for (int i = 0; i < 10; ++i)
                {
                    double x_cur = minX + i * xdiv;

                    string text = x_cur.ToString();

                    if (text.Length > 5)
                        text = text.Substring(0, 5);

                    using (Font font1 = new Font("Arial", 8, FontStyle.Bold, GraphicsUnit.Point))
                    {
                        RectangleF rectF1 = new RectangleF(i * w10, 2, w10, axeH - 2);
                        e.Graphics.DrawString(text, font1, Brushes.Black, rectF1);
                        e.Graphics.DrawRectangle(Pens.White, Rectangle.Round(rectF1));
                    }
                }
            }
        }

        private void panel3_Paint(object sender, PaintEventArgs e)
        {
            // подписи осей по y

            if (Curves.Count > 0)
            {
                double yoff = Math.Abs(-minY + minY) + ysign_off;

                int Height = this.panel1.Size.Height;

                // выведем оси:
                double ydiv = Math.Abs(maxY - minY) / Convert.ToDouble(10);

                int axeW = panel3.Size.Width;
                int axeH = panel3.Size.Height;

                int h10 = axeH / 10;

                for (int i = 0; i < 10; ++i)
                {
                    double y_cur = maxY - (minY + i * ydiv) + yoff;

                    string text = y_cur.ToString();

                    if (text.Length > 4)
                        text = text.Substring(0, 4);

                    using (Font font1 = new Font("Arial", 8, FontStyle.Bold, GraphicsUnit.Point))
                    {
                        RectangleF rectF1 = new RectangleF(2, i * h10, axeW - 2, h10);
                        e.Graphics.DrawString(text, font1, Brushes.Black, rectF1);
                        e.Graphics.DrawRectangle(Pens.White, Rectangle.Round(rectF1));
                    }
                }
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            Regime = comboBox1.SelectedIndex;
            this.panel1.Invalidate();
        }

        private void panel1_MouseMove(object sender, MouseEventArgs e)
        {
            var Point = panel1.PointToClient(MousePosition);
            double X = Convert.ToDouble(Point.X);
            double Y = Convert.ToDouble(Point.Y);
            int lenX = 3;
            int lenY = 3;
            if(Curves.Count > 0)
            {
                X = X / zoomX + xsign_off;
                Y = (this.panel1.Size.Height - Y) / zoomY + ysign_off;

                double xdiv = (maxX - minX) / Convert.ToDouble(10);
                lenX = (xdiv.ToString()).Length + 1;
                double ydiv = (maxY - minY) / Convert.ToDouble(10);
                lenY = (ydiv.ToString()).Length + 1;
            }
            X = Math.Round(X, lenX);
            Y = Math.Round(Y, lenY);
            var _X = Convert.ToSingle(X).ToString();
            var _Y = Convert.ToSingle(Y).ToString();
            if (_X.Length > lenX) _X = _X.Substring(0, lenX);
            if (_Y.Length > lenY) _Y = _Y.Substring(0, lenY);
            label1.Text = _X;
            label2.Text = _Y;
        }
    }
}
