#!/usr/bin/env python3

import time
import rclpy
from rclpy.node import Node
from lifecycle_msgs.srv import ChangeState
from lifecycle_msgs.msg import Transition


class LifecycleNodeManagerNode(Node):
    def __init__(self):
        super().__init__("lifecycle_node_manager")
        self.declare_parameter("managed_node_name", rclpy.Parameter.Type.STRING)
        node_name = self.get_parameter("managed_node_name").value
        service_change_state_name = "/" + node_name + "/change_state"
        self.client = self.create_client(ChangeState, service_change_state_name)


    def change_state(self, transition: Transition):
        self.client.wait_for_service()
        request = ChangeState.Request()
        request.transition = transition
        future = self.client.call_async(request)
        rclpy.spin_until_future_complete(self, future)


    def initialization_sequence(self):
        # Unconfigured to inactive
        self.get_logger().info("Trying to switch to configuring")
        transition = Transition()
        transition.id = Transition.TRANSITION_CONFIGURE
        transition.label = "configure"
        self.change_state(transition)
        self.get_logger().info("Configuring OK, now in inactive state")

        # sleep for the example
        time.sleep(3.0)

        # Inactive to active
        self.get_logger().info("Trying switch to active")
        transition.id = Transition.TRANSITION_ACTIVATE
        transition.label = "activate"
        self.change_state(transition)
        self.get_logger().info("Activating OK, now in active state")



def main(args=None):
    rclpy.init(args=args)
    node = LifecycleNodeManagerNode()
    node.initialization_sequence()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == "__main__":
    main()