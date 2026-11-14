#include "components_cpp/node1.hpp"

MultiThreadExecutorNode1::MultiThreadExecutorNode1() : rclcpp::Node("multi_thread_executor_node_1")
{        
    timer1 = this->create_wall_timer(
        1000ms, std::bind(&MultiThreadExecutorNode1::timer1_callback, this));
    timer2 = this->create_wall_timer(
        1000ms, std::bind(&MultiThreadExecutorNode1::timer2_callback, this));
    timer3 = this->create_wall_timer(
        1000ms, std::bind(&MultiThreadExecutorNode1::timer3_callback, this));
    RCLCPP_INFO(this->get_logger(), "Node 1 has been started");
}

void MultiThreadExecutorNode1::timer1_callback()
{        
    std::this_thread::sleep_for(2000ms);
    RCLCPP_INFO(this->get_logger(), "cb 1");
}
    
void MultiThreadExecutorNode1::timer2_callback()
{
    std::this_thread::sleep_for(2000ms);
    RCLCPP_INFO(this->get_logger(), "cb 2");
}

void MultiThreadExecutorNode1::timer3_callback()
{
    std::this_thread::sleep_for(2000ms);
    RCLCPP_INFO(this->get_logger(), "cb 3");
}