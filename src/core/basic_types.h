#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#if !defined(CORE_BASIC_TYPES_H_)
#define CORE_BASIC_TYPES_H_

using namespace std;

class Car {
  public:
    int id; // 车辆id
    int from_id; // 始发地路口id
    int to_id; // 目的地路口id
    int max_speed; // 最高时速
    int plan_time; // 计划出发时间

    int status = 0; // 0-未出发状态 1-等待行驶状态 2-终止状态

    int at_road_id = -1; // 车辆当前所处的道路
    int at_channel_id = -1; // 车辆当前所处的车道

    Car(int id_, int from_id_, int to_id_, int max_speed_, int plan_time_);
    Car(string s);
    void updateStatus(int new_status);

    /* 车辆在车库中出库的优先级
     * 1. 如果出发时间不同，则先出发者优先级高
     * 2. 如果出发时间相通，则id小者优先级高
     */
    struct cmpAtPort {
      bool operator () (Car a, Car b) {
        return !(
          (a.plan_time < b.plan_time) ||
          (a.plan_time == b.plan_time && a.id < b.id)
        );
      }
    };
};

class Road {
  public:
    int id; // 道路id
    int length; // 道路长度
    int max_speed; // 最高限速
    int from_id; // 起点路口id
    int to_id; // 终点路口id
    int channels_num; // 车道数目
    bool is_duplex; // 是否双向道路

    vector<vector<Car>> s2e_channels; // 正向车道 from_cross -> to_cross
    vector<vector<Car>> e2s_channels; // 反向车道 to_cross -> from_cross

    Road(int id_, int length_, int max_speed_, int from_id_, int to_id_,
        int channels_, bool is_duplex_);
    Road(string s);
};

class Cross {
  public:
    int id; // 路口id
    int top_road_id; // 0点方向路口id
    int right_road_id; // 3点方向路口id
    int bottom_road_id; // 6点方向路口id
    int left_road_id; // 9点方向路口id

    // 神奇车库，使用优先队列来实现，出库的时候按照id以及出发时间进行排序出发
    priority_queue<Car, vector<Car>, Car::cmpAtPort> car_port;

    Cross(int id_, int top_road_id_, int right_road_id_, int bottom_road_id_,
        int left_road_id_);
    Cross(string s);

    /* 向车库中添加一辆车 */
    int addCar(const Car& c);
};

class Traffic {
  public:
    vector<Cross> crosses;
    vector<Road> roads;
    vector<Car> cars;

    map<int, int> car_id2index;
    map<int, int> road_id2index;
    map<int, int> cross_id2index;

    int initTraffic(string car_path, string cross_path, string road_path);
    template<class TrafficInstance>
    vector<TrafficInstance> initInstance(string file_path);

    template<typename T>
    map<int, int> buildMapIndex(vector<T>& vs) {
      map<int, int> tmp_id2index;
      for (int i = 0; i < int(vs.size()); i++) {
        tmp_id2index[vs[i].id] = i;
      }
      return tmp_id2index;
    };
    void portCarsToPort();
};

#endif // CORE_BASI_CTYPES_H_
