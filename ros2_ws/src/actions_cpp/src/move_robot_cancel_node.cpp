#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"

using Bool = std_msgs::msg::Bool;

class MoveRobotCancelNode : public rclcpp::Node
{
public:
    MoveRobotCancelNode(): rclcpp::Node("move_robot_cancel_node")
    {
        cancel_goal_pub = this->create_publisher<Bool>("cancel_goal", 10);
        timer = this->create_wall_timer(std::chrono::duration<double>(0.5), 
            std::bind(&MoveRobotCancelNode::timer_callback, this));
    }

    void cancel_goal()
    {
        RCLCPP_INFO(this->get_logger(), "Sending cancel goal request");
        auto msg = Bool();
        msg.data = true;
        cancel_goal_pub->publish(msg);
    }

private:
    rclcpp::Publisher<Bool>::SharedPtr cancel_goal_pub;
    rclcpp::TimerBase::SharedPtr timer;

    void timer_callback()
    {
        cancel_goal();
        timer->cancel();
    }
};


int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MoveRobotCancelNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}