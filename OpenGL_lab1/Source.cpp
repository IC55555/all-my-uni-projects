//Цвета
#define RED 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4

//Палитра
#define PRED 1
#define PGREEN 2
#define PBLUE 3

//Передвижение
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//Удаление
#define DELETE 1
//#define DALL 2

//Выбор примитива
#define NEXT 1 
#define PRIV 2

//Отмена предыдущего действия
#define BACKWARD 1

//Закончить примитив
#define NEW 1

#include "glut.h"
#include <vector>
#include <iostream>
#include <windows.h>

using namespace std;

GLint Width = 512, Height = 512;

GLubyte ColorR = 255, ColorG = 255, ColorB = 255;

float red = 1.0f, blue = 0.5f, green = 0.5f;

int cur_num = -1;

struct type_point
{
	GLint x, y;
	type_point(GLint _x, GLint _y) { x = _x; y = _y; }
};

//vector <type_point> Points;

vector<vector<type_point>> Points;

//vector<line> v;
//
//line l;

int group;

struct color
{
	GLubyte R = 255, G = 255, B = 255;
};

vector<color> Colors;

vector<type_point> temp;

/* Функция вывода на экран */
void Display(void)
{
	glClearColor(0.5, 0.5, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	//glEnable(GL_POINT_SMOOTH);
	glPointSize(6);

	for (int j = 0; j < Points.size(); j++)
	{
		glColor3ub(Colors[j].R, Colors[j].G, Colors[j].B);
		if (j == cur_num)
		{
			glLineWidth(8);
		}
		else
		{
			glLineWidth(3);
		}
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < Points[j].size(); i++)
			glVertex2i(Points[j][i].x, Points[j][i].y);
		glEnd();
	}
	glFinish();
}

void MoveUp()
{
	int y;
	int i, n = Points[cur_num].size();
	for (i = 0; i < n; i++) Points[cur_num][i].y += 9;

}

void MoveDown()
{
	int y;
	int i, n = Points[cur_num].size();
	for (i = 0; i < n; i++) Points[cur_num][i].y -= 9;

}

void MoveLeft()
{
	int x;
	int i, n = Points[cur_num].size();
	for (i = 0; i < n; i++) Points[cur_num][i].x -= 9;

}

void MoveRight()
{
	int x;
	int i, n = Points[cur_num].size();
	for (i = 0; i < n; i++) Points[cur_num][i].x += 9;

}

/* Функция изменения размеров окна */
void Reshape(GLint w, GLint h)
{
	Width = w; Height = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void New()
{
	if (Points.size())
	{
		vector<type_point> temp;
		color temp_color;
		temp_color.R = ColorR;
		temp_color.G = ColorG;
		temp_color.B = ColorB;
		Points.push_back(temp);
		Colors.push_back(temp_color);
		cur_num++;
	}
}

void delete_all()
{
	//for (int i = 0; i < Points.size(); i++)
	//{
	//	if (Points.size())
	//	{
	//		Colors.erase(Colors.begin() + i);
	//		Points.erase(Points.begin() + i);
	//		i--;
	//	}
	//}


	//while (Points.size() > 0)
	//{

	//	Colors.erase(Colors.begin());
	//	Points.erase(Points.begin());
	//	
	//}


	////удалить информацию о цвете текущего набора
	//Colors.erase(Colors.begin() + cur_num);
	//
	//New();

	//glutSetWindowTitle;
}

void Delete()
{
	if (Points.size())
	{
		Colors.erase(Colors.begin() + cur_num);
		Points.erase(Points.begin() + cur_num);
		cur_num--;
	}
}

void Next_prim()
{
	if (cur_num < Points.size() - 1)
		cur_num++;
}

void Previous_prim()
{
	if (cur_num > 0)
		cur_num--;

}

void BackWard()
{
	if (Points[cur_num].size())
		Points[cur_num].pop_back();
}

void Red()
{
	Colors[cur_num].R += 5;
}

void Green()
{
	Colors[cur_num].G += 5;
}

void Blue()
{
	Colors[cur_num].B += 5;
}

void processGroup(int option)
{
	switch (option)
	{
	case NEXT: Next_prim(); break;
	case PRIV: Previous_prim(); break;
	}
	glutPostRedisplay();
}

void processDeleteMenu(int option)
{
	switch (option)
	{
	case DELETE: Delete(); break;
	case BACKWARD: BackWard(); break;
	//case DALL: delete_all(); break;
	}
	glutPostRedisplay();
}

//Передвижение примитива
void proccessMoveMenu(int option)
{
	switch (option)
	{
	case UP: MoveUp(); break;
	case DOWN: MoveDown(); break;
	case LEFT: MoveLeft(); break;
	case RIGHT: MoveRight(); break;
	}
	glutPostRedisplay();
}

//Палитра
void proccessRGB(int option)
{
	switch (option)
	{
	case PRED: Red(); break;
	case PGREEN: Green(); break;
	case PBLUE: Blue(); break;
	}
	glutPostRedisplay();
}

void processColorMenu(int option)
{
	if (cur_num == -1)
	{
		switch (option)
		{
		case RED:
			ColorR = 255;
			ColorG = 0;
			ColorB = 0;
			break;
		case GREEN:
			ColorR = 0;
			ColorG = 255;
			ColorB = 0;
			break;
		case BLUE:
			ColorR = 0;
			ColorG = 0;
			ColorB = 255;
			break;
		case ORANGE:
			ColorR = 255;
			ColorG = 128;
			ColorB = 128;
			break;
		}
	}
	else
		switch (option)
		{
		case RED:
			/* ColorR = 255;
			ColorG = 0;
			ColorB = 0; */
			Colors[cur_num].R = 255;
			Colors[cur_num].G = 0;
			Colors[cur_num].B = 0;
			break;
		case GREEN:
			/*ColorR = 0;
			ColorG = 255;
			ColorB = 0;*/
			Colors[cur_num].R = 0;
			Colors[cur_num].G = 255;
			Colors[cur_num].B = 0;
			break;
		case BLUE:
			/* ColorR = 0;
			ColorG = 0;
			ColorB = 255; */
			Colors[cur_num].R = 0;
			Colors[cur_num].G = 0;
			Colors[cur_num].B = 255;
			break;
		case ORANGE:
			/* ColorR = 255;
			ColorG = 128;
			ColorB = 128; */
			Colors[cur_num].R = 255;
			Colors[cur_num].G = 128;
			Colors[cur_num].B = 128;

			break;
		}
}

void proccessMenu(int option)
{
	switch (option)
	{
	case NEW: New(); break;
	}
	glutPostRedisplay();
}

/* Функция обработки сообщений от клавиатуры */
void Keyboard(unsigned char key, int x, int y)
{
	if (cur_num == -1)
		return;
	int i, n = Points[cur_num].size();
	/* Изменение RGB-компонент цвета точек */
	if (key == 'r' || key == 'R') Red();
	if (key == 'g' || key == 'G') Green();
	if (key == 'b' || key == 'B') Blue();

	/* Изменение XY-кординат точек */
	if (key == 'w' || key == 'W') { MoveUp(); }
	if (key == 's' || key == 'S') { MoveDown(); }
	if (key == 'a' || key == 'A') { MoveLeft(); }
	if (key == 'd' || key == 'D') { MoveRight(); }

	/* удаление последней точки по нажатию backspace */
	if (key == 0x08) { BackWard(); }

	//пробел - создание новой фигуры
	if (key == 0x20) { New(); }

	//q
	//стрелка влево - переход к предыдущей фигуре
	if (key == 'q' || key == 'Q') { Previous_prim(); }

	//e
	//стрелка вправо - переход к следующей фигуре
	if (key == 'e' || key == 'E') { Next_prim(); }

	//x
	//Delete - удаление текущей всей фигуры
	if (key == 'x' || key == 'X') { Delete(); }

	//if (key == 'y' || key == 'Y') { delete_all(); }

	glutPostRedisplay();
}

/* Функция обработки сообщения от мыши */
void Mouse(int button, int state, int x, int y)
{
	/* клавиша была нажата, но не отпущена */
	if (state != GLUT_DOWN) return;
	/* новая точка по левому клику */
	if (button == GLUT_LEFT_BUTTON)
	{
		if (cur_num == -1)
		{
			color temp_color;
			temp_color.R = ColorR;
			temp_color.G = ColorG;
			temp_color.B = ColorB;
			Points.push_back(temp);
			Colors.push_back(temp_color);
			cur_num++;
		}
		type_point p(x, Height - y);
		Points[cur_num].push_back(p);
	}
	glutPostRedisplay();
}

void createGLUTMenus()
{
	int menu;
	int color_menu;
	int move_menu;
	int delete_menu;
	int select_menu;
	int rgb_menu;

	//добавить записи в нашем меню
	color_menu = glutCreateMenu(processColorMenu);
	glutAddMenuEntry("Red", RED);
	glutAddMenuEntry("Blue", BLUE);
	glutAddMenuEntry("Green", GREEN);
	glutAddMenuEntry("Orange", ORANGE);

	move_menu = glutCreateMenu(proccessMoveMenu);
	glutAddMenuEntry("up", UP);
	glutAddMenuEntry("down", DOWN);
	glutAddMenuEntry("left", LEFT);
	glutAddMenuEntry("right", RIGHT);

	select_menu = glutCreateMenu(processGroup);
	glutAddMenuEntry("next", NEXT);
	glutAddMenuEntry("PRIV", PRIV);

	delete_menu = glutCreateMenu(processDeleteMenu);
	glutAddMenuEntry("delete", DELETE);
	glutAddMenuEntry("back", BACKWARD);
	//glutAddMenuEntry("delete all", DALL);

	rgb_menu = glutCreateMenu(proccessRGB);
	glutAddMenuEntry("R", PRED);
	glutAddMenuEntry("G", PGREEN);
	glutAddMenuEntry("B", PBLUE);

	menu = glutCreateMenu(proccessMenu);
	glutAddSubMenu("move", move_menu);
	glutAddMenuEntry("new", NEW);
	glutAddSubMenu("select", select_menu);
	glutAddSubMenu("color", color_menu);
	glutAddSubMenu("rgb", rgb_menu);
	glutAddSubMenu("delete", delete_menu);

	// прикрепить меню правой кнопки
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* Головная программа */
void main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Рисуем точки");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	createGLUTMenus();
	glutMainLoop();
}