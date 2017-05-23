#define CATCH_CONFIG_MAIN
#include "cpplinq.h"
#include "catch.hpp"

using namespace cpplinq;

TEST_CASE("IEnumerable constuction", "[IEnumerable]")
{
	std::vector<int> test_vector_copy = { 1, 2, 3, 4, 5 };
	std::vector<int> test_vector_move = { 1, 2, 3, 4, 5 };
	std::forward_list<int> test_list_move = { 1, 2, 3, 4, 5 };

	SECTION("Constructing by copy")
	{
		auto copy_ienumerable = LINQ(test_vector_copy);

		REQUIRE(copy_ienumerable.ToVector() == test_vector_copy);
	}
	
	SECTION("Constructing by move of vector")
	{
		auto vec_move_ienumerable = LINQ(std::move(test_vector_move));

		REQUIRE(vec_move_ienumerable.ToVector() == test_vector_copy);
	}

	SECTION("Constructing by move of other container")
	{
		auto cont_move_ienumerable = LINQ(std::move(test_list_move));

		REQUIRE(cont_move_ienumerable.ToVector() == test_vector_copy);
	}
}
