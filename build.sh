SDK_DIR="./SDK/SDK_C++"

if [ -d dist ]; then
  rm -rf dist
fi
mkdir dist

cp -r ${SDK_DIR}/* dist

cp -r ./src/core dist/CodeCraft-2019

cd dist
sh build.sh