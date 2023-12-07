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

int main()
{
	analizator lex;
	int lex_er = lex.lex_analiz("Input_program.txt", "Errors.txt", "Tokens.txt", "Out.txt");
	if (lex_er)
	{
		lex.parse();
		lex.postfix_print("Tree.txt");
	}
	setlocale(0, "");
	return 0;
}
