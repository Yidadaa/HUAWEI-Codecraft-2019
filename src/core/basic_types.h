#include<string>
#include<vector>
#include<queue>

#if !defined(CORE_BASIC_TYPES_H_)
#define CORE_BASIC_TYPES_H_

using namespace std;

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
        int channels_, bool is_duplex_) {
      id = id;
      length = length;
      max_speed = max_speed_;
      from_id = from_id_;
      to_id = to_id_;
      channels_num = channels_;
      is_duplex = is_duplex_;
    }
};

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

    Car(int id_, int from_id_, int to_id_, int max_speed_, int plan_time_) {
      id = id_;
      from_id = from_id_;
      to_id = to_id_;
      max_speed = max_speed_;
      plan_time = plan_time_;
    }

    void updateStatus(int new_status) {
      status = new_status;
    }
};

class Cross {
  public:
    int id; // 路口id
    int top_road_id; // 0点方向路口id
    int right_road_id; // 3点方向路口id
    int bottom_road_id; // 6点方向路口id
    int left_road_id; // 9点方向路口id

    priority_queue<Car> car_port; // 神奇车库，使用优先队列来实现

    Cross(int id_, int top_road_id_, int right_road_id_, int bottom_road_id_,
        int left_road_id_) {
      id = id_;
      top_road_id = top_road_id_;
      right_road_id = right_road_id_;
      bottom_road_id = bottom_road_id_;
      left_road_id = left_road_id_;
    }

    int addCar() {}
};

#endif // CORE_BASI_CTYPES_H_
