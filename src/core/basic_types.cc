#include "basic_types.h"

using namespace std;

Road::Road(int id_, int length_, int max_speed_, int from_id_, int to_id_,
    int channels_, bool is_duplex_) {
  id = id_;
  length = length;
  max_speed = max_speed_;
  from_id = from_id_;
  to_id = to_id_;
  channels_num = channels_;
  is_duplex = is_duplex_;
}

Road::Road(string s) {
  stringstream ss;
  ss << s;
  ss >> id >> length >> max_speed >> channels_num
    >> from_id >> to_id >> is_duplex;
}

/* 车辆驶入道路的逻辑 */
void Road::addCar(Car* car, int last_cross_id) {
  // 根据车辆驶来的方向来确认要驶入的道路
  auto* target_channels = last_cross_id == this->from_id
    ? &s2e_channels : &e2s_channels;
    // TODO: 处理车辆驶入道路的逻辑
  target_channels->at(0).push(car);
}

/* 获取可用的车道索引 */
int getAvailableChannelIndex(vector<queue<Car*>>& channels) {
  int index = 0;
  for (auto channel = channels.begin(); channel != channels.end(); channel++) {
    if (channel->back()->at_road_position > 0) {
      return index;
    }
  }
  return -1;
}

Car::Car(int id_, int from_id_, int to_id_, int max_speed_, int plan_time_) {
  id = id_;
  from_id = from_id_;
  to_id = to_id_;
  max_speed = max_speed_;
  plan_time = plan_time_;
}

/**
 * 从流中生成类实例，字符串流必须包含与类构造函数相匹配的参数数目
 */
Car::Car(string s) {
  stringstream ss;
  ss << s;
  ss >> id >> from_id >> to_id >> max_speed >> plan_time;
}

void Car::updateStatus(int new_status) {
  status = new_status;
}

void Car::changeSpeed(int new_s) {
  if (new_s > max_speed) throw "The new speed cannot larger than max speed!";
  speed = new_s;
}

Cross::Cross(int id_, int top_road_id_, int right_road_id_, int bottom_road_id_,
    int left_road_id_) {
  id = id_;
  top_road_id = top_road_id_;
  right_road_id = right_road_id_;
  bottom_road_id = bottom_road_id_;
  left_road_id = left_road_id_;
}
Cross::Cross(string s) {
  stringstream ss;
  ss << s;
  ss >> id >> top_road_id >> right_road_id >> bottom_road_id >> left_road_id;
}

/* 车辆入库操作使用指针，来保证全局状态统一 */
int Cross::addCar(Car* c) {
  car_port.push(c);
  return car_port.size();
}

void Traffic::initTraffic(string car_path, string road_path, string cross_path) {
  // 构建路网的基本实例
  cars = initInstance<Car>(car_path);
  roads = initInstance<Road>(road_path);
  crosses = initInstance<Cross>(cross_path);
  car_id2index = buildMapIndex<Car>(cars);
  road_id2index = buildMapIndex<Road>(roads);
  cross_id2index = buildMapIndex<Cross>(crosses);

  portCarsToPort();
}

template<class TrafficInstance>
vector<TrafficInstance> Traffic::initInstance(string file_path) {
  vector<TrafficInstance> instances;

  ifstream the_file;
  the_file.open(file_path);

  if (!the_file.is_open()) return instances;

  string temp_string;
  while (getline(the_file, temp_string)) {
    // 跳过注释行
    if (temp_string[0] == '#') continue;

    // 过滤掉不需要的字符
    string filtered_string;
    for (auto s:temp_string) {
      if (s != '(' && s != ')' && s != ',') {
        filtered_string += s;
      }
    }

    // 生成类实例
    instances.push_back(TrafficInstance(filtered_string));
  }

  the_file.close();
  return instances;
}

/* 将车辆停靠在对应的车库中去 */
void Traffic::portCarsToPort() {
  for (auto c = cars.begin(); c != cars.end(); c++) {
    if (c->from_id) {
      auto the_cross_index = cross_id2index.find(c->from_id);
      if (the_cross_index != cross_id2index.end()) {
        crosses[the_cross_index->second].addCar(&*c);
      }
    }
  }
}