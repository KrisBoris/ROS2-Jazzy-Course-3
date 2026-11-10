from launch import LaunchDescription
from launch_ros.actions import Node, LifecycleNode

def generate_launch_description():
    ld = LaunchDescription()

    move_robot_server_node_name_1 = "move_robot_server_node_1"
    move_robot_server_node_name_2 = "move_robot_server_node_2"
    move_robot_server_node_name_3 = "move_robot_server_node_3"

    action_server_name_1 = "move_robot_r2d2"
    action_server_name_2 = "move_robot_walle"
    action_server_name_3 = "move_robot_omnidroid"

    lifecycle_move_robot_server_1 = LifecycleNode(
        package="lifecycle_action_server_py",
        executable="lifecycle_move_robot_server",
        name=move_robot_server_node_name_1,
        namespace="",
        parameters = [
            {"action_server_name": action_server_name_1},
        ],
    )

    lifecycle_move_robot_server_2 = LifecycleNode(
            package="lifecycle_action_server_py",
            executable="lifecycle_move_robot_server",
            name=move_robot_server_node_name_2,
            namespace="",
            parameters = [
                {"action_server_name": action_server_name_2},
            ],
        )

    lifecycle_move_robot_server_3 = LifecycleNode(
            package="lifecycle_action_server_py",
            executable="lifecycle_move_robot_server",
            name=move_robot_server_node_name_3,
            namespace="",
            parameters = [
                {"action_server_name": action_server_name_3},
            ],
        )

    lifecycle_move_robot_node_manager = Node(
        package="lifecycle_action_server_py",
        executable="lifecycle_move_robot_node_manager",
        parameters=[
            {"managed_node_names": [move_robot_server_node_name_1,
                                     move_robot_server_node_name_2,
                                     move_robot_server_node_name_3],}
        ],
    )

    ld.add_action(lifecycle_move_robot_server_1)
    ld.add_action(lifecycle_move_robot_server_2)
    ld.add_action(lifecycle_move_robot_server_3)
    ld.add_action(lifecycle_move_robot_node_manager)
    
    return ld