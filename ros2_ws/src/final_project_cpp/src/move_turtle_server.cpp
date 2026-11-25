#include "final_project_cpp/move_turtle_server.hpp"


final_project_cpp::MoveTurtleServerNode::MoveTurtleServerNode(const rclcpp::NodeOptions &options) 
    : rclcpp_lifecycle::LifecycleNode("move_turtle_server", options)
{
    RCLCPP_INFO(this->get_logger(), "In constructor");
    cb_group = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);    
    is_server_activated = false;
}

final_project_cpp::MoveTurtleServerNode::~MoveTurtleServerNode()
{

}

LifecycleCallbackReturn final_project_cpp::MoveTurtleServerNode::on_configure(const rclcpp_lifecycle::State &previous_state)
{
    (void)previous_state;
    RCLCPP_INFO(this->get_logger(), "In on_configure");
    move_turtle_server = rclcpp_action::create_server<MoveTurtle>(
        this,
        "move_turtle",
        std::bind(&MoveTurtleServerNode::goal_callback, this, _1, _2),
        std::bind(&MoveTurtleServerNode::cancel_callback, this, _1),
        std::bind(&MoveTurtleServerNode::handle_accepted_callback, this, _1),
        rcl_action_server_get_default_options(),
        cb_group
    );
    turtle_spawn_client = this->create_client<turtlesim::srv::Spawn>("/spawn");
    turtle_kill_client = this->create_client<turtlesim::srv::Kill>("/kill");
    turtle_vel_pub = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
    spawnTurtle();
    return LifecycleCallbackReturn::SUCCESS;
}

LifecycleCallbackReturn final_project_cpp::MoveTurtleServerNode::on_activate(const rclcpp_lifecycle::State &previous_state)
{    
    RCLCPP_INFO(this->get_logger(), "In on_activate");    
    is_server_activated = true;
    rclcpp_lifecycle::LifecycleNode::on_activate(previous_state);
    return LifecycleCallbackReturn::SUCCESS;
}

LifecycleCallbackReturn final_project_cpp::MoveTurtleServerNode::on_deactivate(const rclcpp_lifecycle::State &previous_state)
{
    RCLCPP_INFO(this->get_logger(), "In on_deactivate");
    is_server_activated = false;
    rclcpp_lifecycle::LifecycleNode::on_deactivate(previous_state);
    return LifecycleCallbackReturn::SUCCESS;
}

LifecycleCallbackReturn final_project_cpp::MoveTurtleServerNode::on_cleanup(const rclcpp_lifecycle::State &previous_state)
{
    (void)previous_state;
    RCLCPP_INFO(this->get_logger(), "In on_cleanup");
    killTurtle();
    move_turtle_server.reset();
    turtle_spawn_client.reset();
    turtle_kill_client.reset();
    turtle_vel_pub.reset();
    return LifecycleCallbackReturn::SUCCESS;
}

LifecycleCallbackReturn final_project_cpp::MoveTurtleServerNode::on_shutdown(const rclcpp_lifecycle::State &previous_state)
{
    (void)previous_state;
    RCLCPP_INFO(this->get_logger(), "In on_shutdown");
    killTurtle();
    is_server_activated = false;
    move_turtle_server.reset();
    turtle_spawn_client.reset();
    turtle_kill_client.reset();
    turtle_vel_pub.reset();
    return LifecycleCallbackReturn::SUCCESS;
}

LifecycleCallbackReturn final_project_cpp::MoveTurtleServerNode::on_error(const rclcpp_lifecycle::State &previous_state)
{
    (void)previous_state;
    RCLCPP_INFO(this->get_logger(), "In on_error");
    killTurtle();
    is_server_activated = false;
    move_turtle_server.reset();
    turtle_spawn_client.reset();
    turtle_kill_client.reset();
    turtle_vel_pub.reset();
    return LifecycleCallbackReturn::SUCCESS;
}


rclcpp_action::GoalResponse final_project_cpp::MoveTurtleServerNode::goal_callback(
    const rclcpp_action::GoalUUID &uuid,
    const std::shared_ptr<const MoveTurtle::Goal> goal)
{
    (void)uuid;
    RCLCPP_INFO(this->get_logger(), "Received a goal");

    if(!is_server_activated){
        RCLCPP_WARN(this->get_logger(), "Server is not active");
        return rclcpp_action::GoalResponse::REJECT;
    }

    if(goal->linear_vel_x <= 0.0) {
        RCLCPP_WARN(this->get_logger(), "Linear velocity must be greater than zero");
        return rclcpp_action::GoalResponse::REJECT;
    }
    if(goal->angular_vel_z <= 0.0) {
        RCLCPP_WARN(this->get_logger(), "Angular velocity must be greater than zero");
        return rclcpp_action::GoalResponse::REJECT;
    }
    if(goal->duration <= 0.0) {
        RCLCPP_WARN(this->get_logger(), "Duration must be greater than zero");
        return rclcpp_action::GoalResponse::REJECT;
    }

    // Preempt the existing goal
    {
        std::lock_guard<std::mutex> lock(mutex);
        if(goal_handle) {
        if(goal_handle->is_active()) {
            RCLCPP_INFO(this->get_logger(), "Aborting current goal and accepting the new goal");
            preempted_goal_uuid = goal_handle->get_goal_id();
        }
    }
    }

    RCLCPP_INFO(this->get_logger(), "Accepting and executing the goal");
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

void final_project_cpp::MoveTurtleServerNode::handle_accepted_callback(const std::shared_ptr<MoveTurtleGoalHandle> &goal_handle)
{
    RCLCPP_INFO(this->get_logger(), "Executing the goal");
    execute_goal(goal_handle);
}

rclcpp_action::CancelResponse final_project_cpp::MoveTurtleServerNode::cancel_callback(const std::shared_ptr<MoveTurtleGoalHandle> goal_handle)
{
    (void)goal_handle;
    RCLCPP_INFO(this->get_logger(), "Received a cancel request");
    return rclcpp_action::CancelResponse::ACCEPT;
}

void final_project_cpp::MoveTurtleServerNode::execute_goal(std::shared_ptr<MoveTurtleGoalHandle> goal_handle)
{
    {
        std::lock_guard<std::mutex> lock(mutex);
        this->goal_handle = goal_handle;
    }
    double linear_vel = goal_handle->get_goal()->linear_vel_x;
    double angular_vel = goal_handle->get_goal()->angular_vel_z;
    double duration_sec = goal_handle->get_goal()->duration;

    auto result = std::make_shared<MoveTurtle::Result>();
    auto feedback = std::make_shared<MoveTurtle::Feedback>();

    auto start_time = now();
    auto duration = rclcpp::Duration(duration_sec, 0);
    rclcpp::Rate rate(10);
    
    while(rclcpp::ok()) {
        if(now() - start_time > duration) {            
            stopTurtle();
            RCLCPP_INFO(this->get_logger(), "Mission accomplished");
            result->success = true;
            goal_handle->succeed(result);
            return;
        }
        {
            std::lock_guard<std::mutex> lock(mutex);
            if(preempted_goal_uuid == goal_handle->get_goal_id()) {                
                stopTurtle();
                RCLCPP_INFO(this->get_logger(), "New goal arrived. Aborting current goal");
                result->success = false;
                goal_handle->abort(result);
                return;
        }
        }        
        if(goal_handle->is_canceling()) {
            stopTurtle();
            RCLCPP_INFO(this->get_logger(), "Goal was cancelled");
            result->success = false;
            goal_handle->canceled(result);
            return;
        }
        if(!is_server_activated) {
            stopTurtle();
            RCLCPP_INFO(this->get_logger(), "Server was deactivated. Aborting the goal");
            result->success = false;
            goal_handle->abort(result);
            return;
        }
        auto msg = geometry_msgs::msg::Twist();
        msg.linear.x = linear_vel;
        msg.angular.z = angular_vel;
        turtle_vel_pub->publish(msg);
        rate.sleep();
    }
}


void final_project_cpp::MoveTurtleServerNode::spawnTurtle()
{
    while(!turtle_spawn_client->wait_for_service(std::chrono::seconds(3)))
        RCLCPP_INFO(this->get_logger(), "Waiting for spawn turtle service");

    auto request = std::make_shared<turtlesim::srv::Spawn::Request>();
    request->x = spawn_position.x;
    request->y = spawn_position.y;

    turtle_spawn_client->async_send_request(
        request, [this, request](rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture future)
    {
        this->spawnTurtleServiceCallback(future, request);
    });
}

void final_project_cpp::MoveTurtleServerNode::spawnTurtleServiceCallback(
        rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture future,
        turtlesim::srv::Spawn::Request::SharedPtr request)
{
    auto response = future.get();
    turtle_name = response->name;
    RCLCPP_INFO(this->get_logger(), "Turtle %s has been spawned at x: %.2f, y: %.2f", 
        turtle_name.c_str(), request->x, request->y);
}

void final_project_cpp::MoveTurtleServerNode::killTurtle()
{
    while(!turtle_kill_client->wait_for_service(std::chrono::seconds(3)))
        RCLCPP_INFO(this->get_logger(), "Waiting for kill turtle service");

    auto request = std::make_shared<turtlesim::srv::Kill::Request>();
    request->name = turtle_name;

    turtle_kill_client->async_send_request(request,
        std::bind(&MoveTurtleServerNode::killTurtleServiceCallback, this, _1));
}

void final_project_cpp::MoveTurtleServerNode::killTurtleServiceCallback(
    rclcpp::Client<turtlesim::srv::Kill>::SharedFuture future)
{
    auto result = future.get();
    RCLCPP_INFO(this->get_logger(), "Turtle was killed. R.I.P.");
}

void final_project_cpp::MoveTurtleServerNode::stopTurtle()
{
    auto stop_msg = geometry_msgs::msg::Twist();
    stop_msg.linear.x = 0;
    stop_msg.angular.z = 0;
    turtle_vel_pub->publish(stop_msg);
}


#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(final_project_cpp::MoveTurtleServerNode)