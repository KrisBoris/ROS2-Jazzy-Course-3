#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from rclpy.action import ActionClient
from rclpy.action.client import ClientGoalHandle, GoalStatus
from my_robot_interfaces.action import CountUntil


class CountUntilClientNode(Node):
    
    def __init__(self):
        super().__init__("count_until_client_node")
        self.count_until_client = ActionClient(
            self,
            CountUntil,
            "count_until")
        self.get_logger().info("Action client node has been started")


    def send_goal(self, target_number, period):
        self.count_until_client.wait_for_server()

        goal = CountUntil.Goal()
        goal.target_number = target_number
        goal.period = period

        self.get_logger().info("Sending goal")
        self.count_until_client. \
            send_goal_async(goal, feedback_callback=self.goal_feedback_callback). \
                add_done_callback(self.goal_response_callback)
        # self.timer = self.create_timer(3.0, self.cancel_goal)


    def cancel_goal(self):
        self.get_logger().info("Canceling the goal")
        self.goal_handle.cancel_goal_async()
        self.timer.cancel()


    def goal_response_callback(self, future):
        self.goal_handle: ClientGoalHandle = future.result()
        if self.goal_handle.accepted:
            self.get_logger().info("Goal got accepted")
            self.goal_handle. \
                get_result_async(). \
                    add_done_callback(self.goal_result_callback)
        else:
            self.get_logger().warn("Goal got rejected")


    def goal_result_callback(self, future):
        status = future.result().status
        result = future.result().result
        if status == GoalStatus.STATUS_SUCCEEDED:
            self.get_logger().info("Mission accomplished")
        elif status == GoalStatus.STATUS_ABORTED:
            self.get_logger().info("Mission aborted")
        elif status == GoalStatus.STATUS_CANCELED:
            self.get_logger().info("Mission canceledf")
        self.get_logger().info("Result: " + str(result.reached_number))


    def goal_feedback_callback(self, feedback_msg):
        number = feedback_msg.feedback.current_number
        self.get_logger().info("Received feedback: " + str(number))



def main(args=None):
    rclpy.init(args=args)
    node = CountUntilClientNode()
    node.send_goal(5, 1.0)    
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