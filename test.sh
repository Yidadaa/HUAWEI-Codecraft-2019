# 如果build路径不存在，则创建该路径
if [ ! -d './build' ]; then
  mkdir build
fi

cd build
cmake ..
make
cd ../bin
./TestCodeCraft ../data/config/car.txt ../data/config/road.txt ../data/config/cross.txt

./CodeCraft-2019 ../data/config/car.txt ../data/config/road.txt ../data/config/cross.txt ./answer.txt