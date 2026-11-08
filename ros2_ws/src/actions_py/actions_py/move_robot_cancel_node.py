#!/usr/bin/env python3

import time
import rclpy
from rclpy.node import Node
from std_msgs.msg import Bool


class MoveRobotCancelNode(Node):
    def __init__(self):
        super().__init__("move_robot_cancel_node")
        self.move_robot_cancel_pub = self.create_publisher(
            Bool,
            "cancel_robot_move",
            10,
        )        
        self.get_logger().info("move_robot_cancel node has been started")


    def cancel_goal(self):
        self.get_logger().info("Sending goal cancel request")
        cancel_msg = Bool()
        cancel_msg.data = True
        self.move_robot_cancel_pub.publish(cancel_msg)



def main(args=None):
    rclpy.init(args=args)
    node = MoveRobotCancelNode()
    time.sleep(0.1)
    node.cancel_goal()
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