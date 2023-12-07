#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <queue>
#include <map>

using namespace std;
//__________________________СИНТАКСИЧЕСКИЙ АНАЛИЗ__________________________\\
	
	// Элемент таблицы разбора
struct synt_table_el
{
	vector<string> terminal; 	// Терминальный символ
	int jump;			// Переход
	bool accept;			// Принимаем или нет
	bool put_in_stack;		// Кладем в стек или нет
	bool should_return;		// Возвращаемся или нет
	bool error;			// Возвращаем ошибку или нет
};

// Элемент постфиксной записи
struct postfix_elem
{
	string id;
	short int type; // 1 - идентификатор, константа или знак операции, 2 - ;

	postfix_elem()
	{
		id = "", type = 0;
	}

	postfix_elem(string id_, int type_)
	{
		id = id_, type = type_;
	}

	postfix_elem(string id_)
	{
		id = id_, type = 1;
	}

	friend ostream &operator << (ostream &ostream_, const postfix_elem &pe_)
	{
		ostream_ << pe_.id;
		return ostream_;
	}
};

struct tokens
{
	int table;
	int i;
	int j;
	tokens(int _table, int _i, int _j)
	{
		table = _table;
		i = _i;
		j = _j;
	}
	tokens() {}
};

string get_token_text(tokens &get_t);	// Определяем какая строка содержится в токене
vector <synt_table_el> parsing_table;	// Таблица разбора
stack <int> parsing_stack;			// Стек, используемый для разбора
vector<postfix_elem> postfix_record;	// Постфиксная запись	
ifstream fin;
ofstream err, token, out;

bool parse(); // Синтаксический анализ
bool make_postfix(vector<tokens> t, ofstream &parse_error_f); // Построение постфиксной записи
void postfix_print(string file_tree); // Печать постфиксной записи в файл
bool priority(string what1, string what2); // Сравнение приоритетов операций
