#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct DFA
{
	char state;
	DFA *right;
	DFA *left;


};

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
	token() :table(), i(), j() {}
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
	ConstTable *delimiters;

	Lexeme(string fileName)
	{

		createTokens(fileName, "tokenFile.txt", "errorFile.txt");
	}


	string str;
	int value = 0;
	VarTableElem var;

	bool createTokens(string codeName, string tokenName, string errorName)
	{

		token tmp;
		ofstream tokenFile(tokenName), errorFile(errorName);
		ifstream code(codeName);
		char ch;
		string str;
		int flag;
		bool unexpected = true;
		place pl;
		if (code.peek() != EOF)
		{
			code.get(ch);
			while (1)
			{
				unexpected = true;
				if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
				{
					do
					{
						str += ch;
						if (code.peek() == EOF)
						{ 
							errorFile << "EOF"; return false;
						}
						code.get(ch);
					} while (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == '_');

					flag = keyWords->findElem(str);
					if (flag != -1)
					{
						tmp.table = 0;
						tmp.i = flag;
						tmp.j = -1;
						tokenFile << tmp.table << " " << tmp.i << " " << tmp.j << endl;
						unexpected = false;
					}
					else
					{
						pl = tableVar->findElem(str);
						tmp.table = 4;
						tmp.i = pl.i;
						tmp.j = pl.j;
						tokenFile << tmp.table << " " << tmp.i << " " << tmp.j << endl;
						unexpected = false;
					}
					str.clear();
				}

				if (ch >= '0' && ch <= '9')
				{
					do
					{
						str += ch;
						if (code.peek() == EOF) 
						{ 
							errorFile << "EOF";
							return false;
						}
						code.get(ch);
						if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
						{
							errorFile << "error2: unexpected constant";
							return false;
						}
					} while (ch >= '0' && ch <= '9');

					pl = tableInt->findElem(str);
					tmp.table = 3;
					tmp.i = pl.i;
					tmp.j = pl.j;
					tokenFile << tmp.table << " " << tmp.i << " " << tmp.j << endl;
					unexpected = false;
					str.clear();
				}

				if (ch == '=')
				{
					str = ch;
					if (code.peek() == EOF) { errorFile << "EOF";	return false; }
					code.get(ch);
					tmp.table = 1;
					tmp.i = operations->findElem(str);
					tmp.j = -1;
					tokenFile << tmp.table << " " << tmp.i << " " << tmp.j << endl;
					unexpected = false;
					str.clear();
					while (ch == ' ')
					{
						if (code.peek() == EOF) { errorFile << "EOF"; return false; }
						code.get(ch);
					}
					if (ch == '-' || ch == '+')
					{
						if (ch == '-') str = ch;
						if (code.peek() == EOF) { errorFile << "EOF"; return false; }
						code.get(ch);
					}
				}

				if (ch == '+' || ch == '-' || ch == '*' || ch == '!' || ch == '<' || ch == '>')
				{
					str += ch;
					if (code.peek() == EOF) { errorFile << "EOF";	return false; }
					code.get(ch);
					if (ch == '+' || ch == '-' || ch == '=')
					{
						str += ch;
						if (code.peek() == EOF) { errorFile << "EOF"; return false; }
						code.get(ch);
					}
					flag = operations->findElem(str);
					if (flag != -1)
					{
						tmp.table = 1;
						tmp.i = flag;
						tmp.j = -1;
						tokenFile << tmp.table << " " << tmp.i << " " << tmp.j << endl;
						unexpected = false;
					}
					else { errorFile << "error1"; return false; }
					str.clear();
				}

				if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ':' || ch == ';' || ch == ',' || ch == '.')
				{
					str += ch;
					flag = delimiters->findElem(str);
					if (flag != -1)
					{
						tmp.table = 2;
						tmp.i = flag;
						tmp.j = -1;
						tokenFile << tmp.table << " " << tmp.i << " " << tmp.j << endl;
						unexpected = false;
					}
					else { errorFile << "error1: unexpected symbol"; return false; }
					str.clear();
					if (code.peek() == EOF) { errorFile << "EOF";	return false; }
					code.get(ch);
				}

				if (ch == '/')
				{
					if (code.peek() == EOF) { errorFile << "error1: unexpected symbol";	return false; }
					code.get(ch);
					if (ch == '/')
						do
						{
							if (code.peek() == EOF) { errorFile << "EOF";	return false; }
							code.get(ch);
						} while (ch != '\n');
					else if (ch == '*')
					{
						char ch2 = ' ';
						do
						{
							if (code.peek() == EOF) { errorFile << "error3: unclosed comment";	return false; }
							code.get(ch);
							if (ch == '*')
							{
								if (code.peek() == EOF) { errorFile << "error3: unclosed comment";	return false; }
								code.get(ch2);
							}
						} while (!(ch == '*' && ch2 == '/'));

					}
					else { errorFile << "error2"; return false; }
					unexpected = false;
					if (code.peek() == EOF) { errorFile << "EOF";	return false; }
					code.get(ch);
				}

				while (ch == '\n' || ch == '\t' || ch == ' ')
				{
					unexpected = false;
					if (code.peek() == EOF) { errorFile << "EOF";	return false; }
					code.get(ch);
				}

				if (unexpected)
				{
					errorFile << "error1: unexpected symbol"; return false;
				}
			}
		}
		tokenFile.close();
		errorFile.close();
		code.close();
	}


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

DFA* init()
{
	DFA *temp = new DFA;//S
	temp -> state = 'S';
	temp -> right = NULL;
	temp -> left = NULL;
	DFA *S = temp;

	temp -> left = new DFA;
	temp = temp -> left; //V
	temp -> state = 'V';
	temp -> right = NULL;
	temp -> left = NULL;
	//S -> left = temp;

	temp -> left = new DFA;
	temp = temp ->left;//F
	temp -> state = 'F';
	temp -> right = temp;
	temp -> left = temp;
	DFA *F = temp;
	
	temp = S -> left;
	temp ->left = F;

	temp = S -> right; //U
	temp = new DFA;
	temp -> state = 'U';
	temp -> right = F;
	temp -> left = NULL;
	S -> right = temp;

	temp = temp ->left;//Z
	temp = new DFA;
	temp -> state = 'Z';
	temp -> right = S -> right;
	temp -> left = S -> left;

	S -> left -> right = temp;
	S -> right -> left = S -> left -> right;

	return S;
}


string input()
{
	string str;
	cin >> str;
	
	return str;
}

char process(string str, DFA* S)
{
	DFA *p = S;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '0')
			p = p -> left;
		else if (str[i] == '1')
			p = p -> right;
		else
		{
			cout << "input error";
			return 'E';
		}	
	}
	return p -> state;
}

void main()
{
	//auto lexeme = new Lexeme();
	//int index;
	//for (int i = 0; i < lexeme->keyWords->array.size(); i++)
	//	cout << lexeme->keyWords->array[i] << "  ";

	////           Ищем в таблице 'while':
	//index = lexeme->keyWords->findElem("while");
	//cout << endl << "answer = " << index << endl << endl;
	//token a = token(1, 1, 0);
	//lexeme->getElemByToken(a);
	//cout << "name = " << lexeme->var.name << "  value = " << lexeme->var.value << endl;

	/*DFA *S = init();
	cout << "Enter a chain numbers: ";
	string str = input();
	cout << process(str, S) << endl;*/
	
	auto lexeme = new Lexeme("code.txt");
	system("pause");
}