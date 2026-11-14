#!/usr/bin/env python3

import time
from rclpy.node import Node


class Node2(Node):
    def __init__(self):
        super().__init__("multi_thread_executor_node_2")
        self.timer4 = self.create_timer(
            1.0, self.timer4_callback)
        self.timer5 = self.create_timer(
            1.0, self.timer5_callback)
        self.get_logger().info("multi_thread_executor node 2 has been started")


    def timer4_callback(self):
        time.sleep(2.0)
        self.get_logger().info("cb 4")


    def timer5_callback(self):
        time.sleep(2.0)
        self.get_logger().info("cb 5")
