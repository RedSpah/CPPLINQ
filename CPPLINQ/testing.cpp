#define CATCH_CONFIG_MAIN
#include "cpplinq.h"
#include "catch.hpp"
#include <vector>
using namespace cpplinq;

struct stable_sort_tester
{
	int O = 0, V = 0;
	stable_sort_tester() {};
	stable_sort_tester(int O_, int V_) { O = O_; V = V_; }
	
};

bool operator<(const stable_sort_tester& p, const stable_sort_tester& o) { return p.O < o.O; }
bool operator>(const stable_sort_tester& p, const stable_sort_tester& o) { return p.O > o.O; }
bool operator==(const stable_sort_tester& p, const stable_sort_tester& o) { return p.O == o.O; }
bool operator!=(const stable_sort_tester& p, const stable_sort_tester& o) { return p.O != o.O; }

/*
int main()
{
	//std::forward()

	auto stable_sort_tester_container = LINQ(std::vector<stable_sort_tester>({ { 1, 1 },{ 1, 0 },{ 0, 2 },{ 0, 0 },{ 1, 1 },{ 0, 1 },{ 1, 2 } }));
	auto stable_sorted_values = LINQ(std::vector<int>({ 2, 0, 1, 1, 0, 1, 2 }));
	auto stable_sort_tester_comparator_function = [](stable_sort_tester l, stable_sort_tester r) -> bool {return -l.O < -r.O; };
	
	stable_sort_tester_container.copy().stable_sort().transform([](stable_sort_tester v) -> int {return v.V; }).equal(stable_sorted_values);


	auto c = LINQ(std::vector<int>({ 1,2,3,4,5,6,7 }));
	auto comparator_function = [](int l, int r) -> bool {return -l < -r; };
	auto d = c.sort(comparator_function);
}
*/

TEST_CASE("Container constuction", "[LINQ() Function]")
{
	std::vector<int> test_vector_copy = { 1, 2, 3, 4, 5 };
	std::vector<int> test_vector_move = { 1, 2, 3, 4, 5 };
	std::forward_list<int> test_list_move = { 1, 2, 3, 4, 5 };

	SECTION("Constructing by copy")
	{
		auto copy_ienumerable = LINQ(test_vector_copy);

		REQUIRE(copy_ienumerable.to_vector() == test_vector_copy);
	}
	
	SECTION("Constructing by move of vector")
	{
		auto vec_move_ienumerable = LINQ(std::move(test_vector_move));

		REQUIRE(vec_move_ienumerable.to_vector() == test_vector_copy);
	}

	SECTION("Constructing by move of other container")
	{
		auto cont_move_ienumerable = LINQ(std::move(test_list_move));

		REQUIRE(cont_move_ienumerable.to_vector() == test_vector_copy);
	}
}

TEST_CASE("Sorting I - Basic", "[Sorting I]")
{
	auto unsorted_container = LINQ(std::vector<int>({ 1, 4, 7, 2, 8, 9, 5, 6, 10, 3, 0 }));
	auto sorted_container = LINQ(std::vector<int>({ 0,1,2,3,4,5,6,7,8,9,10 }));

	SECTION("Basic Sort")
	{
		REQUIRE(unsorted_container.copy().sort().equal(sorted_container));
	}

	SECTION("Is Sorted")
	{
		REQUIRE_FALSE(unsorted_container.is_sorted());
		REQUIRE(unsorted_container.copy().sort().is_sorted());
		REQUIRE(sorted_container.is_sorted());
	}

	SECTION("Partial Sort")
	{
		REQUIRE(unsorted_container.copy().partial_sort(5).copy_slice(0, 4).to_vector() == std::vector<int>({ 0,1,2,3,4 }));
		REQUIRE_FALSE(unsorted_container.copy().partial_sort(5).copy_slice(5, 10).to_vector() == std::vector<int>({ 5,6,7,8,9,10 }));
	}

	auto stable_sort_tester_container = LINQ(std::vector<stable_sort_tester>({ { 1, 4 },{ 1, 5 },{ 0, 1 },{ 0, 2 },{ 1, 6 },{ 0, 3 },{ 1, 7 } }));
	auto stable_sorted_values = LINQ(std::vector<int>({ 1, 2, 3, 4, 5, 6, 7 }));
	auto stable_sort_tester_value_extractor = [](stable_sort_tester v) -> int {return v.V; };

	SECTION("Stable Sort")
	{
		REQUIRE(stable_sort_tester_container.copy().stable_sort().transform(stable_sort_tester_value_extractor).to_vector() == stable_sorted_values);
	}
}

TEST_CASE("Sorting II - Comparator Function", "[Sorting II]")
{
	auto unsorted_container = LINQ(std::vector<int>({ 1, 4, 7, 2, 8, 9, 5, 6, 10, 3, 0 }));
	auto sorted_container = LINQ(std::vector<int>({ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 }));
	auto comparator_function = [](int l, int r) -> bool {return -l < -r; };

	SECTION("Comparator Function Sort")
	{
		REQUIRE(unsorted_container.copy().sort(comparator_function).to_vector() == sorted_container.to_vector());
		REQUIRE_FALSE(unsorted_container.copy().sort().to_vector() == sorted_container.to_vector());
	}

	SECTION("Comparator Function Is Sorted")
	{
		REQUIRE(unsorted_container.copy().sort(comparator_function).is_sorted(comparator_function));
		REQUIRE_FALSE(unsorted_container.copy().sort(comparator_function).is_sorted());
	}

	SECTION("Comparator Function Partial Sort")
	{
		REQUIRE(unsorted_container.copy().partial_sort(5, comparator_function).copy_slice(0, 4).to_vector() == std::vector<int>({ 10,9,8,7,6 }));
		REQUIRE_FALSE(unsorted_container.copy().partial_sort(5, comparator_function).copy_slice(5, 10).to_vector() == std::vector<int>({ 5,4,3,2,1,0 }));
	}
	
	auto stable_sort_tester_container = LINQ(std::vector<stable_sort_tester>({ { 1, 4 },{ 1, 5 },{ 0, 1 },{ 0, 2 },{ 1, 6 },{ 0, 3 },{ 1, 7 } }));
	auto stable_sort_tester_comparator_sorted = LINQ(std::vector<int>({ 1,2,3,4,5,6,7 }));
	auto stable_sort_tester_value_extractor = [](stable_sort_tester v) -> int {return v.V; };

	SECTION("Comparator Function Stable Sort")
	{
		REQUIRE(stable_sort_tester_container.copy().stable_sort().transform(stable_sort_tester_value_extractor).to_vector() == stable_sort_tester_comparator_sorted.to_vector());
	}
}

TEST_CASE("Sorting III - Value Function", "[Sorting III]")
{
	auto unsorted_container = LINQ(std::vector<int>({ 1, -4, -7, 2, 8, -9, -5, 6, 10, -3, 0 }));
	auto naively_sorted_container = LINQ(std::vector<int>({-9,-7,-5,-4,-3,0,1,2,6,8,10}));
	auto sorted_container = LINQ(std::vector<int>({ 0,1,2,-3,-4,-5,6,-7,8,-9,10 }));
	auto value_function = [](int n) {return std::abs(n); };

	SECTION("Value Function Sort")
	{
		REQUIRE(unsorted_container.copy().sort(value_function).to_vector() == sorted_container.to_vector());
		REQUIRE_FALSE(unsorted_container.copy().sort(value_function).to_vector() == naively_sorted_container.to_vector());
	}

	SECTION("Value Function Is Sorted")
	{
		REQUIRE(unsorted_container.copy().sort(value_function).is_sorted(value_function));
		REQUIRE_FALSE(unsorted_container.copy().sort(value_function).is_sorted());
		REQUIRE_FALSE(unsorted_container.copy().sort().is_sorted(value_function));
	}

	SECTION("Value Function Partial Sort")
	{
		REQUIRE(unsorted_container.copy().partial_sort(5, value_function).copy_slice(0, 4).to_vector() == std::vector<int>({ 0,1,2,-3,-4 }));
		REQUIRE_FALSE(unsorted_container.copy().partial_sort(5, value_function).copy_slice(5, 10).to_vector() == std::vector<int>({ 5,6,-7,8,-9,10 }));
	}

	auto stable_sort_tester_container = LINQ(std::vector<stable_sort_tester>({ { 4, 4 },{ 3, 5 },{ 7, 1 },{ 5, 2 },{ 1, 6 },{ 4, 3 },{ 0, 7 } }));
	auto stable_sort_tester_value_function = [](stable_sort_tester l) -> int {return l.O + l.V;};
	auto stable_sort_tester_value_sorted = LINQ(std::vector<int>({ 2,6,3,7,4,5,1 }));
	auto stable_sort_tester_value_extractor = [](stable_sort_tester v) -> int {return v.V; };

	SECTION("Comparator Function Stable Sort")
	{
		REQUIRE(stable_sort_tester_container.copy().stable_sort(stable_sort_tester_value_function).transform(stable_sort_tester_value_extractor).to_vector() == stable_sort_tester_value_sorted.to_vector());
	} 
}


