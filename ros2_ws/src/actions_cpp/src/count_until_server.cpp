#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_robot_interfaces/action/count_until.hpp"

using CountUntil = my_robot_interfaces::action::CountUntil;
using CountUntilGoalHandle = rclcpp_action::ServerGoalHandle<CountUntil>;
using namespace std::placeholders;

class CountUntilServerNode : public rclcpp::Node
{
public:
    CountUntilServerNode(): rclcpp::Node("count_until_server_node") 
    {
        cb_group = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);
        count_until_server = rclcpp_action::create_server<CountUntil>(
            this,
            "count_until",
            std::bind(&CountUntilServerNode::goal_callback, this, _1, _2),
            std::bind(&CountUntilServerNode::cancel_callback, this, _1),
            std::bind(&CountUntilServerNode::handle_accepted_callback, this, _1),
            rcl_action_server_get_default_options(),
            cb_group
        );
        RCLCPP_INFO(this->get_logger(), "Action server has been created");
    }

private:
    rclcpp_action::Server<CountUntil>::SharedPtr count_until_server;
    std::shared_ptr<CountUntilGoalHandle> goal_handle;
    rclcpp_action::GoalUUID preemted_goal_id;
    rclcpp::CallbackGroup::SharedPtr cb_group;
    std::mutex mutex;

    rclcpp_action::GoalResponse goal_callback(
        const rclcpp_action::GoalUUID &uuid,
        std::shared_ptr<const CountUntil::Goal> goal) 
    {
        (void)uuid;        
        RCLCPP_INFO(this->get_logger(), "Received a goal");

        // Reject a new goal if one is already running
        // {
        //     std::lock_guard<std::mutex> lock(mutex);
        //     if(goal_handle) {
        //         if(goal_handle->is_active()) {
        //             RCLCPP_INFO(this->get_logger(), "Goal is already active. Rejecting new goal");
        //             return rclcpp_action::GoalResponse::REJECT;
        //         }
        //     }
        // }

        if(goal->target_number <= 0) {
            RCLCPP_INFO(this->get_logger(), "Rejecting the goal");
            return rclcpp_action::GoalResponse::REJECT;
        }

        // Preempt the existing goal when receiving a new valid goal
        {
            std::lock_guard<std::mutex> lock(mutex);
            if(goal_handle) {
                if(goal_handle->is_active()) {
                    RCLCPP_INFO(this->get_logger(), "Abort current goal and accept new goal");
                    preemted_goal_id = goal_handle->get_goal_id();
                }
            }
        }

        RCLCPP_INFO(this->get_logger(), "Executing the goal");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse cancel_callback(
        const std::shared_ptr<CountUntilGoalHandle> goal_handle)
    {
        (void)goal_handle;
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handle_accepted_callback(
        const std::shared_ptr<CountUntilGoalHandle> goal_handle)
    {
        execute_goal(goal_handle);
    }

    void execute_goal(std::shared_ptr<CountUntilGoalHandle> goal_handle)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            this->goal_handle = goal_handle;
        }        
        int target_number = goal_handle->get_goal()->target_number;
        double period = goal_handle->get_goal()->period;
        auto feedback = std::make_shared<CountUntil::Feedback>();
        auto result = std::make_shared<CountUntil::Result>();

        int counter = 0;
        rclcpp::Rate loop_rate(1.0 / period);
        for(int i = 0; i < target_number; ++i) {
            {
                std::lock_guard<std::mutex> lock(mutex);
                if(goal_handle->get_goal_id() == preemted_goal_id) {
                    result->reached_number = counter;
                    goal_handle->abort(result);
                    return;
            }
            }
            if(goal_handle->is_canceling()) {
                result->reached_number = counter;
                goal_handle->canceled(result);
                return;
            }
            ++counter;
            feedback->current_number = counter;
            goal_handle->publish_feedback(feedback);
            RCLCPP_INFO(this->get_logger(), "Current number: %d", counter);
            loop_rate.sleep();
        }
        
        result->reached_number = counter;
        goal_handle->succeed(result);
    }
};


int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CountUntilServerNode>();
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
    // rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}