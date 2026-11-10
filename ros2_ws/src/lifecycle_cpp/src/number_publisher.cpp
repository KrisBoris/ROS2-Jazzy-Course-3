#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "example_interfaces/msg/int64.hpp"

using LifecycleCallbackReturn = 
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;
using Int64 = example_interfaces::msg::Int64;

class NumberPublisherNode : public rclcpp_lifecycle::LifecycleNode
{
public:
    NumberPublisherNode(): rclcpp_lifecycle::LifecycleNode("number_publisher")
    {
        RCLCPP_INFO(this->get_logger(), "In constructor");
        number = 1;
        publish_frequency = 1.0;
    }

    LifecycleCallbackReturn on_configure(const rclcpp_lifecycle::State &previous_state)
    {
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(), "In on_configure");
        number_pub = this->create_publisher<Int64>("number", 10);
        timer = this->create_wall_timer(std::chrono::duration<double>(1.0 / publish_frequency),
            std::bind(&NumberPublisherNode::publish_number, this));
        timer->cancel();
        return LifecycleCallbackReturn::SUCCESS;        
    }

    LifecycleCallbackReturn on_activate(const rclcpp_lifecycle::State &previous_state)
    {        
        RCLCPP_INFO(this->get_logger(), "In on_activate");
        timer->reset();
        rclcpp_lifecycle::LifecycleNode::on_activate(previous_state);
        return LifecycleCallbackReturn::SUCCESS;
    }

    LifecycleCallbackReturn on_deactivate(const rclcpp_lifecycle::State &previous_state)
    {        
        RCLCPP_INFO(this->get_logger(), "In on_deactivate");
        timer->cancel();
        rclcpp_lifecycle::LifecycleNode::on_deactivate(previous_state);
        return LifecycleCallbackReturn::SUCCESS;
    }

    LifecycleCallbackReturn on_cleanup(const rclcpp_lifecycle::State &previous_state)
    {
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(), "In on_cleanup");
        number_pub.reset();
        timer.reset();
        return LifecycleCallbackReturn::SUCCESS;
    }

    LifecycleCallbackReturn on_shutdown(const rclcpp_lifecycle::State &previous_state)
    {
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(), "In on_shutdown");
        number_pub.reset();
        timer.reset();
        return LifecycleCallbackReturn::SUCCESS;
    }

    LifecycleCallbackReturn on_error(const rclcpp_lifecycle::State &previous_state)
    {
        (void)previous_state;
        RCLCPP_INFO(this->get_logger(), "In on_error");
        number_pub.reset();
        timer.reset();
        return LifecycleCallbackReturn::FAILURE;
    }

private:
    rclcpp::Publisher<Int64>::SharedPtr number_pub;
    rclcpp::TimerBase::SharedPtr timer;
    int number;
    double publish_frequency;

    void publish_number()
    {
        auto msg = Int64();
        msg.data = number;
        number_pub->publish(msg);
        ++number;
    }
};


int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberPublisherNode>();
    rclcpp::spin(node->get_node_base_interface());
    rclcpp::shutdown();
    return 0;
}