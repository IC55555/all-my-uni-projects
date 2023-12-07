using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using cube;
//using Microsoft.Windows.Compatibility;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using static Microsoft.DirectX.Matrix;

namespace cube
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

        }

        public void InitializeGraphics()
        {
            // Создание объекта и настройка параметров представления
            // Создать объект параметров представления
            PresentParameters presentParams = new PresentParameters();
            // Установить оконный режим
            presentParams.Windowed = true;
            // Сбрасывать содержимое буфера, если он не готов к представлению
            presentParams.SwapEffect = SwapEffect.Discard;
            // Создать объект устройства и сохранить ссылку на него
            Device device = new Device(0, DeviceType.Hardware, this, CreateFlags.SoftwareVertexProcessing, presentParams);
        }

        public static Microsoft.DirectX.Matrix.LookAtLH(
  Microsoft.DirectX.Vector3 cameraPosition, // Положение камеры
  Microsoft.DirectX.Vector3 cameraTarget,   // Положение объекта
  Microsoft.DirectX.Vector3 cameraUpVector) // Направление камеры

        // Установка камеры в сцену
        public void SetupCamera()
        {
            //Создание перспективы 
            device.Transform.Projection = Matrix.PerspectiveFovLH(
                (float)Math.PI / 4, // Угол зрения равен 45 градусов
                                    // Форматное соотношение сторон
                (float)this.ClientSize.Width / (float)this.ClientSize.Height,
                1.0F,               // Ближний план
                100.0F);            // Дальний план

            //Добавление камеры 
            device.Transform.View = Matrix.LookAtLH(
                new Vector3(0, 0, 5.0F),    // Положение камеры
                new Vector3(),              // Положение объекта текущее
                new Vector3(0, 1, 0));      // Направление камеры

            // Преобразование пространства для поворота треугольника относительно оси Z
            device.Transform.World = Matrix.RotationZ((float)Math.PI / 6.0F);
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            // Определить внутреннюю ссылку на вершинный буфер
            VertexBuffer buffer = (VertexBuffer)sender; // Явное приведение типов

            // Создать локальный массив структур непреобразованных координат
            CustomVertex.PositionColored[] verts = new CustomVertex.PositionColored[36];

            // Задать параметры аппроксимирующих треугольников
            // Порядок обхода вершин для лицевой стороны правый
            // (по часовой стрелке), для невидимой - левый 
            //
            // Задняя грань: треугольник 2-0-1 (невидимая сторона - обход левый)
            verts[0] = new CustomVertex.PositionColored(1.0F, 1.0F, 1.0F, Color.Red.ToArgb());
            verts[1] = new CustomVertex.PositionColored(-1.0F, 1.0F, 1.0F, Color.Red.ToArgb());
            verts[2] = new CustomVertex.PositionColored(-1.0F, -1.0F, 1.0F, Color.Red.ToArgb());
            // Задняя грань: треугольник 2-1-3 (невидимая сторона - обход левый)
            verts[3] = new CustomVertex.PositionColored(1.0F, 1.0F, 1.0F, Color.Red.ToArgb());
            verts[4] = new CustomVertex.PositionColored(-1.0F, -1.0F, 1.0F, Color.Red.ToArgb());
            verts[5] = new CustomVertex.PositionColored(1.0F, -1.0F, 1.0F, Color.Red.ToArgb());
            //
            // Передняя грань: треугольник 5-6-4 (лицевая сторона - обход правый)
            verts[6] = new CustomVertex.PositionColored(1.0F, 1.0F, -1.0F, Color.Blue.ToArgb());
            verts[7] = new CustomVertex.PositionColored(-1.0F, -1.0F, -1.0F, Color.Blue.ToArgb());
            verts[8] = new CustomVertex.PositionColored(-1.0F, 1.0F, -1.0F, Color.Blue.ToArgb());
            // Передняя грань: треугольник 5-7-6 (лицевая сторона - обход правый)
            verts[9] = new CustomVertex.PositionColored(1.0F, 1.0F, -1.0F, Color.Blue.ToArgb());
            verts[10] = new CustomVertex.PositionColored(1.0F, -1.0F, -1.0F, Color.Blue.ToArgb());
            verts[11] = new CustomVertex.PositionColored(-1.0F, -1.0F, -1.0F, Color.Blue.ToArgb());
            //
            // Верхняя грань: треугольник 0-5-4 (лицевая сторона - обход правый)
            verts[12] = new CustomVertex.PositionColored(-1.0F, 1.0F, 1.0F, Color.Yellow.ToArgb());
            verts[13] = new CustomVertex.PositionColored(1.0F, 1.0F, -1.0F, Color.Yellow.ToArgb());
            verts[14] = new CustomVertex.PositionColored(-1.0F, 1.0F, -1.0F, Color.Yellow.ToArgb());
            // Верхняя грань: треугольник 0-2-5 (лицевая сторона - обход правый)
            verts[15] = new CustomVertex.PositionColored(-1.0F, 1.0F, 1.0F, Color.Yellow.ToArgb());
            verts[16] = new CustomVertex.PositionColored(1.0F, 1.0F, 1.0F, Color.Yellow.ToArgb());
            verts[17] = new CustomVertex.PositionColored(1.0F, 1.0F, -1.0F, Color.Yellow.ToArgb());
            //
            // Нижняя грань: треугольник 1-6-7 (невидимая сторона - обход левый)
            verts[18] = new CustomVertex.PositionColored(-1.0F, -1.0F, 1.0F, Color.Black.ToArgb());
            verts[19] = new CustomVertex.PositionColored(-1.0F, -1.0F, -1.0F, Color.Black.ToArgb());
            verts[20] = new CustomVertex.PositionColored(1.0F, -1.0F, -1.0F, Color.Black.ToArgb());
            // Нижняя грань: треугольник 1-7-3 (невидимая сторона - обход левый)
            verts[21] = new CustomVertex.PositionColored(-1.0F, -1.0F, 1.0F, Color.Black.ToArgb());
            verts[22] = new CustomVertex.PositionColored(1.0F, -1.0F, -1.0F, Color.Black.ToArgb());
            verts[23] = new CustomVertex.PositionColored(1.0F, -1.0F, 1.0F, Color.Black.ToArgb());
            //
            // Левая грань: треугольник 0-6-1 (невидимая сторона - обход левый)
            verts[24] = new CustomVertex.PositionColored(-1.0F, 1.0F, 1.0F, Color.Gray.ToArgb());
            verts[25] = new CustomVertex.PositionColored(-1.0F, -1.0F, -1.0F, Color.Gray.ToArgb());
            verts[26] = new CustomVertex.PositionColored(-1.0F, -1.0F, 1.0F, Color.Gray.ToArgb());
            // Левая грань: треугольник 0-4-6 (невидимая сторона - обход левый)
            verts[27] = new CustomVertex.PositionColored(-1.0F, 1.0F, 1.0F, Color.Gray.ToArgb());
            verts[28] = new CustomVertex.PositionColored(-1.0F, 1.0F, -1.0F, Color.Gray.ToArgb());
            verts[29] = new CustomVertex.PositionColored(-1.0F, -1.0F, -1.0F, Color.Gray.ToArgb());
            //
            // Правая грань: треугольник 2-3-7 (лицевая сторона - обход правый)
            verts[30] = new CustomVertex.PositionColored(1.0F, 1.0F, 1.0F, Color.Green.ToArgb());
            verts[31] = new CustomVertex.PositionColored(1.0F, -1.0F, 1.0F, Color.Green.ToArgb());
            verts[32] = new CustomVertex.PositionColored(1.0F, -1.0F, -1.0F, Color.Green.ToArgb());
            // Правая грань: треугольник 2-7-5 (лицевая сторона - обход правый)
            verts[33] = new CustomVertex.PositionColored(1.0F, 1.0F, 1.0F, Color.Green.ToArgb());
            verts[34] = new CustomVertex.PositionColored(1.0F, -1.0F, -1.0F, Color.Green.ToArgb());
            verts[35] = new CustomVertex.PositionColored(1.0F, 1.0F, -1.0F, Color.Green.ToArgb());

            // Заполнить вершинный буфер данными треугольников
            buffer.SetData(verts, 0, LockFlags.None);


            // Создать вершинный буфер
            var vb = new VertexBuffer(typeof(CustomVertex.PositionColored),
                                36,
                                device,
                                Usage.Dynamic | Usage.WriteOnly,
                                CustomVertex.PositionColored.Format,
                                Pool.Default);

        }
    }
}



//public Form1()
//{
//    InitializeComponent();

//}