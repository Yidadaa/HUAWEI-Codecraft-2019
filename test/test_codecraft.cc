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
  Traffic traffic = Traffic();
};

class SimpleTrafficData : public testing::Test {
public:
  virtual void SetUp() {
    vector<string> test_cars = {
      "1 1 2 3 1",
      "2 1 2 3 2",
      "3 1 2 3 1",
      "4 1 2 3 2",
      "5 2 3 3 2"
    };
    for (auto s:test_cars) {
      cars.push_back(Car(s));
    }
  }
  vector<Car> cars;
};

class ShortestPathTestData : public testing::Test {
public:
  virtual void SetUp() {
    vector<string> test_cars = {
      "1 1 4 1 1",
    };
    vector<string> test_roads = {
      "1 1 1 1 1 2 0",
      "2 2 1 1 1 3 0",
      "3 3 1 1 2 3 1",
      "4 4 1 1 2 4 0",
      "5 5 1 1 3 4 0"
    };
    vector<string> test_crosses = {
      "1 1 2 -1 -1",
      "2 4 3 1 -1",
      "3 3 5 2 -1",
      "4 4 5 -1 -1"
    };
    for (auto s:test_cars) cars.push_back(Car(s));
    for (auto s:test_roads) roads.push_back(Road(s));
    for (auto s:test_crosses) crosses.push_back(Cross(s));
    traffic.initTraffic(cars, crosses, roads);
  }
  vector<Car> cars;
  vector<Road> roads;
  vector<Cross> crosses;
  Traffic traffic = Traffic();
};

TEST_F(TrafficTestEnv, test_traffic) {
  ASSERT_EQ(the_argc, 4) << "Test arguments must be 4";
  EXPECT_EQ(traffic.cars.size(), 128);
  EXPECT_EQ(traffic.roads.size(), 60);
  EXPECT_EQ(traffic.crosses.size(), 36);

  auto* the_car = &traffic.cars[0];
  /* Test Car */
  EXPECT_EQ((*the_car).id, 10000);
  EXPECT_EQ((*the_car).from_id, 15);
  EXPECT_EQ((*the_car).to_id, 35);
  EXPECT_EQ((*the_car).max_speed, 6);
  EXPECT_EQ((*the_car).plan_time, 1);

  EXPECT_EQ(traffic.getCarById(the_car->id)->id, the_car->id);

  auto the_road = traffic.roads[0];
  /* Test Road */
  EXPECT_EQ(the_road.id, 5000);
  EXPECT_EQ(the_road.length, 10);
  EXPECT_EQ(the_road.max_speed, 5);
  EXPECT_EQ(the_road.channels_num, 1);
  EXPECT_EQ(the_road.from_id, 1);
  EXPECT_EQ(the_road.to_id, 2);
  EXPECT_EQ(the_road.is_duplex, 1);

  EXPECT_EQ(traffic.getRoadById(the_road.id)->id, the_road.id);

  auto the_cross = traffic.crosses[0];
  /* Test Cross */
  EXPECT_EQ(the_cross.id, 1);
  EXPECT_EQ(the_cross.top_road_id, 5000);
  EXPECT_EQ(the_cross.right_road_id, 5005);
  EXPECT_EQ(the_cross.bottom_road_id, -1);
  EXPECT_EQ(the_cross.left_road_id, -1);

  EXPECT_EQ(traffic.getCrossById(the_cross.id)->id, the_cross.id);

  /* 测试车辆是否正确地停到车库中 */
  int s = 0;
  for (auto cross:traffic.crosses) {
    s += cross.car_port.size();
  }
  EXPECT_EQ(s, traffic.cars.size());
}

/* 测试交通图的权重存取功能 */
TEST_F(TrafficTestEnv, test_traffic_weights_table) {
  const int t = 42;
  const int id = 42;
  const double w = 1.0;
  traffic.setWeightOf(t, id, w);
  EXPECT_EQ(traffic.getWeightOf(t, id), w);
  EXPECT_EQ(traffic.getWeightOf(t + 1, id), 0.0);
  EXPECT_EQ(traffic.getWeightOf(t, id + 1), 0.0);

  double sum_w = w;
  int end_time = t;
  for (int i = 1; i < 5; i++) {
    sum_w += w + i;
    end_time = t + i;
    traffic.setWeightOf(t + i, id, w + i);
  }
  EXPECT_EQ(
    traffic.getWeightOfRange(t, end_time, id),
    sum_w / (end_time - t + 1)
  );
}

/* 测试车辆路径权重更新函数 */
TEST_F(ShortestPathTestData, test_trafic_updateweightsbypath) {
  auto the_car = cars[0];
  the_car.path = { &roads[0], &roads[3] };
  EXPECT_EQ(the_car.from_id, the_car.path[0]->from_id);
  EXPECT_EQ(the_car.to_id, the_car.path[the_car.path.size() - 1]->to_id);
  // 测试权重更新函数
  traffic.updateWeightsByPath(&the_car);
  EXPECT_EQ(traffic.getWeightOfRange(1, 3, 1), 1.0 / 3);
  EXPECT_EQ(traffic.getWeightOfRange(3, 6, 4), 16.0 / 4);
}

/* 测试神奇车库的出库功能 */
TEST_F(SimpleTrafficData, test_cross_car_port) {
  auto test_cross = Cross("1 1000 1001 1002 1003");

  for (auto it = cars.begin(); it != cars.end(); it++) {
    test_cross.addCar(&*it);
    it->updateStatus(1); // 更改车辆状态，查看车库中车辆状态是否已同步更改
  }

  const int n = int(cars.size());
  ASSERT_EQ(test_cross.car_port.size(), n);

  int cases[] = { 1, 3, 2, 4, 5 };
  for (int i = 0; i < n; i++) {
    EXPECT_EQ(test_cross.car_port.top()->id, cases[i]);
    EXPECT_EQ(test_cross.car_port.top()->status, 1);
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

/* 车辆变更速度时的测试 */
TEST_F(SimpleTrafficData, test_change_car_speed) {
  auto car = cars[0];
  car.max_speed = 4;
  EXPECT_NO_THROW(car.changeSpeed(3));
  EXPECT_ANY_THROW(car.changeSpeed(5));
}

TEST_F(SimpleTrafficData, test_road_addcar) {
  // TODO: 测试车辆驶入逻辑
  // TODO: 测试getAvailableChannelIndex函数
}

int main(int argc, char **argv) {
  the_argc = argc;
  the_argv = argv;
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
