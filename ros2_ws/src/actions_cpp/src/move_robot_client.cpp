#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_robot_interfaces/action/move_robot.hpp"
#include "std_msgs/msg/bool.hpp"

using MoveRobot = my_robot_interfaces::action::MoveRobot;
using MoveRobotGoalHandle = rclcpp_action::ClientGoalHandle<MoveRobot>;
using namespace std::placeholders;

class MoveRobotClientNode : public rclcpp::Node
{
public:
    MoveRobotClientNode(): rclcpp::Node("move_robot_client_node")
    {
        move_robot_client = rclcpp_action::create_client<MoveRobot>(this, "move_robot");
        cancel_robot_move_sub = this->create_subscription<std_msgs::msg::Bool>("cancel_goal", 10,
            std::bind(&MoveRobotClientNode::subscription_callback, this, _1));
        RCLCPP_INFO(this->get_logger(), "Action client has been started");
    }

    void send_goal(int target_position, int velocity)
    {
        move_robot_client->wait_for_action_server();

        auto goal = MoveRobot::Goal();
        goal.position = target_position;
        goal.velocity = velocity;

        auto options = rclcpp_action::Client<MoveRobot>::SendGoalOptions();
        options.goal_response_callback = 
            std::bind(&MoveRobotClientNode::response_callback, this, _1);
        options.result_callback = 
            std::bind(&MoveRobotClientNode::result_callback, this, _1);
        options.feedback_callback = 
            std::bind(&MoveRobotClientNode::feedback_callback, this, _1, _2);

        move_robot_client->async_send_goal(goal, options);
        RCLCPP_INFO(this->get_logger(), "Sending goal");
    }

private:
    rclcpp_action::Client<MoveRobot>::SharedPtr move_robot_client;
    MoveRobotGoalHandle::SharedPtr goal_handle;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr cancel_robot_move_sub;

    void response_callback(const MoveRobotGoalHandle::SharedPtr &goal_handle)
    {
        if(!goal_handle) {
            RCLCPP_INFO(this->get_logger(), "Goal got rejected");
        }
        else {
            this->goal_handle = goal_handle;
            RCLCPP_INFO(this->get_logger(), "Goal got accepted");
        }
    }

    void result_callback(const MoveRobotGoalHandle::WrappedResult &result)
    {
        auto status = result.code;
        if(status == rclcpp_action::ResultCode::SUCCEEDED) {
            RCLCPP_INFO(this->get_logger(), "Mission accomplished");
        }
        else if(status == rclcpp_action::ResultCode::ABORTED) {
            RCLCPP_INFO(this->get_logger(), "Mission aborted");
        }
        else if(status == rclcpp_action::ResultCode::CANCELED) {
            RCLCPP_INFO(this->get_logger(), "Mission canceled");
        }
        int reached_position = result.result->position;
        RCLCPP_INFO(this->get_logger(), "Reached position: %d", reached_position);
    }

    void feedback_callback(const MoveRobotGoalHandle::SharedPtr &goal_handle,
        const std::shared_ptr<const MoveRobot::Feedback> &feedback)
    {
        (void)goal_handle;
        int current_position = feedback->current_position;
        RCLCPP_INFO(this->get_logger(), "[Feedback] Current position: %d", current_position);
    }

    void subscription_callback(std_msgs::msg::Bool msg)
    {
        (void)msg;
        RCLCPP_INFO(this->get_logger(), "Canceling goal");
        move_robot_client->async_cancel_goal(goal_handle);
    }
};


int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MoveRobotClientNode>();
    node->send_goal(66, 3);
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}