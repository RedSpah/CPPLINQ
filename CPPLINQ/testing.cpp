#include "cpplinq.h"

int main()
{
	std::vector<int> TestVector = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	std::forward_list<int> TestList = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	auto value = cpplinq::LINQ(TestVector);
	auto mvalue = cpplinq::LINQ(std::move(TestVector));
	auto mmvalue = cpplinq::LINQ(std::move(TestList));
	return 0;
}