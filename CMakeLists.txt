cmake_minimum_required(VERSION 3.8)
project(gear_turtle)

set(CMAKE_CXX_STANDARD 17)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)

add_executable(gear src/gear.cpp)
ament_target_dependencies(gear rclcpp geometry_msgs turtlesim)

install(TARGETS gear DESTINATION lib/${PROJECT_NAME})

ament_package()
