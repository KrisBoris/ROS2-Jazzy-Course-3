#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_robot_interfaces/action/count_until.hpp"

using CountUntil = my_robot_interfaces::action::CountUntil;
using CountUntilGoalHandle = rclcpp_action::ClientGoalHandle<CountUntil>;
using namespace std::placeholders;

class CountUntilClientNode : public rclcpp::Node
{
public:
    CountUntilClientNode() : rclcpp::Node("count_until_client_node")
    {        
        count_until_client = rclcpp_action::create_client<CountUntil>(this, "count_until");
        RCLCPP_INFO(this->get_logger(), "Action client has been started");
    }

    void send_goal(int target_number, double period)
    {
        count_until_client->wait_for_action_server();

        auto goal = CountUntil::Goal();
        goal.target_number = target_number;
        goal.period = period;

        auto options = rclcpp_action::Client<CountUntil>::SendGoalOptions();
        options.result_callback = 
            std::bind(&CountUntilClientNode::goal_result_callback, this, _1);
        options.goal_response_callback = 
            std::bind(&CountUntilClientNode::goal_response_callback, this, _1);
        options.feedback_callback = 
            std::bind(&CountUntilClientNode::feedback_callback, this, _1, _2);

        count_until_client->async_send_goal(goal, options);
        RCLCPP_INFO(this->get_logger(), "Sending goal");

        // timer = this->create_wall_timer(
        //     std::chrono::seconds(2),
        //     std::bind(&CountUntilClientNode::timer_callback, this));
    }

private:
    rclcpp_action::Client<CountUntil>::SharedPtr count_until_client;
    rclcpp::TimerBase::SharedPtr timer;
    CountUntilGoalHandle::SharedPtr goal_handle;    

    void goal_response_callback(const CountUntilGoalHandle::SharedPtr &goal_handle)
    {
        if(!goal_handle) {
            RCLCPP_INFO(this->get_logger(), "Goal got rejected");
        }
        else {
            this->goal_handle = goal_handle;
            RCLCPP_INFO(this->get_logger(), "Goal got accepted");
        }
    }

    void goal_result_callback(const CountUntilGoalHandle::WrappedResult &result)
    {
        auto status = result.code;
        if(status == rclcpp_action::ResultCode::SUCCEEDED) {
            RCLCPP_INFO(this->get_logger(), "Mission accomplished");
        }
        else if(status == rclcpp_action::ResultCode::ABORTED) {
            RCLCPP_WARN(this->get_logger(), "Mission aborted");
        }
        else if(status == rclcpp_action::ResultCode::CANCELED) {
            RCLCPP_WARN(this->get_logger(), "Mission canceled");
        }
        int reached_number = result.result->reached_number;
        RCLCPP_INFO(this->get_logger(), "Reached number: %d", reached_number);
    }

    void feedback_callback(const CountUntilGoalHandle::SharedPtr &goal_handle,
        const std::shared_ptr<const CountUntil::Feedback> &feedback)
    {
        (void)goal_handle;
        int current_number = feedback->current_number;
        RCLCPP_INFO(this->get_logger(), "Feedback: %d", current_number);
    }

    void timer_callback()
    {
        RCLCPP_INFO(this->get_logger(), "Canceling the goal");
        count_until_client->async_cancel_goal(goal_handle);
        timer->cancel();
    }
};


int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CountUntilClientNode>();    
    node->send_goal(6, 0.9);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}