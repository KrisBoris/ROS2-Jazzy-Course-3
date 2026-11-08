#!/usr/bin/env python3

import threading
import time
import rclpy
from rclpy.node import Node
from rclpy.action import ActionServer, GoalResponse, CancelResponse
from rclpy.action.server import ServerGoalHandle
from my_robot_interfaces.action import MoveRobot
from rclpy.executors import MultiThreadedExecutor
from rclpy.callback_groups import ReentrantCallbackGroup


class MoveRobotServerNode(Node):
    
    def __init__(self):
        super().__init__("move_robot_server_node")
        self.MOVEMENT_SCOPE = {"min_scope": 0, "max_scope": 100}
        self.MAX_VELOCITY = 30
        self.current_position = 50
        self.goal_handle: ServerGoalHandle = None
        self.goal_lock = threading.Lock()        
        self.move_robot_server = ActionServer(
            self,
            MoveRobot,
            "move_robot",
            goal_callback=self.goal_callback,
            handle_accepted_callback=self.handle_accepted_callback,
            cancel_callback=self.cancel_callback,
            execute_callback=self.execute_callback,
            callback_group=ReentrantCallbackGroup(),
        )        
        self.get_logger().info("move_robot action server has been created")


    def goal_callback(self, goal_request: MoveRobot.Goal):
        self.get_logger().info("Goal received")
        
        if (goal_request.position > self.MOVEMENT_SCOPE["max_scope"] or
            goal_request.position < self.MOVEMENT_SCOPE["min_scope"]):
            self.get_logger().warn(f"Requested position outside the scope: {goal_request.position}")
            return GoalResponse.REJECT
        elif (goal_request.velocity <= 0 or
              goal_request.velocity > self.MAX_VELOCITY):
            self.get_logger().warn(f"Requested invalid velocity: f{goal_request.velocity}"
                                   f" (max velocity: {self.MAX_VELOCITY})")
            return GoalResponse.REJECT
        
        # Policy: preempt existing goal when receiving a new one
        with self.goal_lock:
            if self.goal_handle is not None and self.goal_handle.is_active:
                self.get_logger().info("Aborting current goal and accepting a new one")
                self.goal_handle.abort()

        self.get_logger().info("Goal accepted")
        return GoalResponse.ACCEPT


    def handle_accepted_callback(self, goal_handle: ServerGoalHandle):        
        goal_handle.execute()


    def cancel_callback(self, goal_handle: ServerGoalHandle):
        self.get_logger().info("Received the cancel request")
        return CancelResponse.ACCEPT


    def execute_callback(self, goal_handle: ServerGoalHandle):
        with self.goal_lock:
            self.goal_handle = goal_handle
        self.get_logger().info("Executing the goal")

        target_position = goal_handle.request.position
        velocity = goal_handle.request.velocity
        feedback = MoveRobot.Feedback()
        result = MoveRobot.Result()

        while not self.current_position == target_position:
            if not goal_handle.is_active:
                result.position = self.current_position
                result.message = "Aborted. New position was set."
                self.__release_goal_handle()
                return result
            if goal_handle.is_cancel_requested:
                self.get_logger().info("Canceling the goal")
                goal_handle.canceled()
                result.position = self.current_position
                result.message = "Canceled"
                self.__release_goal_handle()
                return result
                
            if self.current_position < target_position:
                self.current_position += velocity
                if self.current_position > target_position:
                    self.current_position = target_position
            else:
                self.current_position -= velocity
                if self.current_position < target_position:
                    self.current_position = target_position
            time.sleep(1.0)
            feedback.current_position = self.current_position
            goal_handle.publish_feedback(feedback)
            self.get_logger().info(f"Current position: {self.current_position}")

        goal_handle.succeed()
        self.__release_goal_handle()
        result.position = self.current_position
        result.message = "Target reached"
        return result            


    def __release_goal_handle(self):
        with self.goal_lock:
            self.goal_handle = None



def main(args=None):
    rclpy.init(args=args)
    node = MoveRobotServerNode()
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