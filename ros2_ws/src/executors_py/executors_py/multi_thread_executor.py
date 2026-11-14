#!/usr/bin/env python3

import time
import rclpy
from rclpy.node import Node
from rclpy.executors import MultiThreadedExecutor
from rclpy.callback_groups import ReentrantCallbackGroup, MutuallyExclusiveCallbackGroup


class MultiThreadExecutorNode(Node):
    def __init__(self):
        super().__init__("multi_thread_executor_node")
        self.cb_group1 = MutuallyExclusiveCallbackGroup()
        self.cb_group2 = MutuallyExclusiveCallbackGroup()
        self.cb_group3 = ReentrantCallbackGroup()
        self.timer1 = self.create_timer(
            1.0, self.timer1_callback, callback_group=self.cb_group1)
        self.timer2 = self.create_timer(
            1.0, self.timer2_callback, callback_group=self.cb_group2)
        self.timer3 = self.create_timer(
            1.0, self.timer3_callback, callback_group=self.cb_group3)
        self.get_logger().info("multi_thread_executor node has been started")


    def timer1_callback(self):
        time.sleep(2.0)
        self.get_logger().info("cb 1")


    def timer2_callback(self):
        time.sleep(2.0)
        self.get_logger().info("cb 2")


    def timer3_callback(self):
        time.sleep(2.0)
        self.get_logger().info("cb 3")



def main(args=None):
    rclpy.init(args=args)
    node = MultiThreadExecutorNode()
    executor = MultiThreadedExecutor()
    executor.add_node(node)
    try:
        executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == "__main__":
    main()