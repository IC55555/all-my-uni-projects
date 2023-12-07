#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct VarTableElem  //������� ����������
{
	VarTableElem(string name, string type, bool value) :name(name), value(value) {} //�������� ����������
	VarTableElem() : name(""), value(0) {} // ���������� �� ���������
	string name; // ������ ����
	bool value; // ������ ��������
};

struct place // ���������� ���������(��� ������)
{
	int i;
	int j;
	place(int i, int j) :i(i), j(j) {} //�������� ����������
	place() :i(), j() {}// ���������� �� ���������
};

struct token // ��������� ��� ����������� ���� ��������
{
	int table;
	int i;
	int j;
	token(int table, int i, int j) :table(table), i(i), j(j) {}//�������� ����������
};

class ConstTable //������� ��������
{
public:
	vector<string> array;
	ConstTable(string fileName) //�����������
	{
		readTable(fileName);
	};
	~ConstTable() // ����������
	{
		array.~vector();
	}

	int findElem(string Name)
	{
		auto res = find(array.begin(), array.end(), Name);
		if (res == array.end())
			return -1; // ���� ������� �� ������, ��������� -1
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
	void readTable(string fileName) //���������� � ������ �� �����
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
	vector <vector<VarTableElem>> array; //������ �������� ��� ����� ���������(�������)

	TableVar()
	{
		array.resize(52);
	};
	~TableVar()
	{
		array.~vector();
	}

	place findElem(string name) // ����� �� ��������� ����������� ���������
	{
		place pl;
		auto hash = getHash(name);
		pl.i = hash; // ��� - ��� ������� ������� ������, � ���� ��� �� ����� ������
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

	void changeElem(place pl, bool value) // ����������� 1 ��� 0 ��������, � ����������� �� ��� �������
	{
		array[pl.i][pl.j].value = value;
	}
	VarTableElem getElem(place pl)
	{
		return array[pl.i][pl.j];
	}

private:
	int getHash(string h)  // ��������� ���������� ���� ������� ������� ������(���)
	{
		char len = 0; //�������� 0 ���� �� ������� �� len, ����� ���������
		if (h[0] >= 'A' && h[0] <= 'Z')
			len = h[0] - 'A';
		if (h[0] >= 'a' && h[0] <= 'z')
			len = h[0] - 'a' + 26;
		return (int)len;
	}
};

class TableInt //������� ������ ���� int
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

	place findElem(string value) // ����� �� ��������� ����������� ���������
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
	void changeElem(place pl, int value) // ����������� 1 ��� 0 ��������, � ����������� �� ��� �������
	{
		array[pl.i][pl.j] = value;
	}
	int getElem(place pl)
	{
		return array[pl.i][pl.j];
	}

private:
	int getHash(string h) // ��������� ���������� ���� ������� ������� ������(���)
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

	Lexeme() // ����������� �� ���������(������)
	{}

	string str;
	int value = 0;
	VarTableElem var;

	void getElemByToken(token tmp) //���� ��������� �� ���������� ������
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

	//           ���� � ������� 'while':
	/*index = lexeme->keyWords->findElem("while");
	cout << endl << "answer = " << index << endl << endl;*/
	//
	//
	//	//           ���� � ������� ��������
	//	for (int i = 0; i < lexeme->operations->array.size(); i++)
	//		cout << lexeme->operations->array[i] << endl;
	//	index = lexeme->operations->findElem("=");
	//	cout << "answer = " << index << endl << endl; 
	//
	////	           ��������� ��������� ����������� � �������:
	//	for (int i = 0; i < lexeme->separators->array.size(); i++)
	//		cout << lexeme->separators->array[i] << endl;
	//	index = lexeme->separators->findElem(";");
	//	cout << "answer = " << index << endl << endl;


		//           ��������� ������������ ������������ �������� � �������:
		//for (int i = 0; i < lexeme->operations->array.size(); i++)
		//	cout << lexeme->operations->array[i] << endl;
		//index = lexeme->operations->findElem("+");
		//index = lexeme->operations->findElem("-");
		//index = lexeme->operations->findElem(">=");
		//index = lexeme->operations->findElem("<=");
		//index = lexeme->operations->findElem(">");
		//index = lexeme->operations->findElem("<");
		//cout << "answer =" << index << endl << endl;


		//           ���� � ������� �������� �������� ���� int
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


		//           ���� �������� � ������� ����������
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