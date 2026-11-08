#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from rclpy.action import ActionClient
from rclpy.action.client import ClientGoalHandle, GoalStatus
from my_robot_interfaces.action import MoveRobot


class MoveRobotActionClient(Node):
    def __init__(self):
        super().__init__("move_robot_action_client")
        self.move_robot_client = ActionClient(
            self,
            MoveRobot,
            "move_robot",
        )
        self.get_logger().info("move_robot action client node has been started")


    def send_goal(self, target_position, velocity):
        self.move_robot_client.wait_for_server()
        
        goal = MoveRobot.Goal()
        goal.position = target_position
        goal.velocity = velocity

        self.get_logger().info("Sending goal")
        self.move_robot_client. \
            send_goal_async(goal, feedback_callback=self.goal_feedback_callback). \
                add_done_callback(self.goal_response_callback)


    def goal_response_callback(self, future):
        self.goal_handle: ClientGoalHandle = future.result()        
        if self.goal_handle.accepted:        
            self.get_logger().info("Goal was accepted")
            self.goal_handle.get_result_async(). \
                add_done_callback(self.goal_result_callback)
        else:
            self.get_logger().info("Goal was rejected")


    def goal_result_callback(self, future):
        status = future.result().status
        result = future.result().result
        if status == GoalStatus.STATUS_SUCCEEDED:
            self.get_logger().info("Mission accomplished")
        elif status == GoalStatus.STATUS_ABORTED:
            self.get_logger().info("Mission aborted")
        elif status == GoalStatus.STATUS_CANCELED:
            self.get_logger().info("Mission canceled")
        self.get_logger().info(f"Reached position: {result.position}")


    def goal_feedback_callback(self, feedback_msg):
        current_position = feedback_msg.feedback.current_position
        self.get_logger().info(f"Feedback - current position: {current_position}")


    def cancel_goal(self):
        self.get_logger().info("Canceling the goal")
        self.goal_handle.cancel_goal_async()



def main(args=None):
    rclpy.init(args=args)
    node = MoveRobotActionClient()
    node.send_goal(66, 5)
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