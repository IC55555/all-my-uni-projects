#include "Analizator.h"
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



analizator::analizator()
{
	Words_table.input("Words.txt");		// ������� �������� ���� 14
	Num_table.input("Num.txt");			// ������� ���� 11
	Letters_table.input("Letters.txt");	// ������� ���� 10
	Operations_table.input("Operations.txt");// ������� �������� 12
	Opers_table.input("Opers.txt");		// ������� �������� 12
	Signs_table.input("Signs.txt");		// ������� ������ 13
							// ������� ��������������� 15 (����������)
							// ������� �������� 16 (����������)
	// ���� ������� �������
	ifstream inp_tab("Parsing table.txt");
	while (!inp_tab.eof())
	{
		synt_table_el add_el; // ����������� �������
		string read_str;
		inp_tab >> read_str;
		while (read_str != "|")
		{
			add_el.terminal.push_back(read_str);
			inp_tab >> read_str;
		};
		inp_tab >> add_el.jump >> add_el.accept >> add_el.put_in_stack >> add_el.should_return >> add_el.error;
		parsing_table.push_back(add_el);
	};
};
//__________________________�������������� ������__________________________\\

string analizator::get_token_text(analizator::tokens &get_t) // ���������� ����� ������ ���������� � ������
{
	string rez_str;	// ������, ������� �� ��e�
	//��������� ������� ��� �����
	if (get_t.num_table == 0 && get_t.num_chain == 0 && get_t.num == 0)
		return "F:";
	if (get_t.num_table == 0 && get_t.num_chain == 0 && get_t.num == 1)
		return "!F";

	switch (get_t.num_table)	// ��e� �� ��������
	{
	case 12: // ������� �������� 12
	{
		Operations_table.search_elem(rez_str, get_t.num);
		break;
	}
	case 13: // ������� ������ 13
	{
		char sym;
		Signs_table.search_elem(sym, get_t.num);
		char s2[1];
		s2[0] = sym;
		rez_str.assign(s2, 1);
		break;
	}
	case 14: // ������� �������� ���� 14
	{
		Words_table.search_elem(rez_str, get_t.num);
		break;
	}
	case 15: // ������� ��������������� 15 (����������)
	{
		lexeme tmp_l;
		Ident_table.search_lexeme(get_t.num_chain, get_t.num, tmp_l);
		rez_str = tmp_l.name;
		break;
	}
	case 16: // ������� �������� 16 (����������)
	{
		lexeme tmp_l;
		Const_table.search_lexeme(get_t.num_chain, get_t.num, tmp_l);
		rez_str = tmp_l.name;
		break;
	}
	}
	return rez_str;
}

bool analizator::parse()
{
	ifstream parse_token_f(filetoken.c_str()); // ����� ��� ����� �������
	ofstream parse_error_f(errorfile.c_str()); // ����� ��� ������ ������

	bool local_error = false;		// ������� ������
	bool need_postfix = false;      // ����� �� ��������� ���������� ����������� ������ ��� ������ ������
	vector<tokens> code_expr_infix;  // ���� ��, �� ���� �������� ������ � ��������� (�������) �������
	int cur_row = 0;				// ������� ������ � ������� �������
	int prev_row;					// ���������� ������ � ������� �������
	bool have_type = false;			// ���������� �� �� ������ ���
	bool find_do = false, find_while = false, add_mark1 = false, add_mark2 = false; //����������� ����� � �����
	parse_token_f >> cur_t >> next_t;
	string token_str_prev;			// �������� ����������� ������

	while (!parse_token_f.eof() && !local_error)
	{
		string token_str = get_token_text(cur_t), mark; // ����� ����� ���������� � ������
		if (cur_t.num_table == 15 || cur_t.num_table == 16)
			token_str = "var";

		if (find_do && token_str == "var")
			add_mark1 = true;
		if (find_while && token_str == "var")
			add_mark2 = true;

		if (token_str == "var" || token_str == "(")
			need_postfix = true;

		bool find_terminal = false; // �������� �� ������ ��������

		for (int i = 0; i < parsing_table[cur_row].terminal.size() & !find_terminal; i++)
		{
			if (parsing_table[cur_row].terminal[i] == token_str)
				find_terminal = true;
		}

		if (find_terminal)		//������������
		{
			bool change_row = false; //����� ������

			if (parsing_table[cur_row].put_in_stack)
				parsing_stack.push(cur_row + 1); //��������� � ����

			if (parsing_table[cur_row].accept)  //��������� ��������
			{
				if (need_postfix) //��������� � ���� ��� ����������� ������
				{
					if (add_mark1) //��������� ����� do
					{
						code_expr_infix.push_back(tokens(0, 0, 0));
						add_mark1 = false;
						find_do = false;
					}
					code_expr_infix.push_back(cur_t);
					if (token_str == ")" && add_mark2) //��������� ����� while
					{
						code_expr_infix.push_back(tokens(0, 0, 1));
						add_mark2 = false;
						find_while = false;
					}
				}

				if (token_str == ";" || token_str == ",")  //���� ��������� ������ �������� ���������
				{
					make_postfix(code_expr_infix, parse_error_f); //������ ����������� ������
					code_expr_infix.clear(); //�������� ����� ���������
					need_postfix = false;
				}

				if (token_str == ";")
					have_type = false; //"��������" ���

				if (token_str == "int") //���������� ���
					have_type = true;


				if (token_str == "var" && have_type && cur_row == 31) //������� ��� � �������
					Ident_table.add_type(get_token_text(cur_t));
				else
				{
					lexeme l;
					Ident_table.search_lexeme(get_token_text(cur_t), l);
					if (l.type == false)
					{
						parse_error_f << "�������������� ������: ���������� �" << get_token_text(cur_t) << "� �� �������� ���!" << endl;
						local_error = true;
					}
				}

				if (cur_row == 45 && cur_t.num_table != 15) //���� ����� ���������� ��������� ���-�� ���������
				{
					parse_error_f << "�������������� ������: ��������� �" << get_token_text(cur_t) << "� �� ����� ���� ��������� ��������!" << endl;
					local_error = true;
				}

				cur_t = next_t;	// ������ ����� 
				if (!parse_token_f.eof())
					parse_token_f >> next_t; // ��������� ���������
			}

			if (parsing_table[cur_row].should_return)
			{
				prev_row = cur_row;	// ���������� ���������� 
				cur_row = parsing_stack.top(); // ���� ���� ����� �� �����, ����
				parsing_stack.pop();
				change_row = true;
			}

			if (!change_row && parsing_table[cur_row].jump != -1)
				cur_row = parsing_table[cur_row].jump;	// ���� ����, �������
		}
		else	// ���� ��������������
			if (parsing_table[cur_row].error) // ���� ������, �� ���������� �
			{
				local_error = true;
				parse_error_f << "�������������� ������: ������ � ��������� �" << get_token_text(cur_t) << "�" << endl;
			}
			else	//��������� �� ���������
				cur_row++;
		//���������, ����� �� �� �������� ����� ������
		token_str_prev = token_str;
		if (token_str_prev == "do")
			find_do = true;
		if (token_str_prev == "while")
			find_while = true;
	};

	parse_token_f.close();
	parse_error_f.close();
	return !local_error;
}

// ���������� ����������� ������
bool analizator::make_postfix(vector<tokens> t, ofstream &parse_error_f)
{
	stack<string> stack_temp;
	bool error_flag = false;
	int index = 0, i;
	while (index < t.size() && !error_flag)
	{
		for (i = index; i < t.size() && !error_flag && get_token_text(t[i]) != ";" && get_token_text(t[i]) != ","; i++)
		{
			string token_text = get_token_text(t[i]);
			//��������� �����
			if (token_text == "F:")
			{
				token_text = token_text + to_string(postfix_record.size() + 1);
				postfix_record.push_back(postfix_elem(token_text));
			}
			if (token_text == "!F")
			{
				token_text = token_text + to_string(postfix_record.size() + 2);
				postfix_record.push_back(postfix_elem(token_text));
			}
			//��������� ��������������� � ��������
			if (t[i].num_table == 15 || t[i].num_table == 16)
				postfix_record.push_back(postfix_elem(token_text));
			else //��������� ������
				if (token_text == "(")
					stack_temp.push(token_text);
				else
					if (token_text == ")")
					{
						while (!stack_temp.empty() && stack_temp.top() != "(")
						{
							string tmpstr = stack_temp.top();
							postfix_record.push_back(postfix_elem(tmpstr));
							stack_temp.pop();
						}
						if (stack_temp.empty())
						{
							parse_error_f << "�������������� ������: �������������� �)�!" << endl;
							error_flag = true;
						}
						else
							stack_temp.pop();
					}
					else
						if (t[i].num_table == 12) //��������� ��������
						{
							while (!stack_temp.empty() && priority(token_text, stack_temp.top()))
							{
								string tmpstr = stack_temp.top();
								postfix_record.push_back(postfix_elem(tmpstr));
								stack_temp.pop();
							}
							stack_temp.push(token_text);
						}
		}

		if (error_flag)
		{
			postfix_record.clear();
			return false;
		}
		else
		{
			while (!stack_temp.empty() && stack_temp.top() != "(" && stack_temp.top() != ")") //�������� ������
			{
				string tmpstr = stack_temp.top();
				postfix_record.push_back(postfix_elem(tmpstr, 1));
				stack_temp.pop();
			}
			if (!stack_temp.empty())
			{
				parse_error_f << "�������������� ������: ������� ������ ������!" << endl;
				error_flag = true;
			}
		}
		if (error_flag)
		{
			postfix_record.clear();
			return false;
		}
		index = i + 1;
		postfix_record.push_back(postfix_elem(";", 2));
	}
	return true;
}
// ������ ����������� ������ � ����
void analizator::postfix_print(string f_name)
{
	ofstream out_f(f_name.c_str());
	for (int i = 0; i < postfix_record.size(); i++)
		out_f << postfix_record[i] << " ";
	out_f.close();
}

// ��������� ����������� ��������
bool analizator::priority(string what1, string what2)
{
	int pw1 = 0;
	if (what1 == "=" || what1 == "+=" || what1 == "-=") pw1 = 10;
	else if (what1 == "!=" || what1 == "==") pw1 = 20;
	else if (what1 == "+" || what1 == "-") pw1 = 30;

	int pw2 = 0;
	if (what2 == "=" || what2 == "+=" || what2 == "-=") pw2 = 10;
	else if (what2 == "!=" || what2 == "==") pw2 = 20;
	else if (what2 == "+" || what2 == "-") pw2 = 30;

	if (pw1 <= pw2) return true;
	return false;
};
