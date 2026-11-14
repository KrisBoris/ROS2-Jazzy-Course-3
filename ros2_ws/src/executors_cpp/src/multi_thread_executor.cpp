#include <chrono>
#include <thread>
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class MultiThreadExecutorNode1 : public rclcpp::Node
{
public:
    MultiThreadExecutorNode1() : rclcpp::Node("multi_thread_executor_node_1")
    {
        cb_group1 = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
        cb_group2 = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
        timer1 = this->create_wall_timer(
            1000ms, std::bind(&MultiThreadExecutorNode1::timer1_callback, this), cb_group1);
        timer2 = this->create_wall_timer(
            1000ms, std::bind(&MultiThreadExecutorNode1::timer2_callback, this), cb_group2);
        timer3 = this->create_wall_timer(
            1000ms, std::bind(&MultiThreadExecutorNode1::timer3_callback, this), cb_group2);
        RCLCPP_INFO(this->get_logger(), "Node 1 has been started");
    }

private:
    rclcpp::TimerBase::SharedPtr timer1;
    rclcpp::TimerBase::SharedPtr timer2;
    rclcpp::TimerBase::SharedPtr timer3;

    rclcpp::CallbackGroup::SharedPtr cb_group1;
    rclcpp::CallbackGroup::SharedPtr cb_group2;

    void timer1_callback()
    {        
        std::this_thread::sleep_for(2000ms);
        RCLCPP_INFO(this->get_logger(), "cb 1");
    }
    
    void timer2_callback()
    {
        std::this_thread::sleep_for(2000ms);
        RCLCPP_INFO(this->get_logger(), "cb 2");
    }

    void timer3_callback()
    {
        std::this_thread::sleep_for(2000ms);
        RCLCPP_INFO(this->get_logger(), "cb 3");
    }
};


class MultiThreadExecutorNode2 : public rclcpp::Node
{
public:
    MultiThreadExecutorNode2() : rclcpp::Node("multi_thread_executor_node_2")
    {
        cb_group = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);        
        timer4 = this->create_wall_timer(
            1000ms, std::bind(&MultiThreadExecutorNode2::timer4_callback, this), cb_group);
        timer5 = this->create_wall_timer(
            1000ms, std::bind(&MultiThreadExecutorNode2::timer5_callback, this), cb_group);
        RCLCPP_INFO(this->get_logger(), "Node 2 has been started");
    }

private:
    rclcpp::TimerBase::SharedPtr timer4;
    rclcpp::TimerBase::SharedPtr timer5;

    rclcpp::CallbackGroup::SharedPtr cb_group;

    void timer4_callback()
    {        
        std::this_thread::sleep_for(2000ms);
        RCLCPP_INFO(this->get_logger(), "cb 4");
    }

    void timer5_callback()
    {
        std::this_thread::sleep_for(2000ms);
        RCLCPP_INFO(this->get_logger(), "cb 5");
    }
};


int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node1 = std::make_shared<MultiThreadExecutorNode1>();
    auto node2 = std::make_shared<MultiThreadExecutorNode2>();
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node1);
    executor.add_node(node2);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}