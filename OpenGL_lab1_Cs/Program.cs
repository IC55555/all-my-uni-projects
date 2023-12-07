using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using OpenTK;
using OpenTK.Graphics.OpenGL;


namespace OpenGL_lab1_Cs
{

    static class constant
    {
        public const double RED = 1;
        public const double GREEN = 2;
        public const double BLUE = 3;
        public const double ORANGE = 4;

    }



    static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        /// 
        //int Width = 1080, Height = 720;
        //byte ColorR = 255, ColorG = 255, ColorB = 255;

        //float red = 1.0f, blue = 0.5f, green = 0.5f;

        //int cur_num = -1;


        //struct type_point
        //{
        //    GLint x, y;
        //    type_point(GLint _x, GLint _y) { x = _x; y = _y; }
        //};
        ////vector <type_point> Points;
        //vector<vector<type_point>> Points;

        //struct color
        //{
        //    GLubyte R = 255, G = 255, B = 255;
        //};

        //vector<color> Colors;

        ///* Функция вывода на экран */
        //void Display(void)
        //{
        //    glClearColor(0.5, 0.5, 0.5, 1);
        //    glClear(GL_COLOR_BUFFER_BIT);
        //    //glEnable(GL_POINT_SMOOTH);
        //    glPointSize(6);

        //    for (int j = 0; j < Points.size(); j++)
        //    {
        //        glColor3ub(Colors[j].R, Colors[j].G, Colors[j].B);
        //        if (j == cur_num)
        //        {
        //            glLineWidth(8);

        //        }
        //        else
        //        {
        //            glLineWidth(3);
        //        }
        //        glBegin(GL_LINE_LOOP);

        //        for (int i = 0; i < Points[j].size(); i++)
        //            glVertex2i(Points[j][i].x, Points[j][i].y);
        //        glEnd();
        //    }
        //    glFinish();
        //}
        ///* Функция изменения размеров окна */
        //void Reshape(GLint w, GLint h)
        //{
        //    Width = w; Height = h;
        //    glViewport(0, 0, w, h);
        //    glMatrixMode(GL_PROJECTION);
        //    glLoadIdentity();
        //    gluOrtho2D(0, w, 0, h);
        //    glMatrixMode(GL_MODELVIEW);
        //    glLoadIdentity();
        //}
        ///* Функция обработки сообщений от клавиатуры */
        //void Keyboard(unsigned char key, int x, int y)
        //{
        //    if (cur_num == -1)
        //        return;
        //    int i, n = Points[cur_num].size();
        //    /* Изменение RGB-компонент цвета точек */
        //    if (key == 'r') Colors[cur_num].R += 5;
        //    if (key == 'g') Colors[cur_num].G += 5;
        //    if (key == 'b') Colors[cur_num].B += 5;
        //    /* Изменение XY-кординат точек */
        //    if (key == 'w') for (i = 0; i < n; i++) Points[cur_num][i].y += 9;
        //    if (key == 's') for (i = 0; i < n; i++) Points[cur_num][i].y -= 9;
        //    if (key == 'a') for (i = 0; i < n; i++) Points[cur_num][i].x -= 9;
        //    if (key == 'd') for (i = 0; i < n; i++) Points[cur_num][i].x += 9;

        //    /* удаление последней точки по нажатию backspace */
        //    if (key == 0x08)
        //        if (Points[cur_num].size())
        //            Points[cur_num].pop_back();
        //    //пробел - создание новой фигуры
        //    if (key == 0x20)
        //    {
        //        if (Points.size())
        //        {
        //            vector<type_point> temp;
        //            color temp_color;
        //            temp_color.R = ColorR;
        //            temp_color.G = ColorG;
        //            temp_color.B = ColorB;
        //            Points.push_back(temp);
        //            Colors.push_back(temp_color);
        //            cur_num++;
        //        }

        //    }
        //    //q
        //    //стрелка влево - переход к предыдущей фигуре
        //    if ((key == 'q') || (key == 'Q'))
        //    {
        //        if (cur_num > 0)
        //            cur_num--;
        //    }
        //    //e
        //    //стрелка вправо - переход к следующей фигуре
        //    if ((key == 'e') || (key == 'E'))
        //    {
        //        if (cur_num < Points.size() - 1)
        //            cur_num++;

        //    }
        //    //x
        //    //Delete - удаление текущей всей фигуры 
        //    if ((key == 'x') || (key == 'X'))
        //    {
        //        if (Points.size())
        //        {
        //            Colors.erase(Colors.begin() + cur_num);
        //            Points.erase(Points.begin() + cur_num);
        //            cur_num--;

        //        }
        //    }
        //    glutPostRedisplay();
        //}
        ///* Функция обработки сообщения от мыши */
        //void Mouse(int button, int state, int x, int y)
        //{
        //    /* клавиша была нажата, но не отпущена */
        //    if (state != GLUT_DOWN) return;
        //    /* новая точка по левому клику */
        //    if (button == GLUT_LEFT_BUTTON)
        //    {
        //        if (cur_num == -1)
        //        {
        //            vector<type_point> temp;
        //            color temp_color;
        //            temp_color.R = ColorR;
        //            temp_color.G = ColorG;
        //            temp_color.B = ColorB;
        //            Points.push_back(temp);
        //            Colors.push_back(temp_color);
        //            cur_num++;
        //        }
        //        type_point p(x, Height -y);
        //        Points[cur_num].push_back(p);
        //    }
        //    glutPostRedisplay();
        //}


        //void processMenuEvents(int option)
        //{
        //    if (cur_num == -1)
        //    {
        //        switch (option)
        //        {
        //            case RED:
        //                ColorR = 255;
        //                ColorG = 0;
        //                ColorB = 0;
        //                break;
        //            case GREEN:
        //                ColorR = 0;
        //                ColorG = 255;
        //                ColorB = 0;
        //                break;
        //            case BLUE:
        //                ColorR = 0;
        //                ColorG = 0;
        //                ColorB = 255;
        //                break;
        //            case ORANGE:
        //                ColorR = 255;
        //                ColorG = 128;
        //                ColorB = 128;

        //                break;
        //        }
        //    }
        //    else
        //        switch (option)
        //        {
        //            case RED:
        //                /* ColorR = 255;
        //                 ColorG = 0;
        //                 ColorB = 0; */
        //                Colors[cur_num].R = 255;
        //                Colors[cur_num].G = 0;
        //                Colors[cur_num].B = 0;
        //                break;
        //            case GREEN:
        //                /*ColorR = 0;
        //                ColorG = 255;
        //                ColorB = 0;*/
        //                Colors[cur_num].R = 0;
        //                Colors[cur_num].G = 255;
        //                Colors[cur_num].B = 0;
        //                break;
        //            case BLUE:
        //                /* ColorR = 0;
        //                 ColorG = 0;
        //                 ColorB = 255; */
        //                Colors[cur_num].R = 0;
        //                Colors[cur_num].G = 0;
        //                Colors[cur_num].B = 255;
        //                break;
        //            case ORANGE:
        //                /* ColorR = 255;
        //                 ColorG = 128;
        //                 ColorB = 128; */
        //                Colors[cur_num].R = 255;
        //                Colors[cur_num].G = 128;
        //                Colors[cur_num].B = 128;
        //                break;
        //        }
        //}

        //void createGLUTMenus()
        //{
        //    int menu;
        //    // создать меню
        //    // сообщить GLUT что "processMenuEvents" будет
        //    // обрабатывать события
        //    menu = glutCreateMenu(processMenuEvents);
        //    //добавить записи в нашем меню
        //    glutAddMenuEntry("Red", RED);
        //    glutAddMenuEntry("Blue", BLUE);
        //    glutAddMenuEntry("Green", GREEN);
        //    glutAddMenuEntry("Orange", ORANGE);
        //    // прикрепить меню правой кнопки
        //    glutAttachMenu(GLUT_RIGHT_BUTTON);
        //}

        ///* Головная программа */
        //void main(int argc, char* argv[])
        //{

        //    glutInit(&argc, argv);
        //    glutInitDisplayMode(GLUT_RGB);
        //    glutInitWindowSize(Width, Height);
        //    glutCreateWindow("Рисуем точки");
        //    glutDisplayFunc(Display);
        //    glutReshapeFunc(Reshape);
        //    glutKeyboardFunc(Keyboard);
        //    glutMouseFunc(Mouse);
        //    createGLUTMenus();
        //    glutMainLoop();
        //}




        [STAThread]
        static void Main()
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}
