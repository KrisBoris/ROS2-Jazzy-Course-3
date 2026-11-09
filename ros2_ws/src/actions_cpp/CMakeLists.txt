cmake_minimum_required(VERSION 3.8)
project(actions_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(my_robot_interfaces REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(count_until_server src/count_until_server.cpp)
ament_target_dependencies(count_until_server rclcpp rclcpp_action my_robot_interfaces)

add_executable(count_until_client src/count_until_client.cpp)
ament_target_dependencies(count_until_client rclcpp rclcpp_action my_robot_interfaces)

add_executable(move_robot_server src/move_robot_server.cpp)
ament_target_dependencies(move_robot_server rclcpp rclcpp_action my_robot_interfaces)

add_executable(move_robot_client src/move_robot_client.cpp)
ament_target_dependencies(move_robot_client rclcpp rclcpp_action my_robot_interfaces std_msgs)

add_executable(move_robot_cancel_node src/move_robot_cancel_node.cpp)
ament_target_dependencies(move_robot_cancel_node rclcpp std_msgs)


install(TARGETS
  count_until_server
  count_until_client
  move_robot_server
  move_robot_client
  move_robot_cancel_node
  DESTINATION lib/${PROJECT_NAME}/
)

ament_package()
