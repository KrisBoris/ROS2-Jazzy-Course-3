#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "turtlesim/srv/spawn.hpp"
#include "turtlesim/srv/kill.hpp"
#include "my_robot_interfaces/action/move_turtle.hpp"
#include "geometry_msgs/msg/twist.hpp"

using LifecycleCallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;
using MoveTurtle = my_robot_interfaces::action::MoveTurtle;
using MoveTurtleGoalHandle = rclcpp_action::ServerGoalHandle<MoveTurtle>;
using namespace std::placeholders;

namespace final_project_cpp {

struct turtleSpawnPosition {
    float x;
    float y;
};

class MoveTurtleServerNode : public rclcpp_lifecycle::LifecycleNode
{
public:
    MoveTurtleServerNode(const rclcpp::NodeOptions &options);
    ~MoveTurtleServerNode();

private:
    rclcpp_action::Server<MoveTurtle>::SharedPtr move_turtle_server;
    bool is_server_activated;
    std::shared_ptr<MoveTurtleGoalHandle> goal_handle;
    rclcpp_action::GoalUUID preempted_goal_uuid;
    rclcpp::CallbackGroup::SharedPtr cb_group;
    std::mutex mutex;
    
    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr turtle_spawn_client;
    rclcpp::Client<turtlesim::srv::Kill>::SharedPtr turtle_kill_client;
    turtleSpawnPosition spawn_position;
    std::string turtle_name;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr turtle_vel_pub;

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
    
    void spawnTurtle();
    void spawnTurtleServiceCallback(
        rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture future,
        turtlesim::srv::Spawn::Request::SharedPtr request);    
    void killTurtle();
    void killTurtleServiceCallback(rclcpp::Client<turtlesim::srv::Kill>::SharedFuture future);
    void stopTurtle();
};
}   // namespace final_project_cpp