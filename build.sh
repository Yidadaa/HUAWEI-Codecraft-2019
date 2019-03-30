SDK_DIR="./SDK/SDK_C++/CodeCraft-2019"

if [ -d ${SDK_DIR}/core ]; then
  rm -rf ${SDK_DIR}/core
fi

cp -r ./src/core ${SDK_DIR}