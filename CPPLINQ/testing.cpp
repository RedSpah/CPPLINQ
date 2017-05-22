#include "cpplinq.h"
#include <iostream>

int main()
{
	using namespace cpplinq::templ;

	std::vector<int> TestVector = {0, 1, 2, 3, 4, 5, 7, 9, 11};
	std::forward_list<int> TestList = {0, 1, 2, 3, 4, 5};
	auto refienum_test = cpplinq::LINQ(TestVector);
	//auto stdienum_test = cpplinq::LINQ(std::move(TestVector));
	auto movienum_test = cpplinq::LINQ(std::move(TestList));

	auto l = [](int i) {return i < 2; };

	//sanitizer_t<decltype(l), int> a;
	constexpr bool sas = is_defined_v<decltype(l), int>;

	constexpr bool vv = is_func_v<decltype(l), bool, int>;

	constexpr bool b = is_counter_func_v<decltype(l), int>;

	/*
	constexpr bool n = is<n_func<decltype(l), int>>;

	is_func_2<bloat<decltype(l), bool, int>, bool>;
	constexpr bool b = is<filter<decltype(l), int>>;
	*/
	//std::cout << vas << "\n";

	//cpplinq::templ::is_func_defined<decltype(l), int>::value;
	//using s = typename std::result_of<decltype(l)(int)>::type;
	/*
	auto d = refienum_test.Select([](int n) {return (float)n + 0.5; });
	for (float a : d) { std::cout << a << " "; }
	*/
	//decltype(d) v;

	//stdienum_test.Distinct();

	// Unit tests I think:

	//auto orderby1_1 = test_base.Copy().OrderBy([](int a) {return a; }).ToVector();
	/*
	if (orderby1_1 == std::vector<int>{0, 1, 2, 3, 4, 5})
	{
		std::cout << "IEnum OrderBy(F1) Test 1 Passed" << std::endl;
	}

	auto orderby1_2 = test_base.Copy().OrderBy([](int a) {return -a; }).ToVector();

	if (orderby1_2 == std::vector<int>{5, 4, 3, 2, 1, 0})
	{
		std::cout << "IEnum OrderBy(F1) Test 2 Passed" << std::endl;
	}
	*/
	std::cin.get();
	return 0;
}