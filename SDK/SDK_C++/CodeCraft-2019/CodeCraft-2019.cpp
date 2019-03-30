#include "iostream"
#include "core/basic_types.h"

int main(int argc, char *argv[])
{
    std::cout << "Begin" << std::endl;
  
  if(argc < 5){
    std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
    exit(1);
  }
  
  std::string carPath(argv[1]);
  std::string roadPath(argv[2]);
  std::string crossPath(argv[3]);
  std::string answerPath(argv[4]);
  
  std::cout << "carPath is " << carPath << std::endl;
  std::cout << "roadPath is " << roadPath << std::endl;
  std::cout << "crossPath is " << crossPath << std::endl;
  std::cout << "answerPath is " << answerPath << std::endl;
  
  auto test = Traffic();
  test.initTraffic(carPath, roadPath, crossPath);
  test.getAllCarPath();

  ofstream out_file;
  out_file.open(answerPath, ios::out | ios::trunc);
  vector<string> paths = test.path2string();
  for (auto s:paths) {
    out_file << s << endl;
  }
  
  return 0;
}