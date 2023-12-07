#pragma once
#include <iostream>
#include <cmath>
#include <string>
#include <list>
using namespace std;

string DecToR(int d, int r)
{
	string Result = "";
	char Sym;
	while (d >= r)
	{
		if (d % r < 10)
		{
			Sym = d % r + 48;
			Result = Sym + Result;
		};
		if (d % r >= 10)
		{
			Sym = d % r + 55;
			Result = Sym + Result;
		};
		d /= r;
	};
	if (d < 10)
	{
		Sym = d + 48;
		Result = Sym + Result;
	};
	if (d >= 10)
	{
		Sym = d + 55;
		Result = Sym + Result;
	};
	return Result;
}

int RToDec(int r, string num)
{
	list<int> list;
	int p = -1;
	int result = 0;
	for (int i = 0; i < num.length(); i++)
	{
		if (num[i] >= 48 && num[i] <= 57)
		{
			list.push_back(num[i] - 48);
		};
		if (num[i] >= 65 && num[i] <= 90)
		{
			list.push_back(num[i] - 55);
		};
		p++;
	};
	for (int n : list)
	{
		result += n * pow(r, p);
		p--;
	};
	return result;
}

string ToUTF8(string win)
{
	if (win.length() == 0)
	{
		return "";
	}
	int num, decbuf;
	string result = "", hexnum, binbuf, u8bin;
	for (int i = 0; i < win.length(); i++)
	{
		num = win[i];
		hexnum = DecToR(num, 16);
		while (hexnum.length() < 4)
		{
			hexnum = "0" + hexnum;
		}
		if (hexnum.compare("007F") <= 0)
		{
			while (hexnum[0] == '0')
			{
				hexnum = hexnum.substr(1);
			}
			result = result + hexnum + " ";
		}
		else if (hexnum.compare("007F") > 0 && hexnum.compare("07FF") <= 0)
		{
			decbuf = RToDec(16, hexnum);
			binbuf = DecToR(decbuf, 2);
			u8bin = "110" + string(binbuf, 0, 5) + "10" + string(binbuf, 5, 6);
			decbuf = RToDec(2, u8bin);
			hexnum = DecToR(decbuf, 16);
			while (hexnum[0] == '0')
			{
				hexnum = hexnum.substr(1);
			}
			result = result + hexnum + " ";
		}
		else
		{
			decbuf = RToDec(16, hexnum);
			binbuf = DecToR(decbuf, 2);
			while (binbuf.length() < 16)
			{
				binbuf = "0" + binbuf;
			}
			u8bin = "1110" + string(binbuf, 0, 4) + "10" + string(binbuf, 4, 6) + "10" + string(binbuf, 10, 6);
			decbuf = RToDec(2, u8bin);
			hexnum = DecToR(decbuf, 16);
			while (hexnum[0] == '0')
			{
				hexnum = hexnum.substr(1);
			}
			result = result + hexnum + " ";
		}
	}
	result.pop_back();
	return result;
}

//int main(int argc, char *argv[])
//{
//	wstring input;
//	cout << "Enter line: ";
//	getline(wcin, input);
//	string output = ToUTF8(input);
//	cout << "Output: " << output << endl;
//	system("pause");
//	return 0;
//}
