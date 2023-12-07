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
//__________________________�������������� ������__________________________\\
	
	// ������� ������� �������
struct synt_table_el
{
	vector<string> terminal; 	// ������������ ������
	int jump;			// �������
	bool accept;			// ��������� ��� ���
	bool put_in_stack;		// ������ � ���� ��� ���
	bool should_return;		// ������������ ��� ���
	bool error;			// ���������� ������ ��� ���
};

// ������� ����������� ������
struct postfix_elem
{
	string id;
	short int type; // 1 - �������������, ��������� ��� ���� ��������, 2 - ;

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

string get_token_text(tokens &get_t);	// ���������� ����� ������ ���������� � ������
vector <synt_table_el> parsing_table;	// ������� �������
stack <int> parsing_stack;			// ����, ������������ ��� �������
vector<postfix_elem> postfix_record;	// ����������� ������	
ifstream fin;
ofstream err, token, out;

bool parse(); // �������������� ������
bool make_postfix(vector<tokens> t, ofstream &parse_error_f); // ���������� ����������� ������
void postfix_print(string file_tree); // ������ ����������� ������ � ����
bool priority(string what1, string what2); // ��������� ����������� ��������
