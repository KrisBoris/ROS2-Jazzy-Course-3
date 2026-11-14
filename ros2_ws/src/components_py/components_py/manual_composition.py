#!/usr/bin/env python3

import rclpy
from rclpy.executors import MultiThreadedExecutor
from components_py.node1 import Node1
from components_py.node2 import Node2


def main(args=None):
    rclpy.init(args=args)
    node1 = Node1()
    node2 = Node2()
    executor = MultiThreadedExecutor()
    executor.add_node(node1)
    executor.add_node(node2)
    try:
        executor.spin()
    except KeyboardInterrupt:
        pass
    finally:
        node1.destroy_node()
        node2.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == "__main__":
    main()