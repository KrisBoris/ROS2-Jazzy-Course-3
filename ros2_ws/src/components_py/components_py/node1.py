#!/usr/bin/env python3

import time
from rclpy.node import Node


class Node1(Node):
    def __init__(self):
        super().__init__("multi_thread_executor_node_1")        
        self.timer1 = self.create_timer(
            1.0, self.timer1_callback)
        self.timer2 = self.create_timer(
            1.0, self.timer2_callback)
        self.timer3 = self.create_timer(
            1.0, self.timer3_callback)
            
        self.get_logger().info("multi_thread_executor node 1 has been started")


    def timer1_callback(self):
        time.sleep(2.0)
        self.get_logger().info("cb 1")


    def timer2_callback(self):
        time.sleep(2.0)
        self.get_logger().info("cb 2")


    def timer3_callback(self):
        time.sleep(2.0)
        self.get_logger().info("cb 3")