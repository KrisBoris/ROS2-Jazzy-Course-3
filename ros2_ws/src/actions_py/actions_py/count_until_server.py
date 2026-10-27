#!/usr/bin/env python3

import time
import rclpy
from rclpy.node import Node
from rclpy.action import ActionServer, GoalResponse, CancelResponse
from rclpy.action.server import ServerGoalHandle
from my_robot_interfaces.action import CountUntil
from rclpy.executors import MultiThreadedExecutor
from rclpy.callback_groups import ReentrantCallbackGroup


class CountUntilServerNode(Node):

    def __init__(self):
        
        super().__init__("count_until_server_node")
        
        self.count_until_server = ActionServer(
            self,
            CountUntil,
            "count_until",            
            goal_callback=self.goal_callback,
            cancel_callback=self.cancel_callback,
            execute_callback=self.execute_callback,
            callback_group=ReentrantCallbackGroup())

        self.get_logger().info("Action server has been started")


    def goal_callback(self, goal_request: CountUntil.Goal):
        self.get_logger().info("Received the goal")
        if goal_request.target_number <= 0:
            self.get_logger().info("Rejecting the goal")
            return GoalResponse.REJECT
        self.get_logger().info("Accepting the goal")
        return GoalResponse.ACCEPT


    def execute_callback(self, goal_handle: ServerGoalHandle):
        target_number = goal_handle.request.target_number
        period = goal_handle.request.period
        feedback = CountUntil.Feedback()
        result = CountUntil.Result()

        self.get_logger().info("Executing the goal")

        counter = 0
        for i in range(target_number):
            if goal_handle.is_cancel_requested:
                self.get_logger().info("Canceling the goal")
                goal_handle.canceled()
                result.reached_number = counter
                return result
            counter += 1
            feedback.current_number = counter
            goal_handle.publish_feedback(feedback)
            self.get_logger().info(str(counter))
            time.sleep(period)
        
        # goal_handle.succeed()
        goal_handle.abort()
        
        result.reached_number = counter
        return result


    def cancel_callback(self, goal_handle: ServerGoalHandle):
        self.get_logger().info("Received the cancel request")
        return CancelResponse.ACCEPT



def main(args=None):
    rclpy.init(args=args)
    node = CountUntilServerNode()
    try:
        rclpy.spin(node, MultiThreadedExecutor())
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()

if __name__ == "__main__":
    main()