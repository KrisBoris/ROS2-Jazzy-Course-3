#include "components_cpp/node2.hpp"

MultiThreadExecutorNode2::MultiThreadExecutorNode2() : rclcpp::Node("multi_thread_executor_node_2")
{           
    timer4 = this->create_wall_timer(
        1000ms, std::bind(&MultiThreadExecutorNode2::timer4_callback, this));
    timer5 = this->create_wall_timer(
        1000ms, std::bind(&MultiThreadExecutorNode2::timer5_callback, this));
    RCLCPP_INFO(this->get_logger(), "Node 2 has been started");
}

void MultiThreadExecutorNode2::timer4_callback()
{        
    std::this_thread::sleep_for(2000ms);
    RCLCPP_INFO(this->get_logger(), "cb 4");
}

void MultiThreadExecutorNode2::timer5_callback()
{
    std::this_thread::sleep_for(2000ms);
    RCLCPP_INFO(this->get_logger(), "cb 5");
}