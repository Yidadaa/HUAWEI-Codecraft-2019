# 如果build路径不存在，则创建该路径
if [ -d './build' ]; then
  rm -rf build
fi

if [ -d './bin' ]; then
  rm -rf bin
fi

mkdir build

cd build
cmake ..
make
cd ../bin
./TestCodeCraft ../data/config/car.txt ../data/config/road.txt ../data/config/cross.txt