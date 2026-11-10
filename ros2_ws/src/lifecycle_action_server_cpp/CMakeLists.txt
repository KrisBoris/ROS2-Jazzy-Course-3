cmake_minimum_required(VERSION 3.8)
project(lifecycle_action_server_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(my_robot_interfaces REQUIRED)

add_executable(lifecycle_move_robot_server src/lifecycle_move_robot_server.cpp)
ament_target_dependencies(lifecycle_move_robot_server rclcpp rclcpp_action rclcpp_lifecycle my_robot_interfaces)

install(TARGETS
  lifecycle_move_robot_server
  DESTINATION lib/${PROJECT_NAME}/
)

ament_package()
