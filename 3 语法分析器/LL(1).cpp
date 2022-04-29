#include<iostream>
#include<set>
#include<algorithm>
#include<vector>
#include <iomanip>
#include<fstream>
#define maxn 100

using namespace std;
struct G {
	char left;	//产生式左部 
	set<char> first;	//每个候选式的first集合 
	string right;	//产生式右部
};
int total;//产生式总数 
G g[maxn];//产生式 
set<char> First[maxn];
set<char> Follow[maxn];
string terminal = "";//终结符 
string nonterminal = "";//非终结符 
vector<char> stack;
vector<char> input;
int haveNone[maxn] = { 0 };//有空 
int hadFollow[maxn] = { 0 };//已经求过follow 
int hadFirst[maxn] = { 0 };//已经求过first 
int table[maxn][maxn] = { 0 };//分析表 
void getFirst(char X)
{
	int flag = 0;
	int index = nonterminal.find(X);
	if (hadFirst[index] > total)return;
	else hadFirst[index]++;
	for (int i = 0; i < total; i++)
	{
		if (g[i].left == X)
		{
			if (haveNone[index]) {
				First[index].insert('@');
				if (g[i].right[0] == '@') {
					g[i].first.insert('@');
					continue;
				}
			}
			//第一个为终结符 
			if (terminal.find(g[i].right[0]) != string::npos)
			{
				g[i].first.insert(g[i].right[0]);
				First[index].insert(g[i].right[0]);
			}
			else {
				//第一个为非终结符 
				for (int j = 0; j < g[i].right.length(); j++)
				{
					//直到找到终结符或者不能推出空串的非终结符才停止
					if (terminal.find(g[i].right[j]) != string::npos)
					{
						g[i].first.insert(g[i].right[j]);
						First[index].insert(g[i].right[j]);
						break;
					}
					else {
						int index_temp = nonterminal.find(g[i].right[j]);
						getFirst(g[i].right[j]);
						set<char>::iterator iter;
						for (iter = First[index_temp].begin(); iter != First[index_temp].end(); iter++)
						{
							g[i].first.insert(*iter);
							First[index].insert(*iter);
						}
						if (!haveNone[index_temp])break;
					}
				}
			}
		}
	}
}

void getFollow(char X)
{
	int index = nonterminal.find(X);
	if (hadFollow[index] > total)return;
	else hadFollow[index]++;
	for (int i = 0; i < total; i++)
	{
		if (g[i].right.find(X) != string::npos) {
			int index_temp = g[i].right.find(X);
			for (int j = index_temp; j < g[i].right.length(); j++)
			{
				if (j + 1 < g[i].right.length()) {
					if (terminal.find(g[i].right[j + 1]) != string::npos) {
						Follow[index].insert(g[i].right[j + 1]);
						break;
					}
					else {
						set<char>::iterator iter;
						int temp = nonterminal.find(g[i].right[j + 1]);
						for (iter = First[temp].begin(); iter != First[temp].end(); iter++)
						{
							if (*iter != '@')
								Follow[index].insert(*iter);
						}
						if (haveNone[temp])continue;
						else break;
					}
				}
				else {
					getFollow(g[i].left);
					int temp = nonterminal.find(g[i].left);
					set<char>::iterator iter;
					for (iter = Follow[temp].begin(); iter != Follow[temp].end(); iter++)
					{
						Follow[index].insert(*iter);
					}

				}
			}
		}
	}
}

void getTable()
{
	for (int i = 0; i < total; i++)
	{
		int p = nonterminal.find(g[i].left);
		if (terminal.find(g[i].right[0]) != string::npos) {
			int q = terminal.find(g[i].right[0]);
			table[p][q] = i + 1;//第一种情况 
		}
		else if (g[i].right[0] == '@') {
			//第三种情况 
			set<char>::iterator iter;
			for (iter = Follow[p].begin(); iter != Follow[p].end(); iter++)
			{
				int temp = terminal.find(*iter);
				table[p][temp] = i + 1;
			}
		}
		else {//可能是第一种或者第二种 
			set<char>::iterator iter;
			for (iter = g[i].first.begin(); iter != g[i].first.end(); iter++)
			{
				if (*iter == '@')continue;
				int temp = terminal.find(*iter);
				table[p][temp] = i + 1;
				cout << p << " " << temp << " " << i + 1 << endl;
			}
			if (g[i].first.find('@') == g[i].first.end()) continue;//不能推出空串	
			//第一种 

			// 执行到这里就是第二种 
			for (iter = Follow[p].begin(); iter != Follow[p].end(); iter++)
			{
				int temp = terminal.find(*iter);
				table[p][temp] = i + 1;
			}
		}
	}
}

void getAnalysis(string str)
{
	stack.push_back('#');
	stack.push_back(nonterminal[0]);
	input.push_back('#');
	for (int i = str.length() - 1; i >= 0; i--) { input.push_back(str[i]); }
	cout << setw(20) << "分析栈" << setw(20) << "输入串" << setw(20) << "所用产生式" << endl;
	//如果剩余输入串长度不为0，就一直循环
	while (input.size() > 0) {
		string outputs = "";
		for (int i = 0; i < stack.size(); i++) {
			outputs += stack[i];
		}
		cout << setw(20) << outputs;
		outputs = "";
		for (int i = input.size() - 1; i >= 0; i--) {
			outputs += input[i];
		}
		cout << setw(20) << outputs;
		char a = stack[stack.size() - 1];
		char b = input[input.size() - 1];
		//如果可以匹配，并且都为# 
		if (a == b && a == '#') {
			cout << setw(20) << "分析成功!" << endl;
			return;
		}
		//可以匹配
		if (a == b) {
			stack.pop_back();
			input.pop_back();
			cout << setw(20) << a << " 匹配" << endl;
		}
		else if (table[nonterminal.find(a)][terminal.find(b)] > 0) {
			//如果有值
			int index = table[nonterminal.find(a)][terminal.find(b)] - 1;
			stack.pop_back();
			if (g[index].right != "@") {
				for (int i = g[index].right.length() - 1; i >= 0; i--) {
					stack.push_back(g[index].right[i]);
				}
			}
			cout << setw(20) << g[index].left << "->" << g[index].right << endl;
		}
		else {
			cout << setw(20) << "错误，它不是LL(1)型文法！" << endl;
			return;
		}
	}
}

void outputFirstAndFollow()
{
	cout << "终结符：" << terminal << endl;
	cout << "非终结符：" << nonterminal << endl;
	set<char>::iterator iter;
	cout << "First集合：" << endl;
	for (int i = 0; i < nonterminal.length(); i++)
	{
		getFirst(nonterminal[i]);
		cout << "First(" << nonterminal[i] << ")={";
		for (iter = First[i].begin(); iter != First[i].end(); iter++)
		{
			cout << (*iter) << ",";
		}
		cout << "}" << endl;
	}
	cout << "单个候选式First集：" << endl;
	for (int i = 0; i < total; i++)
	{
		cout << "First(" << g[i].left << "->" << g[i].right << ")={";
		for (iter = g[i].first.begin(); iter != g[i].first.end(); iter++)
		{
			cout << (*iter) << ",";
		}
		cout << "}" << endl;
	}
	Follow[0].insert('#');
	cout << "Follow集合" << endl;
	for (int i = 0; i < nonterminal.length(); i++)
	{
		getFollow(nonterminal[i]);
		cout << "Follow(" << nonterminal[i] << ")={";
		for (iter = Follow[i].begin(); iter != Follow[i].end(); iter++)
		{
			cout << (*iter) << ",";
		}
		cout << "}" << endl;
	}
}


void outputAnalysis()
{
	cout << setw(10) << "分析表:" << endl;
	cout << setw(10);
	for (int i = 0; i < terminal.length(); i++)
		cout << terminal[i] << setw(5);
	cout << endl;
	for (int i = 0; i < nonterminal.length(); i++)
	{
		cout << nonterminal[i] << setw(5);
		for (int j = 0; j < terminal.length(); j++)
		{
			cout << table[i][j] << setw(5);
		}
		cout << endl;
	}
}

int main()
{
	ifstream read;
	read.open("LL(1).txt", ios::app);
	read >> total;
	for (int i = 0; i < total; i++)
	{
		read >> g[i].left >> g[i].right;
		cout << g[i].left << "->" << g[i].right << endl;
		if (nonterminal.find(g[i].left) == string::npos)
		{
			nonterminal += g[i].left;
		}
		for (int j = 0; j < g[i].right.length(); j++)
		{
			if (g[i].right[j] == '@') {
				int temp = nonterminal.find(g[i].left);
				haveNone[temp] = 1;
			}
			else if (g[i].right[j] >= 'A' && g[i].right[j] <= 'Z')
			{
				if (nonterminal.find(g[i].right[j]) == string::npos)
				{
					nonterminal += g[i].right[j];
				}
			}
			else if (terminal.find(g[i].right[j]) == string::npos)
			{
				terminal += g[i].right[j];
			}

		}
	}
	outputFirstAndFollow();
	terminal += '#';
	getTable();
	outputAnalysis();
	getAnalysis("i+i");
	//getAnalysis("vi,i:r") ;
	return 0; 
}
