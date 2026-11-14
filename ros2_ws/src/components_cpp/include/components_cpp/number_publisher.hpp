#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/msg/int64.hpp"


namespace my_namespace {

using Int64 = example_interfaces::msg::Int64;


class NumberPublisher : public rclcpp::Node
{
public:
    NumberPublisher(const rclcpp::NodeOptions &options);    

private:
    rclcpp::Publisher<Int64>::SharedPtr number_pub;
    rclcpp::TimerBase::SharedPtr timer;
    int number;
    double publish_frequency;

    void publish_number();    
};

}   // namespace my_namespace
