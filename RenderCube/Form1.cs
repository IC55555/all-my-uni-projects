using System.Data;
using System.Text;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using static Microsoft.DirectX.Matrix;


namespace RenderCube
{

    public partial class Form1 : Form
    {
        private Device device = null;
        //private VertexBuffer vb = null;
        private Texture texture = null;
        private VertexBuffer[] vb = new VertexBuffer[6];

        public Form1()
        {
            InitializeComponent();
            InitializeGraphics();
            //Устраняем мерцание окна при бесконечной перерисовке
            this.SetStyle(ControlStyles.AllPaintingInWmPaint |
                ControlStyles.Opaque, true);
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
            device = new Device(0, DeviceType.Hardware, this,
                CreateFlags.SoftwareVertexProcessing, presentParams);

            // Создать вершинный буфер
            for (int i = 0; i < 6; i++) 
            {
                vb[i] = new VertexBuffer(typeof(CustomVertex.PositionTextured),
                                          6,
                                          device,
                                          Usage.Dynamic | Usage.WriteOnly,
                                          CustomVertex.PositionTextured.Format,
                                          Pool.Default);

                // Регистрация события Created вершинного буфера
                vb[i].Created += new EventHandler(vb_Created);

                // Принудительный вызов создания треугольника 
                // и заполнения вершинного буфера при первом 
                // создании формы
                vb_Created(vb[i], null);
            }
            
        }

        //----------------------------------------------------

        void vb_Created(object sender, EventArgs e)
        {
            // Определить внутреннюю ссылку на вершинный буфер
            VertexBuffer buffer = (VertexBuffer)sender; // Явное приведение типов

            // Создать локальный массив структур непреобразованных координат
            CustomVertex.PositionTextured[] verts = new CustomVertex.PositionTextured[36];

            // Задать параметры аппроксимирующих треугольников
            verts[0] = new CustomVertex.PositionTextured(-1.0F, 1.0F, 1.0F, 0.0F, 0.0F);
            verts[1] = new CustomVertex.PositionTextured(-1.0F, -1.0F, 1.0F, 0.0F, 1.0F);
            verts[2] = new CustomVertex.PositionTextured(1.0F, 1.0F, 1.0F, 1.0F, 0.0F);
            verts[3] = new CustomVertex.PositionTextured(-1.0F, -1.0F, 1.0F, 0.0F, 1.0F);
            verts[4] = new CustomVertex.PositionTextured(1.0F, -1.0F, 1.0F, 1.0F, 1.0F);
            verts[5] = new CustomVertex.PositionTextured(1.0F, 1.0F, 1.0F, 1.0F, 0.0F);

            verts[6] = new CustomVertex.PositionTextured(-1.0F, 1.0F, -1.0F, 0.0F, 0.0F);
            verts[7] = new CustomVertex.PositionTextured(1.0F, 1.0F, -1.0F, 1.0F, 0.0F);
            verts[8] = new CustomVertex.PositionTextured(-1.0F, -1.0F, -1.0F, 0.0F, 1.0F);
            verts[9] = new CustomVertex.PositionTextured(-1.0F, -1.0F, -1.0F, 0.0F, 1.0F);
            verts[10] = new CustomVertex.PositionTextured(1.0F, 1.0F, -1.0F, 1.0F, 0.0F);
            verts[11] = new CustomVertex.PositionTextured(1.0F, -1.0F, -1.0F, 1.0F, 1.0F);

            verts[12] = new CustomVertex.PositionTextured(-1.0F, 1.0F, 1.0F, 0.0F, 0.0F);
            verts[13] = new CustomVertex.PositionTextured(1.0F, 1.0F, -1.0F, 1.0F, 1.0F);
            verts[14] = new CustomVertex.PositionTextured(-1.0F, 1.0F, -1.0F, 0.0F, 1.0F);
            verts[15] = new CustomVertex.PositionTextured(-1.0F, 1.0F, 1.0F, 0.0F, 0.0F);
            verts[16] = new CustomVertex.PositionTextured(1.0F, 1.0F, 1.0F, 1.0F, 0.0F);
            verts[17] = new CustomVertex.PositionTextured(1.0F, 1.0F, -1.0F, 1.0F, 1.0F);

            verts[18] = new CustomVertex.PositionTextured(-1.0F, -1.0F, 1.0F, 0.0F, 0.0F);
            verts[19] = new CustomVertex.PositionTextured(-1.0F, -1.0F, -1.0F, 0.0F, 1.0F);
            verts[20] = new CustomVertex.PositionTextured(1.0F, -1.0F, -1.0F, 1.0F, 1.0F);
            verts[21] = new CustomVertex.PositionTextured(-1.0F, -1.0F, 1.0F, 0.0F, 0.0F);
            verts[22] = new CustomVertex.PositionTextured(1.0F, -1.0F, -1.0F, 1.0F, 1.0F);
            verts[23] = new CustomVertex.PositionTextured(1.0F, -1.0F, 1.0F, 1.0F, 0.0F);

            verts[24] = new CustomVertex.PositionTextured(-1.0F, 1.0F, 1.0F, 0.0F, 0.0F);
            verts[25] = new CustomVertex.PositionTextured(-1.0F, -1.0F, -1.0F, 1.0F, 1.0F);
            verts[26] = new CustomVertex.PositionTextured(-1.0F, -1.0F, 1.0F, 1.0F, 0.0F);
            verts[27] = new CustomVertex.PositionTextured(-1.0F, 1.0F, -1.0F, 0.0F, 1.0F);
            verts[28] = new CustomVertex.PositionTextured(-1.0F, -1.0F, -1.0F, 1.0F, 1.0F);
            verts[29] = new CustomVertex.PositionTextured(-1.0F, 1.0F, 1.0F, 0.0F, 0.0F);

            verts[30] = new CustomVertex.PositionTextured(1.0F, 1.0F, 1.0F, 0.0F, 0.0F);
            verts[31] = new CustomVertex.PositionTextured(1.0F, -1.0F, 1.0F, 1.0F, 0.0F);
            verts[32] = new CustomVertex.PositionTextured(1.0F, -1.0F, -1.0F, 1.0F, 1.0F);
            verts[33] = new CustomVertex.PositionTextured(1.0F, 1.0F, -1.0F, 0.0F, 1.0F);
            verts[34] = new CustomVertex.PositionTextured(1.0F, 1.0F, 1.0F, 0.0F, 0.0F);
            verts[35] = new CustomVertex.PositionTextured(1.0F, -1.0F, -1.0F, 1.0F, 1.0F);

            // Заполнить вершинный буфер данными треугольников
            buffer.SetData(verts, 0, LockFlags.None);

            // Создать объект текстуры
            Bitmap bmp = new Bitmap(this.GetType(), "bp0.bmp");
            texture = new Texture(device, bmp, 0, Pool.Managed);
        }


        protected override void OnPaint(PaintEventArgs e)
        {
            // Очистить цветом клиентскую область формы
            device.Clear(ClearFlags.Target, System.Drawing.Color.CornflowerBlue, 1.0F, 0);

            // Вызов нашей функции установки камеры
            SetupCamera();

            // Сформировать сцену с учетом параметров 
            // "положение-нормаль-цвет"
            device.BeginScene();
            // Установить формат обработки вершин при отображении
            device.VertexFormat = CustomVertex.PositionTextured.Format;
            // Нарисовать треугольник по данным из буфера
            for (int i = 0; i < 6; i++)
            {
                device.SetStreamSource(i, vb[i], 0);
                // Установка текстуры
                device.SetTexture(0, texture);
            }
            // Преобразование пространства для произвольных поворотов
            device.Transform.World = Matrix.RotationYawPitchRoll(
                angle / (float)Math.PI,
                angle / (float)Math.PI * 2.0F,
                angle / (float)Math.PI);

            device.DrawPrimitives(PrimitiveType.TriangleList, 0, 12);
            device.EndScene();

            // Показать буфер кадра 
            device.Present();

            // Принудительно перерисовать
            this.Invalidate();
        }

        private float angle = 0;// Закрытый член класса

        // Установка камеры в сцену
        private void SetupCamera()
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

            // Освещение
            device.RenderState.Lighting = false;

            device.Transform.World = Matrix.RotationYawPitchRoll(
                angle / (float)Math.PI,
                angle / (float)Math.PI / 2,
                angle / (float)Math.PI);

            angle += 0.1F;
        }
    }
}