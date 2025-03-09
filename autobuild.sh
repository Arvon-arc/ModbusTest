#!/bin/bash

# 设置项目根目录
PROJECT_DIR=$(dirname "$(readlink -f "$0")")

# 设置构建目录
BUILD_DIR="$PROJECT_DIR/build"

# 创建构建目录
mkdir -p "$BUILD_DIR"

# 进入构建目录
cd "$BUILD_DIR"

# 运行CMake配置项目
make clean
cmake ..

# 构建项目
make

# 返回项目根目录
cd "$PROJECT_DIR"

# 删除构建目录
rm -rf "$BUILD_DIR"

echo "Build completed and build directory removed."