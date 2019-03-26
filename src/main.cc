#include <iostream>
#include "core/basic_types.h"

int main(int argc, char const *argv[]) {
	if(argc < 2){
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}
	
	std::string car_path(argv[1]);
  std::string road_path(argv[2]);
  std::string cross_path(argv[3]);

  auto test = Traffic();
  test.initTraffic(car_path, road_path, cross_path);

  return 0;
}
