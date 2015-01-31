#include "utility.hpp"

#include <vector>

#include "gtest/gtest.h"


using std::vector;


TEST(UtilityTest, HasIntersection) {
	vector<int> v1a{1, 2, 2, 2, 5, 6, 7};
	vector<int> v1b{7, 8, 9};
	EXPECT_TRUE(HasIntersection(v1a.begin(), v1a.end(), v1b.begin(), v1b.end()));
	EXPECT_TRUE(HasIntersection(v1b.begin(), v1b.end(), v1a.begin(), v1a.end()));

	vector<int> v2a{0, 7, 9};
	vector<int> v2b{1, 2, 3, 4, 5, 6, 9};
	EXPECT_TRUE(HasIntersection(v2a.begin(), v2a.end(), v2b.begin(), v2b.end()));
	EXPECT_TRUE(HasIntersection(v2b.begin(), v2b.end(), v2a.begin(), v2a.end()));

	vector<int> v3a{};
	vector<int> v3b{1, 3, 3, 4, 5, 6, 9};
	EXPECT_FALSE(
			HasIntersection(v3a.begin(), v3a.end(), v3b.begin(), v3b.end()));
	EXPECT_FALSE(
			HasIntersection(v3b.begin(), v3b.end(), v3a.begin(), v3a.end()));

	vector<int> v4a{0, 3, 7, 8};
	vector<int> v4b{1, 4, 4, 4, 5, 6, 9};
	EXPECT_FALSE(
			HasIntersection(v4a.begin(), v4a.end(), v4b.begin(), v4b.end()));
	EXPECT_FALSE(
			HasIntersection(v4b.begin(), v4b.end(), v4a.begin(), v4a.end()));
}
