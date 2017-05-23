#include "cpplinq.h"
#include <iostream>

int main()
{
	using namespace cpplinq::templ;

	std::vector<int> TestVector = {0, 1, 2, 3, 4, 5, 7, 9, 11};
	std::forward_list<int> TestList = {0, 1, 2, 3, 4, 5};
	auto refienum_test = cpplinq::LINQ(TestVector);
	auto movienum_test = cpplinq::LINQ(std::move(TestList));

	auto l = [](int i) {return i < 2; };

	constexpr bool sas = is_defined_v<decltype(l), int>;

	constexpr bool vv = is_func_v<decltype(l), bool, int>;

	
	constexpr bool b = is_counter_func_v<decltype(l), int>;

	return 0;
}