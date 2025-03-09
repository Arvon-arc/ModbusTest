cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(PrintNumbersProject VERSION 1.2.3)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 包含头文件目录
include_directories(include)
include_directories(lib/modbus/)

set(LIBMODBUS_VERSION_MAJOR 1)
set(LIBMODBUS_VERSION_MINOR 1)
set(LIBMODBUS_VERSION_MICRO 1)
set(LIBMODBUS_VERSION 1)

# 配置版本号替换
configure_file(
    ${CMAKE_SOURCE_DIR}/lib/modbus/modbus-version.h.in
    ${CMAKE_BINARY_DIR}/lib/modbus/modbus-version.h
)

# 包含生成的头文件目录
include_directories(${CMAKE_BINARY_DIR}/lib/modbus)

# 查找src目录下的所有源文件
file(GLOB SRC_SOURCES "src/*.cc")
file(GLOB SRC_SOURCES "lib/modbus//*.c")

# 创建动态库
add_library(PrintNumbersLib SHARED ${SRC_SOURCES})

# 查找test目录下的所有源文件
file(GLOB TEST_SOURCES "test/*.cc")

# 为每个测试源文件创建一个可执行文件，并链接动态库
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PrintNumbersLib)
endforeach()


