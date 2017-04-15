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

	

	//int a1 = LINQ(i).Aggregate([](int a, int v) {return a + v; });
	//int a2 = LINQ(i).Aggregate(31, [](int a, int v) {return a + v; });
	std::vector<int> a3 = LINQ(i).Concat(k).Distinct().ToVector();
	//auto y = LINQ(colv).SelectMany([](colstr c, int i) {return c.ints; }, [](colstr c, int i) {return i + c.ints[0]; }).ToVector();


	for (auto c : a3) 
	{ 
		cout << c << " ";
		//cout << c.first << " ";
		//std::for_each(c.second.begin(), c.second.end(), [](int v) {cout << v << " "; });
		//cout << '\n';
	}
	cin.get();
}