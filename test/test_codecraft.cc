#include<gtest/gtest.h>

TEST(Example, test1) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(3 > 2);
}

TEST(Example, test2) {
  EXPECT_EQ(1, 2);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
