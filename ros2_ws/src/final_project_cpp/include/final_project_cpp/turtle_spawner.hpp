#include "rclcpp/rclcpp.hpp"
#include "turtlesim/srv/spawn.hpp"
#include "turtlesim/srv/kill.hpp"
#include <chrono>

namespace turtle_spawner {

using namespace std::placeholders;


struct turtleSpawnPosition {
    float x;
    float y;
};


class TurtleSpawnerNode : public rclcpp::Node
{
public:
    TurtleSpawnerNode();

private:
    rclcpp::Client<turtlesim::srv::Spawn>::SharedPtr turtle_spawn_client;
    rclcpp::Client<turtlesim::srv::Kill>::SharedPtr turtle_kill_client;
    turtleSpawnPosition spawn_position;
    std::string turtle_name;

    void spawnTurtle();
    void spawnTurtleServiceCallback(
        rclcpp::Client<turtlesim::srv::Spawn>::SharedFuture future,
        turtlesim::srv::Spawn::Request::SharedPtr request);    
    void killTurtle();
    void killTurtleServiceCallback(
        rclcpp::Client<turtlesim::srv::Kill>::SharedFuture future);
};

}   // namespace - turtle_spawner