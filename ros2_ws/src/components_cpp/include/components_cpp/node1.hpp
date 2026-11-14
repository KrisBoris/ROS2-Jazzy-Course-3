#include <chrono>
#include <thread>
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class MultiThreadExecutorNode1 : public rclcpp::Node
{
public:
    MultiThreadExecutorNode1();

private:
    rclcpp::TimerBase::SharedPtr timer1;
    rclcpp::TimerBase::SharedPtr timer2;
    rclcpp::TimerBase::SharedPtr timer3;

    void timer1_callback();    
    void timer2_callback();
    void timer3_callback();
};