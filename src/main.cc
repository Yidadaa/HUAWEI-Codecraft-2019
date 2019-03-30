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
  std::string answer_path(argv[4]);

  auto test = Traffic();
  test.initTraffic(car_path, road_path, cross_path);
  test.getAllCarPath();

  ofstream out_file;
  out_file.open(answer_path, ios::out | ios::trunc);
  vector<string> paths = test.path2string();
  for (auto s:paths) {
    out_file << s << endl;
  }

  return 0;
}
