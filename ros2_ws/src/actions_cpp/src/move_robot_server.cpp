#include <queue>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_robot_interfaces/action/move_robot.hpp"

using MoveRobot = my_robot_interfaces::action::MoveRobot;
using MoveRobotGoalHandle = rclcpp_action::ServerGoalHandle<MoveRobot>;
using namespace std::placeholders;

class MoveRobotServerNode : public rclcpp::Node
{
public:

    MoveRobotServerNode(): rclcpp::Node("move_robot_server_node") 
    {
        goal_queue_thread = std::thread(&MoveRobotServerNode::run_goal_queue_thread, this);        
        cb_group = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);
        current_position = 50;
        move_robot_server = rclcpp_action::create_server<MoveRobot>(
            this,
            "move_robot",
            std::bind(&MoveRobotServerNode::goal_callback, this, _1, _2),
            std::bind(&MoveRobotServerNode::cancel_callback, this, _1),
            std::bind(&MoveRobotServerNode::handle_accepted_callback, this, _1),
            rcl_action_server_get_default_options(),
            cb_group
        );
        RCLCPP_INFO(this->get_logger(), "move_robot action server has been started");
    }


    ~MoveRobotServerNode()
    {
        goal_queue_thread.join();
    }


private:    
    static constexpr int MIN_SCOPE = 0;
    static constexpr int MAX_SCOPE = 100;
    static constexpr int MAX_VELOCITY = 30;
    int current_position;
    rclcpp_action::Server<MoveRobot>::SharedPtr move_robot_server;
    std::shared_ptr<MoveRobotGoalHandle> goal_handle;
    std::queue<std::shared_ptr<MoveRobotGoalHandle>> goal_queue;
    rclcpp::CallbackGroup::SharedPtr cb_group;   
    std::mutex mutex;
    std::thread goal_queue_thread;


    rclcpp_action::GoalResponse goal_callback(
        const rclcpp_action::GoalUUID &uuid,
        std::shared_ptr<const MoveRobot::Goal> goal)
    {
        (void)uuid;
        int target_position = goal->position;
        int velocity = goal->velocity;
        RCLCPP_INFO(this->get_logger(), "Goal received");

        if(target_position > MAX_SCOPE || target_position < MIN_SCOPE) {
            RCLCPP_INFO(this->get_logger(), "Invalid target position");
            return rclcpp_action::GoalResponse::REJECT;
        }
        if(velocity <= 0 || velocity > MAX_VELOCITY) {
            RCLCPP_INFO(this->get_logger(), "Invalid velocity value");
            return rclcpp_action::GoalResponse::REJECT;
        }

        RCLCPP_INFO(this->get_logger(), "Goal accepted");
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }


    void handle_accepted_callback(const std::shared_ptr<MoveRobotGoalHandle> goal_handle)
    {
        std::lock_guard<std::mutex> lock(mutex);
        goal_queue.push(goal_handle);
        RCLCPP_INFO(this->get_logger(), "Adding new goal to the queue");
        RCLCPP_INFO(this->get_logger(), "Queue size: %d", (int)goal_queue.size());
    }


    rclcpp_action::CancelResponse cancel_callback(
        const std::shared_ptr<MoveRobotGoalHandle> goal_handle)
    {
        (void)goal_handle;
        return rclcpp_action::CancelResponse::ACCEPT;
    }


    void execute_goal(std::shared_ptr<MoveRobotGoalHandle> goal_handle)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            this->goal_handle = goal_handle;
        }
        rclcpp::Rate loop_rate(1.0);
        int target_position = goal_handle->get_goal()->position;
        int velocity = goal_handle->get_goal()->velocity;
        auto feedback = std::make_shared<MoveRobot::Feedback>();
        auto result = std::make_shared<MoveRobot::Result>();

        while(current_position != target_position) {
            if(goal_handle->is_canceling()) {
                result->position = current_position;
                result->message = "Mission canceled";
                goal_handle->canceled(result);
                return;
            }
            if(current_position < target_position) {
                current_position += velocity;
                if(current_position > target_position)
                    current_position = target_position;
            }
            else {
                current_position -= velocity;
                if(current_position < target_position)
                    current_position = target_position;
            }
            loop_rate.sleep();
            feedback->current_position = current_position;
            goal_handle->publish_feedback(feedback);
            RCLCPP_INFO(this->get_logger(), "Current position: %d", current_position);
        }
        result->position = current_position;
        result->message = "Mission accomplished";
        goal_handle->succeed(result);
    }


    void run_goal_queue_thread()
    {
        rclcpp::Rate loop_rate(1000.0);
        while(rclcpp::ok()) {
            std::shared_ptr<MoveRobotGoalHandle> next_goal;
            {
                std::lock_guard<std::mutex> lock(mutex);
                if(goal_queue.size() > 0) {
                    next_goal = goal_queue.front();
                    goal_queue.pop();
                }
            }
            if(next_goal) {
                RCLCPP_INFO(this->get_logger(), "Executing next goal from the queue");
                execute_goal(next_goal);                
            }
            loop_rate.sleep();
        }
    }
};


int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MoveRobotServerNode>();
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}