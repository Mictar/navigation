#include <memory>
#include <functional>
#include <cmath>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Point.h>
#include <ros/ros.h>

#include <geometry_msgs/Pose2D.h>

using std::placeholders::_1;

class Move
{
	private:
		ros::NodeHandle node;

		ros::Subscriber robot_pose;

		ros::Subscriber goal;

		ros::Publisher cmd;

		geometry_msgs::Point current_goal;

		void robotPoseCallback(const geometry_msgs::Pose2D::ConstPtr& pose);

		void goalCallback(const geometry_msgs::Point::ConstPtr& goal);

		float kp = 0.5;
		bool start = false;
	public:
		Move(std::string name);
};

Move::Move(std::string name="Move")
{
	ROS_INFO( "Demarrage du topic move");

	robot_pose = node.subscribe(
			"pose2d",
			10, 
			&Move::robotPoseCallback, 
			this);

	goal = node.subscribe(
			"goal",
			10,
			&Move::goalCallback,
		       	this);

	cmd =  node.advertise<geometry_msgs::Twist>("cmd_vel", 10);

}

void Move::robotPoseCallback(const geometry_msgs::Pose2D::ConstPtr& pose)
{
	float new_x = current_goal.x - pose -> x;
	float new_y = current_goal.y - pose -> y;
	

	float angle_to_goal = atan2(new_y, new_x);

	auto cmd_vel = geometry_msgs::Twist();
	
	float diff_goal_x = std::abs(pose -> x - current_goal.x);
	float diff_goal_y = std::abs(pose -> y - current_goal.y);
	float diff_angle = angle_to_goal - pose -> theta ;
	
	ROS_INFO( "%f, %f, %f", diff_goal_x, diff_goal_y, diff_angle);

	if ( start )
	{
		if ( diff_goal_x < 0.1 && diff_goal_y )
		{
			cmd_vel.linear.x = 0.0;
			cmd_vel.angular.z = 0.0;
		}

		else if ( std::abs( diff_angle ) > 0.1 )
		{
			cmd_vel.linear.x = 0.0;
			cmd_vel.angular.z = kp * diff_angle;
		}
		else
		{
			cmd_vel.linear.x = 0.2;
			cmd_vel.angular.z = 0.0;
		}

		cmd.publish(cmd_vel);
	}

}

void Move::goalCallback(const geometry_msgs::Point::ConstPtr& goal)
{
	ROS_INFO( "Current goal is : %f, %f, %f", goal -> x, goal -> y, goal -> z);

	current_goal = *goal;

	if(! start)
	       start = true;
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "Move");

	Move move;

	ros::spin(  );

	return 0;
}

		
