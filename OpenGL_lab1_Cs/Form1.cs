using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using OpenTK;
using OpenTK.Mathematics;
using OpenTK.Graphics.OpenGL;
using OpenTK.Graphics;
using OpenTK.Platform;
using OpenTK.Windowing.Common;
using OpenTK.Windowing.Desktop;
using OpenTK.Windowing.GraphicsLibraryFramework;
using NativeWindow = OpenTK.Windowing.Desktop.NativeWindow;
using WindowsFormsIntegration;

namespace OpenGL_lab1_Cs
{
    public partial class Form1 : Form
    {
        //OpenTK.GLControl glcontrol1;
        //GLControl glControlCubeRot;
        test = new Control();
        double crds = 45, edgeLen = 5;
        public Form1()
        {
            InitializeComponent();
        }
        private void buttonClose_Click(object sender, EventArgs e)
        {
            Close();
        }
        private void clear()
        {
            GL.ClearColor(Color.Beige);
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
        }
        private void coords()
        {
            GL.LineWidth(1);
            GL.Begin(PrimitiveType.Lines);
            GL.Color3(Color.Red);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(crds, 0, 0);
            GL.Color3(Color.Green);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(0, crds, 0);
            GL.Color3(Color.Blue);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(0, 0, crds);
            GL.End();
        }

        private void cube(double xCube, double yCube)
        {
            double[] vLeftFarBot = { -edgeLen + xCube, -edgeLen + yCube, -edgeLen };
            double[] vLeftNearBot = { -edgeLen + xCube, -edgeLen + yCube, edgeLen };
            double[] vRightFarBot = { edgeLen + xCube, -edgeLen + yCube, -edgeLen };
            double[] vRightNearBot = { edgeLen + xCube, -edgeLen + yCube, edgeLen };
            double[] vLeftFarTop = { -edgeLen + xCube, edgeLen + yCube, -edgeLen };
            double[] vLeftNearTop = { -edgeLen + xCube, edgeLen + yCube, edgeLen };
            double[] vRightFarTop = { edgeLen + xCube, edgeLen + yCube, -edgeLen };
            double[] vRightNearTop = { edgeLen + xCube, edgeLen + yCube, edgeLen };
            GL.LineWidth(2);
            GL.Begin(PrimitiveType.Quads);
            // Bottom
            GL.Color3(Color.DarkGreen);
            GL.Normal3(0.0, -1.0, 0.0);
            GL.Vertex3(vLeftNearBot);
            GL.Vertex3(vLeftFarBot);
            GL.Vertex3(vRightFarBot);
            GL.Vertex3(vRightNearBot);
            // Top
            GL.Normal3(0.0, 1.0, 0.0);
            GL.Color3(Color.DarkMagenta);
            GL.Vertex3(vLeftNearTop);
            GL.Vertex3(vRightNearTop);
            GL.Vertex3(vRightFarTop);
            GL.Vertex3(vLeftFarTop);
            // Front
            GL.Normal3(0.0, 0.0, 1.0);
            GL.Color3(Color.Yellow);
            GL.Vertex3(vLeftNearBot);
            GL.Vertex3(vRightNearBot);
            GL.Vertex3(vRightNearTop);
            GL.Vertex3(vLeftNearTop);
            // Back
            GL.Normal3(0.0, 0.0, -1.0);
            GL.Color3(Color.Blue);
            GL.Vertex3(vLeftFarBot);
            GL.Vertex3(vLeftFarTop);
            GL.Vertex3(vRightFarTop);
            GL.Vertex3(vRightFarBot);
            // Left
            GL.Normal3(-1.0, 0.0, 0.0);
            GL.Color3(Color.Red);
            GL.Vertex3(vLeftFarBot);
            GL.Vertex3(vLeftNearBot);
            GL.Vertex3(vLeftNearTop);
            GL.Vertex3(vLeftFarTop);
            // Right
            GL.Normal3(1.0, 0.0, 0.0);
            GL.Color3(Color.Brown);
            GL.Vertex3(vRightNearBot);
            GL.Vertex3(vRightFarBot);
            GL.Vertex3(vRightFarTop);
            GL.Vertex3(vRightNearTop);
            GL.End();
        }

        private void glControlCubeRot_Load(object sender, EventArgs e)
        {
            int Width = 100; //glControlCubeRot.Width;
            int Height = 100; // glControlCubeRot.Height;
            GL.Viewport(0, 0, Width, Height);
            //
            GL.MatrixMode(MatrixMode.Projection);
            GL.LoadIdentity();
            GL.Ortho(-crds, crds, -crds, crds, -crds, crds);
            GL.Rotate(15, new Vector3d(0, 1, 0));
            GL.Rotate(-55, new Vector3d(1, 0, 0));
            GL.Translate(-10, 0, 0);
            //
            GL.MatrixMode(MatrixMode.Modelview);
            GL.LoadIdentity();
            //
            GL.PolygonMode(MaterialFace.Front, PolygonMode.Fill);
            GL.PolygonMode(MaterialFace.Back, PolygonMode.Point);
            GL.PointSize(8);
            GL.Enable(EnableCap.DepthTest);
            //
            glControlCubeRot.Invalidate();
        }

        private void glControlCubeRot_Paint(object sender, PaintEventArgs e)
        {
            clear();
            coords();
            cube(0, 0);
            glControlCubeRot.SwapBuffers();
        }

        private void buttonRotX_Click(object sender, EventArgs e)
        {
            GL.Rotate(3, 1, 0, 0);
            glControlCubeRot.Invalidate();
        }
        private void buttonRotY_Click(object sender, EventArgs e)
        {
            GL.Rotate(3, 0, 1, 0);
            glControlCubeRot.Invalidate();
        }
        private void buttonRotZ_Click(object sender, EventArgs e)
        {
            GL.Rotate(3, 0, 0, 1);
            glControlCubeRot.Invalidate();
        }

        private void button1_Click(object sender, EventArgs e)
        {
             
        }
    }
}
