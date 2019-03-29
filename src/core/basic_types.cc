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

/* 获取可用的车道索引 */
int Road::getAvailableChannelIndex(vector<queue<Car*>> channels) {
  int index = 0;
  for (auto channel = channels.begin(); channel != channels.end(); channel++) {
    if (channel->back()->at_road_position > 0) {
      return index;
    }
  }
  return -1;
}

/* 车辆驶入道路的逻辑 */
void Road::addCar(Car* car, int last_cross_id) {
  // 根据车辆驶来的方向来确认要驶入的道路
  vector<queue<Car*>>* target_channels = &s2e_channels;
  int direction = 1;
  if (last_cross_id != from_id) {
    if (!is_duplex) throw runtime_error("道路不是双向车道！");
    target_channels = &e2s_channels;
    direction = -1;
  }
  // TODO: 处理车辆驶入道路的逻辑
  // 1. 判断车道空闲情况，注意调用该函数时，目标车道一定是空闲的，否则将会抛出错误
  int availableChannelIndex = this->getAvailableChannelIndex(*target_channels);
  // int availableChannelIndex = 0;
  if (availableChannelIndex < 0)
    throw runtime_error("不合法的调度，目标道路无空闲");
  // 2. 车辆驶入新道路
  target_channels->at(availableChannelIndex).push(car);
  // 3. 更改车辆所在道路以及车道
  car->changeRoad(this, availableChannelIndex, direction);
  target_channels->at(0).push(car);
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
  if (new_s > max_speed)
    throw runtime_error("The new speed cannot larger than max speed!");
  speed = new_s;
}

void Car::changeRoad(Road* road_, int channel_, int direction) {
  if (channel_ >= road_->channels_num)
    throw runtime_error("Target road channels wrong!");
  at_road = road_;
  at_channel_id = channel_;
  at_channel_direction = direction;
}

void Car::changePosition(int new_p) {
  if (new_p > at_road->length) throw runtime_error("车辆所在位置超出车道限制！");
  at_road_position = new_p;
}

/* 车辆往前行驶一个时间单位 */
void Car::move() {
  // 有两种情况，一种是车辆在车道内部行驶，另一种是车辆经过路口，进入下一条路
  // 车辆开始行驶时，必须处于等待行驶状态，行驶过后，变为终止状态
  if (status != 1) throw runtime_error("只有处于等待状态的车辆才能行驶！");
  // 开始判断车辆是否需要驶出路口
  Car* front_car = getFrontCar();

  int speed = min(max_speed, at_road->max_speed);
  int could_move_distance = this->at_road_position + speed;

  if (front_car != NULL) {
    // 存在前车
    if (front_car->status == 1) {
      // 前车处于等待行驶状态，则本车保持不动
      this->updateStatus(1);
    } else if (front_car->status == 2) {
      // 前车处于终止状态，则本车向前行驶，并将自身状态设置为终止状态
      could_move_distance = min(could_move_distance,
        front_car->at_road_position - this->at_road_position - 1);
      this->changePosition(this->at_road_position + could_move_distance);
      this->updateStatus(2); // 车辆移动完毕，处于终止状态
    } else {
      throw runtime_error("道路存在不明情况的前车");
    }
  } else {
    // 不存在前车
    if (could_move_distance > at_road->length) {
      // 出道路
      // 当该车辆要行驶出路口时，将车辆状态设置为等待行驶状态
      // 因为路口处的规则比较复杂，需要单独进行处理
      this->updateStatus(1);
    } else {
      // 不出道路，直接处理车辆状态即可
      this->changePosition(this->at_road_position + could_move_distance);
      this->updateStatus(2);
    }
  }
}

Car* Car::getFrontCar() {
  auto* channels = at_channel_direction > 0 ?
    &at_road->s2e_channels : &at_road->e2s_channels;
  auto* channel = &channels->at(at_channel_id);
  Car* result;
  // 如果车辆是车道中最靠前的一辆车，则返回NULL，表示没有前车
  if (this->id == channel->back()->id) return NULL;
  // 否则就搜索前车
  for (auto it = channel->back(); it->id != this->id; it--) {
    result = it;
    // 一直搜索到队首，都搜索不到，则报错
    if (it == channel->front())
      throw runtime_error("车辆状态与所在道路状态不一致，请检查！");
  }
  return result;
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
  buildIndex();
}

void Traffic::initTraffic(vector<Car> cars_,
    vector<Cross> crosses_, vector<Road> roads_) {
  cars = cars_;
  crosses = crosses_;
  roads = roads_;
  buildIndex();
}

void Traffic::buildIndex() {
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

/* 生成车辆的路径规划 */
void Traffic::getPathOfCar(Car* car) {
  struct CrossD {
    Cross* cross = NULL;
    CrossD* prior_crossd = NULL;
    Road* road = NULL;
    double min_d = __DBL_MAX__;
    int t;
  };
  vector<CrossD> G(int(crosses.size()), CrossD());
  // CrossD[crosses.size()] G;
  unordered_map<int, CrossD*> S;
  unordered_map<int, CrossD*> Q;
  // 定义比较函数
  auto cmp = [Q](pair<const int, CrossD*> a, pair<const int, CrossD*> b) {
    return a.second->min_d < b.second->min_d;
  };
  // 1. 初始化列表
  for (int i = 0; i < int(crosses.size()); i++) {
    G[i].cross = &crosses[i];
    G[i].t = car->plan_time;
    if (crosses[i].id == car->from_id) {
      G[i].min_d = 0;
    }
    Q[crosses[i].id] = &G[i];
  }
  // for (auto g:G) cout << g.cross->id << endl;
  // for (auto iter = Q.begin(); iter != Q.end(); iter++)
  // {
  // cout << iter->first << iter->second->cross->id << Q[iter->first]->cross->id << endl;
  // }
  // 2. 开始处理
  int last_q_size = Q.size(); // 判断死锁
  while (!Q.empty()) {
    auto min_iter = min_element(Q.begin(), Q.end(), cmp);
    auto u = min_iter->second;
    // cout << min_iter->first << endl;
    Q.erase(min_iter->first);
    S[u->cross->id] = &*u;
    vector<Road*> adjs = getAdjRoadOfCross(u->cross);
    // 遍历u的邻居结点，松弛对应的值
    for (auto it = adjs.begin(); it != adjs.end(); it++) {
      // 首先剔除掉已经在S中的结点
      cout << u->cross->id << ' ' << (*it)->from_id << endl;
      if(S.find((*it)->from_id) != S.end() &&
        S.find((*it)->to_id) != S.end()) continue;
      cout << "re: " << u->cross->id << ' ' << (*it)->from_id << endl;
      // 然后剔除不支持逆行的路口，即出发点不是u，并且不是双通路
      if((*it)->from_id != u->cross->id && !(*it)->is_duplex) continue;
      // 然后对剩下的路口进行计算
      int d_t = getTimeCostOf(car, *it);
      double new_d = u->min_d + getWeightOfRange(u->t, u->t + d_t, (*it)->id);
      // 找到该路口对应的目标节点
      int target_cross_id = (*it)->from_id != u->cross->id ?
        (*it)->from_id : (*it)->to_id;
      CrossD* v = Q[target_cross_id];
      // 如果新的权重值小于目标节点权重值，则更新对应权重
      if (new_d < v->min_d) {
        v->min_d = new_d;
        v->prior_crossd = &*u;
        v->road = *it;
        v->t = u->t + d_t;
      }
    }
    if (last_q_size == int(Q.size())) {
      throw runtime_error("路径生成失败！");
    };
    last_q_size = Q.size();
  }
  // 3. 此时S中所有点的权重均已更新完毕，生成对应的路径
  vector<Road*> path;
  auto target_cross = S.find(car->to_id);
  if (target_cross == S.end()) throw runtime_error("路径生成错误！");
  CrossD* the_crossd = target_cross->second;
  while (the_crossd->prior_crossd != NULL) {
    path.push_back(the_crossd->road);
    the_crossd = the_crossd->prior_crossd;
  }
  cout << path.size() << endl;
  reverse(path.begin(), path.end());
  car->path = path; // 更新车辆的路径
  updateWeightsByPath(car);
}

/* 根据车辆路径更新权重 */
void Traffic::updateWeightsByPath(Car* car) {
  if (car->path.empty()) throw runtime_error("车辆的路径为空！");
  int t = car->plan_time; // 开始时间
  for (auto p:car->path) {
    double time_cost = getTimeCostOf(car, p);
    for (int i = 0; i < time_cost; i++) {
      t += 1;
      // 道路权重是时间开销/车道数量
      double w = time_cost / p->channels_num;
      setWeightOf(t, p->id, w);
    }
  }
}

/* 获取某条路在某个时间段的平均权重 */
double Traffic::getWeightOfRange(int from_time, int to_time, int road_id) {
  double w = 0.0;
  int t = to_time - from_time + 1;
  while (from_time <= to_time) {
    w += getWeightOf(from_time, road_id);
    from_time ++;
  }
  return t > 0 ? w / t : w;
}

/* 获取某条路在某个时刻的权重 */
double Traffic::getWeightOf(int t, int road_id) {
  auto t_weights = id_time_weights.find(t);
  if (t_weights != id_time_weights.end()) {
    auto id_weights = t_weights->second.find(road_id);
    if (id_weights != t_weights->second.end()) {
      return id_weights->second;
    }
  }
  Road* r = getRoadById(road_id);
  return r->length;
}

/* 设置某条路在某个时刻的权重 */
void Traffic::setWeightOf(int t, int road_id, double w) {
  auto t_weights = id_time_weights.find(t);
  if (t_weights != id_time_weights.end()) {
    auto id_weights = t_weights->second.find(road_id);
    if (id_weights != t_weights->second.end()) {
      id_weights->second += w;
    }
  }
  id_time_weights[t][road_id] = w + getWeightOf(t, road_id);
}

/* 根据id获取路 */
Road* Traffic::getRoadById(int road_id) {
  auto index = road_id2index.find(road_id);
  if (index == road_id2index.end() ||
    index->second >= int(roads.size())) throw runtime_error("查询路出错！");
  return &roads.at(index->second);
}

/* 根据id获取车 */
Car* Traffic::getCarById(int car_id) {
  auto index = car_id2index.find(car_id);
  if (index == car_id2index.end() ||
      index->second >= int(cars.size())) {
    throw runtime_error("查询车辆出错！");
  };
  return &cars.at(index->second);
}

/* 根据id获取路口 */
Cross* Traffic::getCrossById(int cross_id) {
  auto index = cross_id2index.find(cross_id);
  if (index == cross_id2index.end() ||
    index->second >= int(crosses.size())) {
    throw runtime_error("查询路口出错！");
  }
  return &crosses.at(index->second);
}

int Traffic::getSpeedOf(Car* car, Road* road) {
  return min(car->max_speed, road->max_speed);
}

int Traffic::getTimeCostOf(Car* car, Road* road) {
  int speed = getSpeedOf(car, road);
  return ceil(double(road->length) / double(speed));
}

vector<Road*> Traffic::getAdjRoadOfCross(Cross* cross) {
  vector<Road*> result;
  int ids[] = {
    cross->top_road_id,
    cross->right_road_id,
    cross->bottom_road_id,
    cross->left_road_id
  };
  for (int i = 0; i < 4; ++i) {
    if (ids[i] > -1) {
      result.push_back(getRoadById(ids[i]));
    }
  }
  return result;
}