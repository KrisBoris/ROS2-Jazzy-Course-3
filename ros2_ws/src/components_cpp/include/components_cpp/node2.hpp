#include <chrono>
#include <thread>
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class MultiThreadExecutorNode2 : public rclcpp::Node
{
public:
    MultiThreadExecutorNode2();

private:
    rclcpp::TimerBase::SharedPtr timer4;
    rclcpp::TimerBase::SharedPtr timer5;

    void timer4_callback();    
    void timer5_callback();    
};
