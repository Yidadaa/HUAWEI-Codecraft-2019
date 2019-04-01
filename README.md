# HUAWEI-Codecraft-2019
Private repo for HUAWEI codecraft 2019 via Team - HuaHuaGuai.  
**此代码在成渝赛区初赛排名35，总时间3526，憾失复赛名额。**

### 任务书阅读笔记
#### 系统调度方法
系统调度的顺序是逐车道，逐路口进行调度：
1. 按道路遍历车辆，确定各个车辆的状态：等待行驶状态，终止状态；
2. 处理所有路口处于等待状态的车辆，对于每一个处于等待状态的车辆；C，处理完C的状态之后，还要处理C所在的车道上的其他处于等待状态的车辆状态；
3. 在处理路口车辆时，要检测死锁；
4. 经过路口时交通规则的处理，这一块逻辑很复杂，要写好对应的测试文件；
5. 处理完所有在路上行驶的车辆之后，遍历所有路口的车库，确定是否有车辆到达出发时间；
6. 所有车辆进入终止状态，则本次调度结束。

#### 如何确定发车逻辑
1. 从源文件实例化车辆数据后，遍历所有车辆，将其存储在对应的路口中；
2. 根据[系统调度方法](#系统调度方法)第5条确定是否发车，如果有车辆到达发车时间，且初始车道存在余量，则按照[调度优先级](#调度优先级)第6条确定发车顺序，否则需要等待下一个时间片才能发车。

#### 调度优先级
1. 行驶车辆优先于待发车车辆；
2. 多车道中，相同道路同一位置的车辆，车道序号小者先行；
3. 多车道中，相同道路不同位置的车辆，无论车道序号如何，排在前面的车辆先行；
4. 路口中，对于同一驶入道路，直行进入的车辆先行，左转进入的车辆后之，最后是右转进入该道路的车辆；
5. 经过路口后，驶入目标道路时，优先进入序号较小车道，若较小车道入口处无空位，则顺次进入次小车道，以此进行，若所有车道均已满，则进入终止状态；
6. 待出发车辆出发时，车辆序号小者先行，驶入初始道路按上一条优先级执行。

#### 关于车道的需要注意的点
1. 车道有容量限制，一辆车的长度为1，则长度为L的单车道最多可以容纳L辆车。

### 关于核心代码的运行逻辑
1. 模拟车辆行驶时，逐车辆进行模拟，即每次调用Car.move()即可。

### 运行此项目
#### 安装`google test`依赖库
`google test`库只会在开发阶段用到，提交代码时并不会用到此库。安装方法：
1. 安装源代码：`sudo apt-get install libgtest-dev`
2. 编译源代码：`cd /usr/src/gtest && sudo cmake . && sudo make`
3. 安装编译完成的文件：`sudo cp libgtest*.a /usr/local/lib`

#### 编译并运行
```bash
mkdir build && cd build # 创建缓存目录
cmake .. && make # 编译源代码
cd ../bin && ./TestCodeCraft # 运行单元测试
```

### 代码风格约定
本仓库中所有代码应遵循[Google开源项目风格指南](https://zh-google-styleguide.readthedocs.io/en/latest/contents/)。

### 版权声明
本仓库下`SDK, doc, data`下数据及文件均来自于[华为软件精英挑战赛官网](https://codecraft.huawei.com/Generaldetail)，版权均归华为公司所有。
