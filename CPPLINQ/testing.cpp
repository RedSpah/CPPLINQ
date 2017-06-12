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

TEST_CASE("Container constuction with LINQ()", "[Construction]")
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

TEST_CASE("Sorting I - Basic", "[Sorting]")
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

TEST_CASE("Sorting II - Comparator Function", "[Sorting]")
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
	auto stable_sort_tester_comparator_sorted = LINQ(std::vector<int>({ 4,5,6,7,1,2,3 }));
	auto stable_sort_tester_value_extractor = [](stable_sort_tester v) -> int {return v.V; };
	auto stable_sort_tester_comparator = [](const stable_sort_tester& l, const stable_sort_tester& r) -> bool {return -l.O < -r.O; };

	SECTION("Comparator Function Stable Sort")
	{
		REQUIRE(stable_sort_tester_container.copy().stable_sort(stable_sort_tester_comparator).transform(stable_sort_tester_value_extractor).to_vector() == stable_sort_tester_comparator_sorted.to_vector());
	}
}

TEST_CASE("Sorting III - Value Function", "[Sorting]")
{
	auto unsorted_container = LINQ(std::vector<int>({ 1, -4, -7, 2, 8, -9, -5, 6, 10, -3, 0 }));
	auto naively_sorted_container = LINQ(std::vector<int>({-9,-7,-5,-4,-3,0,1,2,6,8,10}));
	auto sorted_container = LINQ(std::vector<int>({ 0,1,2,-3,-4,-5,6,-7,8,-9,10 }));
	auto value_function = [](int n) {return std::abs(n); };

	SECTION("Value Function Sort")
	{
		CHECK(unsorted_container.copy().sort(value_function).to_vector() == sorted_container.to_vector());
		CHECK_FALSE(unsorted_container.copy().sort(value_function).to_vector() == naively_sorted_container.to_vector());
	}

	SECTION("Value Function Is Sorted")
	{
		CHECK(unsorted_container.copy().sort(value_function).is_sorted(value_function));
		CHECK_FALSE(unsorted_container.copy().sort(value_function).is_sorted());
		CHECK_FALSE(unsorted_container.copy().sort().is_sorted(value_function));
	}

	SECTION("Value Function Partial Sort")
	{
		CHECK(unsorted_container.copy().partial_sort(5, value_function).copy_slice(0, 4).to_vector() == std::vector<int>({ 0,1,2,-3,-4 }));
		CHECK_FALSE(unsorted_container.copy().partial_sort(5, value_function).copy_slice(5, 10).to_vector() == std::vector<int>({ 5,6,-7,8,-9,10 }));
	}

	auto stable_sort_tester_container = LINQ(std::vector<stable_sort_tester>({ { 4, 4 },{ 3, 5 },{ 7, 1 },{ 5, 2 },{ 1, 6 },{ 4, 3 },{ 0, 7 } }));
	auto stable_sort_tester_value_function = [](stable_sort_tester l) -> int {return l.O + l.V;};
	auto stable_sort_tester_value_sorted = LINQ(std::vector<int>({ 2,6,3,7,4,5,1 }));
	auto stable_sort_tester_value_extractor = [](stable_sort_tester v) -> int {return v.V; };

	SECTION("Comparator Function Stable Sort")
	{
		CHECK(stable_sort_tester_container.copy().stable_sort(stable_sort_tester_value_function).transform(stable_sort_tester_value_extractor).to_vector() == stable_sort_tester_value_sorted.to_vector());
	} 
}

TEST_CASE("Partitioning", "[Partitioning]")
{
	auto unpartitioned_container = LINQ(std::vector<stable_sort_tester>({ { 4, 4 },{ 3, 5 },{ 7, 1 },{ 5, 2 },{ 1, 6 },{ 4, 3 },{ 0, 7 } }));
	auto partitioning_function = [](stable_sort_tester l) -> bool {return l.O % 2 == 1; };
	auto stable_sort_tester_value_extractor = [](stable_sort_tester v) -> int {return v.V; };

	SECTION("Partition")
	{
		CHECK(unpartitioned_container.copy().partition(partitioning_function).take_while(partitioning_function).transform(stable_sort_tester_value_extractor).is_permutation(LINQ(std::vector<int>({ 5,1,2,6 }))));
		CHECK(unpartitioned_container.copy().partition(partitioning_function).skip_while(partitioning_function).transform(stable_sort_tester_value_extractor).is_permutation(LINQ(std::vector<int>({ 4,3,7 }))));
	}

	SECTION("Is Partitioned")
	{
		CHECK(unpartitioned_container.copy().partition(partitioning_function).is_partitioned(partitioning_function));
		CHECK_FALSE(unpartitioned_container.copy().is_partitioned(partitioning_function));
	}

	SECTION("Stable Partition")
	{
		CHECK(unpartitioned_container.copy().stable_partition(partitioning_function).take_while(partitioning_function).transform(stable_sort_tester_value_extractor).to_vector() == std::vector<int>({ 5,1,2,6 }));
	}
}

TEST_CASE("Non-Modifying Sequence Operators", "[SeqOps]")
{
	auto test_container = LINQ(std::vector<int>({ 1,2,3,6,5,4,7,8,9 }));

	SECTION("All Of")
	{
		auto all_of_filter = [](int i) {return i < 10; };

		CHECK(test_container.all_of(all_of_filter));
		CHECK_FALSE(test_container.copy().push_back(10).all_of(all_of_filter));
	}

	SECTION("Any Of")
	{
		auto any_of_filter = [](int i) {return i == 6; };

		CHECK(test_container.any_of(any_of_filter));
	}

	SECTION("None Of")
	{
		auto none_of_filter = [](int i) {return i == 10; };

		CHECK(test_container.none_of(none_of_filter));
		CHECK_FALSE(test_container.copy().push_back(10).none_of(none_of_filter));
	}

	SECTION("Any")
	{
		CHECK(test_container.any());
		CHECK_FALSE(container<int>::empty().any());
	}

	SECTION("Count")
	{
		CHECK(test_container.count() == 9);
		CHECK(test_container.copy().push_back(10).count() == 10);
	}

	SECTION("Count(const T& val)")
	{
		CHECK(test_container.count(1) == 1);
		CHECK(test_container.copy().push_back(1).count(1) == 2);
	}

	SECTION("Count If")
	{
		auto count_if_filter = [](int i) {return i < 7; };

		CHECK(test_container.count_if(count_if_filter) == 6);
		CHECK(test_container.copy().push_back(6).count_if(count_if_filter) == 7);
	}

	SECTION("Find If")
	{
		auto find_if_container = LINQ(std::vector<stable_sort_tester>({ {2,2}, {3,0}, {4,3} }));
		auto find_if_filter = [](stable_sort_tester l) {return l.O == 2; };
	
		CHECK(find_if_container.find_if(find_if_filter).V == 2);
	}

	SECTION("Contains")
	{
		CHECK(test_container.contains(5));
		CHECK_FALSE(test_container.contains(10));
	}

	SECTION("Equal")
	{
		CHECK(test_container.equal(test_container));
		CHECK(test_container.equal(LINQ(std::vector<int>({ 1,2,3,6,5,4,7,8,9 }))));
		CHECK_FALSE(test_container.equal(LINQ(std::vector<int>({ 1,2,3,6,5,4,7,8,8 }))));
	}
}