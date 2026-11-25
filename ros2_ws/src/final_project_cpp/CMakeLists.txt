cmake_minimum_required(VERSION 3.8)
project(final_project_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(turtlesim REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(my_robot_interfaces REQUIRED)

include_directories(include)

add_library(move_turtle_component SHARED src/move_turtle_server.cpp)
ament_target_dependencies(move_turtle_component rclcpp rclcpp_action 
  rclcpp_lifecycle rclcpp_components turtlesim geometry_msgs my_robot_interfaces)
rclcpp_components_register_nodes(move_turtle_component "final_project_cpp::MoveTurtleServerNode")

install(TARGETS 
  move_turtle_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

ament_package()
