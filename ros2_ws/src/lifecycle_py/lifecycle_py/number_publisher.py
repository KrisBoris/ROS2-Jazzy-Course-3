#!/usr/bin/env python3

import rclpy
from rclpy.lifecycle import LifecycleNode
from rclpy.lifecycle.node import LifecycleState, TransitionCallbackReturn
from example_interfaces.msg import Int64

class NumberPublisherNode(LifecycleNode):
    def __init__(self):
        super().__init__("number_publisher")
        self.get_logger().info("In number_publisher constructor")
        self.number = 1
        self.publish_frequency = 1.0
        self.number_pub = None
        self.number_timer = None


    # Create ROS2 communication, connect to hardware
    def on_configure(self, state: LifecycleState):
        self.get_logger().info("In on_configure")
        self.number_pub = self.create_publisher(Int64, "number", 10)
        self.number_timer = self.create_timer(
            1.0 / self.publish_frequency, self.publish_number)
        self.number_timer.cancel()   
        return TransitionCallbackReturn.SUCCESS

    # Activate/Enable hardware
    def on_activate(self, state: LifecycleState):
        self.get_logger().info("In on_activate")
        self.number_timer.reset()
        return super().on_activate(state)


    # Deactivate/disable hardware
    def on_deactivate(self, state: LifecycleState):
        self.get_logger().info("In on_deactivate")
        self.number_timer.cancel()
        return super().on_deactivate(state)
    

    # Destroy ROS2 communication, disconnect from hardware
    def on_cleanup(self, state: LifecycleState):
        self.get_logger().info("In on_cleanup")
        self.destroy_lifecycle_publisher(self.number_pub)
        self.destroy_timer(self.number_timer)
        return TransitionCallbackReturn.SUCCESS


    # Clean everything
    def on_shutdown(self, state: LifecycleState):
        self.get_logger().info("In on_shutdown")
        self.destroy_lifecycle_publisher(self.number_pub)
        self.destroy_timer(self.number_timer)
        return TransitionCallbackReturn.SUCCESS


    # Process error, deactivate + cleanup
    def on_error(self, state: LifecycleState):
        self.get_logger().info("In on_error")
        self.destroy_lifecycle_publisher(self.number_pub)
        self.destroy_timer(self.number_timer)
        # do some checks, if ok, then return SUCCESS, if not FAILURE
        return TransitionCallbackReturn.SUCCESS
                


    def publish_number(self):
        msg = Int64()
        msg.data = self.number        
        self.number_pub.publish(msg)        
        self.number += 1



def main(args=None):
    rclpy.init(args=args)
    node = NumberPublisherNode()
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