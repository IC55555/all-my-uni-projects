#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct VarTableElem  //таблица переменных
{
	VarTableElem(string name, string type, bool value) :name(name), value(value) {} //основной констрктор
	VarTableElem() : name(""), value(0) {} // констрктор по умолчанию
	string name; // строка имен
	bool value; // строка значений
};

struct place // координаты выражения(что угодно)
{
	int i;
	int j;
	place(int i, int j) :i(i), j(j) {} //основной констрктор
	place() :i(), j() {}// констрктор по умолчанию
};

struct token // структура для определение вида элемента
{
	int table;
	int i;
	int j;
	token(int table, int i, int j) :table(table), i(i), j(j) {}//основной констрктор
};

class ConstTable //таблица констант
{
public:
	vector<string> array;
	ConstTable(string fileName) //конструктор
	{
		readTable(fileName);
	};
	~ConstTable() // деструктор
	{
		array.~vector();
	}

	int findElem(string Name)
	{
		auto res = find(array.begin(), array.end(), Name);
		if (res == array.end())
			return -1; // если элемент не найден, выводится -1
		else
			return res - array.begin();
	}

	string getElem(int index)
	{
		if (index >= array.size() || index < 0)
			return {};
		else
			return array[index];

	}


private:
	void readTable(string fileName) //считывание в строку из файла
	{
		string tmp;
		fstream in(fileName);
		while (!(in.eof()))
		{
			in >> tmp;
			array.push_back(tmp);
		}
		in.close();
	}
};

class TableVar
{
public:
	vector <vector<VarTableElem>> array; //вектор векторов над полем элементов(таблица)

	TableVar()
	{
		array.resize(52);
	};
	~TableVar()
	{
		array.~vector();
	}

	place findElem(string name) // поиск по указанным координатам выражения
	{
		place pl;
		auto hash = getHash(name);
		pl.i = hash; // Хэш - код первого символа строки, а ищем его по имени строки
		if (array[hash].size())
		{
			auto res = find_if(array[hash].begin(), array[hash].end(), [&](const VarTableElem &s)-> bool {return s.name == name; });
			if (res == array[hash].end())
			{
				array[hash].push_back(VarTableElem(name, "", 0));
				pl.j = array[hash].size() - 1;
			}
			else pl.j = res - array[hash].begin();
		}

		else
		{
			array[hash].push_back(VarTableElem(name, "", 0));
			pl.j = 0;
		}
		return pl;
	}

	void changeElem(place pl, bool value) // присваиваем 1 или 0 элементу, в зависимости от его наличия
	{
		array[pl.i][pl.j].value = value;
	}
	VarTableElem getElem(place pl)
	{
		return array[pl.i][pl.j];
	}

private:
	int getHash(string h)  // получение интовского кода первого символа строки(Хэш)
	{
		char len = 0; //добавили 0 чтоб не ругался на len, может сломаться
		if (h[0] >= 'A' && h[0] <= 'Z')
			len = h[0] - 'A';
		if (h[0] >= 'a' && h[0] <= 'z')
			len = h[0] - 'a' + 26;
		return (int)len;
	}
};

class TableInt //таблица данных типа int
{
public:
	vector<vector<int>> array;

	TableInt()
	{
		array.resize(19);
	};
	~TableInt()
	{
		array.~vector();
	}

	place findElem(string value) // поиск по указанным координатам выражения
	{
		int tmp = stoi(value);
		place pl;
		auto hash = getHash(value);
		pl.i = hash;
		if (array[hash].size())
		{
			auto res = find(array[hash].begin(), array[hash].end(), tmp);
			if (res == array[hash].end())
			{
				array[hash].push_back(tmp);
				pl.j = array[hash].size() - 1;
			}
			else pl.j = res - array[hash].begin();
		}

		else
		{
			array[hash].push_back(tmp);
			pl.j = 0;
		}
		return pl;
	}
	void changeElem(place pl, int value) // присваиваем 1 или 0 элементу, в зависимости от его наличия
	{
		array[pl.i][pl.j] = value;
	}
	int getElem(place pl)
	{
		return array[pl.i][pl.j];
	}

private:
	int getHash(string h) // получение интовского кода первого символа строки(Хэш)
	{
		int len = 0;
		if (h[0] == '-')
			len = (int)h[1] - '0' + 9;
		else len = (int)h[0] - '0';
		return len;
	}
};

class Lexeme
{
public:
	ConstTable *keyWords = new ConstTable("keywords.txt");
	ConstTable *operations = new ConstTable("operations.txt");
	ConstTable *separators = new ConstTable("separator.txt");
	TableInt *tableInt = new TableInt();
	TableVar *tableVar = new TableVar();

	Lexeme() // конструктор по умолчанию(пустой)
	{}

	string str;
	int value = 0;
	VarTableElem var;

	void getElemByToken(token tmp) //сбор выражения по полученным кускам
	{
		switch (tmp.table)
		{
		case 0: str = keyWords->getElem(tmp.i); break;
		case 1: str = operations->getElem(tmp.i); break;
		case 2: str = separators->getElem(tmp.i); break;
		case 3: value = tableInt->getElem(place(tmp.i, tmp.j)); break;
		case 4: var = tableVar->getElem(place(tmp.i, tmp.j)); break;
		}
	}
private:
};

void main()
{
	auto lexeme = new Lexeme();
	int index;
	for (int i = 0; i < lexeme->keyWords->array.size(); i++)
		cout << lexeme->keyWords->array[i] << "  ";

	//           Ищем в таблице 'while':
	/*index = lexeme->keyWords->findElem("while");
	cout << endl << "answer = " << index << endl << endl;*/
	//
	//
	//	//           Ищем в таблице оператор
	//	for (int i = 0; i < lexeme->operations->array.size(); i++)
	//		cout << lexeme->operations->array[i] << endl;
	//	index = lexeme->operations->findElem("=");
	//	cout << "answer = " << index << endl << endl; 
	//
	////	           Проверяем положение разделителя в таблице:
	//	for (int i = 0; i < lexeme->separators->array.size(); i++)
	//		cout << lexeme->separators->array[i] << endl;
	//	index = lexeme->separators->findElem(";");
	//	cout << "answer = " << index << endl << endl;


		//           Проверяем правильность расположения операций в таблице:
		//for (int i = 0; i < lexeme->operations->array.size(); i++)
		//	cout << lexeme->operations->array[i] << endl;
		//index = lexeme->operations->findElem("+");
		//index = lexeme->operations->findElem("-");
		//index = lexeme->operations->findElem(">=");
		//index = lexeme->operations->findElem("<=");
		//index = lexeme->operations->findElem(">");
		//index = lexeme->operations->findElem("<");
		//cout << "answer =" << index << endl << endl;


		//           Ищем в таблице элементы элементы типа int
		//place pl;
		//pl = lexeme->tableInt->findElem("30");
		//pl = lexeme->tableInt->findElem("60");
		//pl = lexeme->tableInt->findElem("10");
		//cout << endl << "answer = " << pl.i << ", " << pl.j << endl << endl;
		//for (int i = 0; i < lexeme->tableInt->array.size(); i++) {
		//	for (int j = 0; j < lexeme->tableInt->array[i].size(); j++)
		//		cout << lexeme->tableInt->array[i][j] << " ";
		//	//cout << endl;
		//}


		//           Ищем элементы в таблице переменных
		place pl;
		pl = lexeme->tableVar->findElem("a");
		pl = lexeme->tableVar->findElem("b");
		pl = lexeme->tableVar->findElem("ba");

		cout << "answer = " << pl.i << ", " << pl.j << endl << endl;
		for (int i = 0; i < lexeme->tableVar->array.size(); i++)
		{
		 cout << i << ": ";
		 for (int j = 0; j < lexeme->tableVar->array[i].size(); j++)
		 {
		 cout << j << ":" << "(" << lexeme->tableVar->array[i][j].name << " " << lexeme->tableVar->array[i][j].value << ") ";
		 }
		 cout << endl;
		}

		token a = token(4, 26, 0);
		lexeme->getElemByToken(a);
		cout << "name = " << lexeme->var.name << "  value = " << lexeme->var.value << endl;
	system("pause");
}