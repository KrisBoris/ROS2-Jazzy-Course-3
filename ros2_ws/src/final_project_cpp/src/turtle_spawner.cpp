#include "final_project_cpp/turtle_spawner.hpp"

namespace turtle_spawner {

TurtleSpawnerNode::TurtleSpawnerNode() : rclcpp::Node("turtle_spawner")
{
    turtle_spawn_client = this->create_client<turtlesim::srv::Spawn>("spawn_turtle");
    turtle_kill_client = this->create_client<turtlesim::srv::Kill>("kill_turtle");
    spawn_position.x = 5.0;
    spawn_position.y = 5.0;
    turtle_name = "";
}

void TurtleSpawnerNode::spawnTurtle()
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

void TurtleSpawnerNode::spawnTurtleServiceCallback(
        rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture future,
        turtlesim::srv::Spawn::Request::SharedPtr request)
{
    auto response = future.get();
    turtle_name = response->name;
    RCLCPP_INFO(this->get_logger(), "Turtle %s has been spawned at x: %.2f, y: %.2f", 
        turtle_name, request->x, request->y);
}

void TurtleSpawnerNode::killTurtle()
{
    while(!turtle_kill_client->wait_for_service(std::chrono::seconds(3)))
        RCLCPP_INFO(this->get_logger(), "Waiting for kill turtle service");

    auto request = std::make_shared<turtlesim::srv::Kill::Request>();
    request->name = turtle_name;

    turtle_kill_client->async_send_request(request,
        std::bind(&TurtleSpawnerNode::killTurtleServiceCallback, this, _1));
}

void TurtleSpawnerNode::killTurtleServiceCallback(
    rclcpp::Client<turtlesim::srv::Kill>::SharedFuture future)
{
    auto result = future.get();
    RCLCPP_INFO(this->get_logger(), "Turtle was killed. R.I.P.");
}

}   // namespace - turtle_spawner