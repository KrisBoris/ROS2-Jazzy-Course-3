#include "rclcpp/rclcpp.hpp"
#include "components_cpp/number_publisher.hpp"


namespace my_namespace {

NumberPublisher::NumberPublisher(
    const rclcpp::NodeOptions &options): rclcpp::Node("number_publisher", options)
{        
    number = 1;
    publish_frequency = 1.0;
    number_pub = this->create_publisher<Int64>("number", 10);
    timer = this->create_wall_timer(std::chrono::duration<double>(1.0 / publish_frequency),
        std::bind(&NumberPublisher::publish_number, this));
}

void NumberPublisher::publish_number()
{
    auto msg = Int64();
    msg.data = number;
    number_pub->publish(msg);
    ++number;
}

}   // namespace my_namespace

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(my_namespace::NumberPublisher)