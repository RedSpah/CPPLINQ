#include <iostream>
#include <string>
#include "cpplinq.h"

using namespace cpplinq;
using namespace std;

struct colstr
{
	std::vector<int> ints;

	colstr(std::initializer_list<int> i) : ints(i) {};
	colstr() {};
};

int main()
{
	std::vector<colstr> colv = { {1,2},{2,3},{3,4} };

	std::vector<int> i{ 0,1,2,3,4,5,6,7,8,9,10 };
	std::vector<int> k{ 1,2,3,4,5,6,7,8,9,10 };

	auto s = LINQ(i).Concat(k).Where([](int x) {return x < 9; }).Where([](int x, int i) {return (x + i) > 9; }).ToVector();


	for (auto c : s) 
	{ 
		cout << c << " ";
		//cout << c.first << " ";
		//std::for_each(c.second.begin(), c.second.end(), [](int v) {cout << v << " "; });
		//cout << '\n';
	}
	cin.get();
}