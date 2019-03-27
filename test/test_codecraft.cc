#include <gtest/gtest.h>
#include "../src/core/basic_types.h"

int the_argc;
char** the_argv;

class TrafficTestEnv : public testing::Test {
public:
  virtual void SetUp() {
    std::string car_path(the_argv[1]);
    std::string road_path(the_argv[2]);
    std::string cross_path(the_argv[3]);
    
    traffic.initTraffic(car_path, road_path, cross_path);
  }
  virtual void TearDown() {
    cout << "TrafficTestEnv tears down" << endl;
  }
  Traffic traffic = Traffic();
};

class SimpleTrafficData : public testing::Test {
public:
  virtual void SetUp() {
    vector<string> test_cars = {
      "1 1 2 3 1",
      "2 1 2 3 2",
      "3 1 2 3 1",
      "4 1 2 3 2"
    };
    for (auto s:test_cars) {
      cars.push_back(Car(s));
    }
  }
  vector<Car> cars;
};

TEST_F(TrafficTestEnv, test_traffic) {
  ASSERT_EQ(the_argc, 4) << "Test arguments must be 4";
  EXPECT_EQ(traffic.cars.size(), 128);
  EXPECT_EQ(traffic.roads.size(), 60);
  EXPECT_EQ(traffic.crosses.size(), 36);

  /* Test Car */
  EXPECT_EQ(traffic.cars[0].id, 10000);
  EXPECT_EQ(traffic.cars[0].from_id, 15);
  EXPECT_EQ(traffic.cars[0].to_id, 35);
  EXPECT_EQ(traffic.cars[0].max_speed, 6);
  EXPECT_EQ(traffic.cars[0].plan_time, 1);

  /* Test Road */
  EXPECT_EQ(traffic.roads[0].id, 5000);
  EXPECT_EQ(traffic.roads[0].length, 10);
  EXPECT_EQ(traffic.roads[0].max_speed, 5);
  EXPECT_EQ(traffic.roads[0].channels_num, 1);
  EXPECT_EQ(traffic.roads[0].from_id, 1);
  EXPECT_EQ(traffic.roads[0].to_id, 2);
  EXPECT_EQ(traffic.roads[0].is_duplex, 1);

  /* Test Cross */
  EXPECT_EQ(traffic.crosses[0].id, 1);
  EXPECT_EQ(traffic.crosses[0].top_road_id, 5000);
  EXPECT_EQ(traffic.crosses[0].right_road_id, 5005);
  EXPECT_EQ(traffic.crosses[0].bottom_road_id, -1);
  EXPECT_EQ(traffic.crosses[0].left_road_id, -1);
}

/* 测试神奇车库的出库功能 */
TEST_F(SimpleTrafficData, test_cross_car_port) {
  auto test_cross = Cross("1 1000 1001 1002 1003");

  for (auto c:cars) {
    test_cross.addCar(c);
  }
  const int n = int(cars.size());
  ASSERT_EQ(test_cross.car_port.size(), n);
  int cases[] = { 1, 3, 2, 4 };
  for (int i = 0; i < n; i++) {
    EXPECT_EQ(test_cross.car_port.top().id, cases[i]);
    test_cross.car_port.pop();
  }
}

/* 测试索引建立功能 */
TEST_F(SimpleTrafficData, test_traffic_buildmapindex) {
  auto traffic = Traffic();
  auto car_index = traffic.buildMapIndex(cars);
  for (int i = 0; i < int(cars.size()); i++) {
    EXPECT_EQ(car_index[cars[i].id], i);
  }
}

int main(int argc, char **argv) {
  the_argc = argc;
  the_argv = argv;
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
