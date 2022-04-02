cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
project(tcms_hz3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#cpp_utl
set(PATH_CPP_UTL D:/Liyuan/cpp_utility)
include_directories(${PATH_CPP_UTL}/include/)
include_directories(${PATH_CPP_UTL}/3rdparty/)
link_directories(${PATH_CPP_UTL}/lib/)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)

add_definitions(-DBOOST_ASIO_STANDALONE)

add_executable(tcms_hz3
  main.cpp
  tcmsdata.cpp
  serialporttcms.cpp
)

target_link_libraries(tcms_hz3 Qt5::Core)
target_link_libraries(${PROJECT_NAME} cpp_utility_all_in_one)
