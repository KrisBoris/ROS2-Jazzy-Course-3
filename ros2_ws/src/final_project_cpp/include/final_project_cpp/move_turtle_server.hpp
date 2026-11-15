#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "final_project_cpp/turtle_spawner.hpp"
#include "my_robot_interfaces/action/move_turtle.hpp"
#include <queue>

using LifecycleCallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;
using MoveTurtle = my_robot_interfaces::action::MoveTurtle;
using MoveTurtleGoalHandle = rclcpp_action::ServerGoalHandle<MoveTurtle>;

class MoveTurtleServerNode : public rclcpp_lifecycle::LifecycleNode
{
public:
    MoveTurtleServerNode(const rclcpp::NodeOptions &options);
    ~MoveTurtleServerNode();

private:
    rclcpp_action::Server<MoveTurtle>::SharedPtr move_turtle_server;
    std::shared_ptr<MoveTurtleGoalHandle> goal_handle;
    std::queue<std::shared_ptr<MoveTurtleGoalHandle>> goal_queue;

    LifecycleCallbackReturn on_configure(const rclcpp_lifecycle::State &previous_state);
    LifecycleCallbackReturn on_activate(const rclcpp_lifecycle::State &previous_state);
    LifecycleCallbackReturn on_deactivate(const rclcpp_lifecycle::State &previous_state);
    LifecycleCallbackReturn on_cleanup(const rclcpp_lifecycle::State &previous_state);
    LifecycleCallbackReturn on_shutdown(const rclcpp_lifecycle::State &previous_state);
    LifecycleCallbackReturn on_error(const rclcpp_lifecycle::State &previous_state);

    rclcpp_action::GoalResponse goal_callback(
        const rclcpp_action::GoalUUID &uuid,
        const std::shared_ptr<const MoveTurtle::Goal> goal);
    void handle_accepted_callback(const std::shared_ptr<MoveTurtleGoalHandle> &goal_handle);
    rclcpp_action::CancelResponse cancel_callback(const std::shared_ptr<MoveTurtleGoalHandle> goal_handle);
    void execute_goal(const std::shared_ptr<MoveTurtleGoalHandle> goal_handle);
    void run_goal_queue_thread();
};
