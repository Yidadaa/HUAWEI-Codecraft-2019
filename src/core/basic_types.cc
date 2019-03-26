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
  ss >> id >> length >> max_speed >> from_id >> to_id >> channels_num >> is_duplex;
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

int Cross::addCar() {
  /*TODO*/
  return 1;
}

int Traffic::initTraffic(string car_path, string road_path, string cross_path) {
  initInstance<Car>(car_path);
  initInstance<Road>(road_path);
  initInstance<Cross>(cross_path);
  return 1;
}

template<class TrafficInstance>
int Traffic::initInstance(string file_path) {
  vector<TrafficInstance> instances;

  ifstream road_file;
  road_file.open(file_path);

  if (!road_file.is_open()) return -1;

  string temp_string;
  while (getline(road_file, temp_string)) {
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

  cout << instances.size() << endl;

  road_file.close();
  return 1;
}