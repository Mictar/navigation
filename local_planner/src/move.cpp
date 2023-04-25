#include <memory>
#include <functional>
#include <cmath>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Point.h>
#include <ros/ros.h>

#include <complex>

#include <std_msgs/Bool.h>

#include <geometry_msgs/Pose2D.h>

using std::placeholders::_1;

class Move
{
	private:
		ros::NodeHandle node;

		ros::Subscriber robot_pose;

		ros::Subscriber goal;

		ros::Publisher cmd;
		
		// Publisher en 
		ros::Publisher goal_succe;

		geometry_msgs::Point current_goal;

		void robotPoseCallback(const geometry_msgs::Pose2D::ConstPtr& pose);

		void goalCallback(const geometry_msgs::Point::ConstPtr& goal);

		float kp = 0.5;
		bool etat = false;
	public:
		Move(std::string name);
};

Move::Move(std::string name="Move")
{
	current_goal.x = 0, current_goal.y = 0, current_goal.z = 0;
	ROS_INFO( "Demarrage du topic move");

	robot_pose = node.subscribe(
			"pose2d",
			10, 
			&Move::robotPoseCallback, 
			this);

	goal = node.subscribe(
			"move_goal",
			10,
			&Move::goalCallback,
		       	this);

	cmd =  node.advertise<geometry_msgs::Twist>("cmd_vel", 10);


	goal_succe = node.advertise<std_msgs::Bool>("goal_succe", 1);
}

void Move::robotPoseCallback(const geometry_msgs::Pose2D::ConstPtr& pose)
{
	//float new_x = current_goal.x - pose -> x;
	//float new_y = current_goal.y - pose -> y;
	std::complex<double> goal_point { current_goal.x - pose -> x,
					 current_goal.y - pose -> y};

	float angle_to_goal = std::arg( goal_point );

	auto cmd_vel = geometry_msgs::Twist();

	auto state = std_msgs::Bool();

	float diff_goal_x = std::abs(pose -> x - current_goal.x);
	float diff_goal_y = std::abs(pose -> y - current_goal.y);
	float diff_angle = angle_to_goal - pose -> theta ;
	
	//ROS_INFO( "%f, %f, %f", diff_goal_x, diff_goal_y, diff_angle);

	if( etat == true )
	{
		if ( diff_goal_x < 0.1 && diff_goal_y < 0.1)
		{
			cmd_vel.linear.x = 0.0;
			cmd_vel.angular.z = 0.0;
			state.data = true;		
			//auto succee = std_msgs::Bool();
			//succee.data = true;

			//goal_succe.publish( succee);
		}

		else if ( std::abs( diff_angle ) > 0.1 )
		{
			cmd_vel.linear.x = 0.0;
			cmd_vel.angular.z = kp * diff_angle;
			state.data = false;
		}
		else
		{
			cmd_vel.linear.x = 0.2;
			cmd_vel.angular.z = 0.0;
			state.data = false;
		}

		cmd.publish(cmd_vel);
		goal_succe.publish(state);
	}

}

void Move::goalCallback(const geometry_msgs::Point::ConstPtr& goal)
{
	ROS_INFO( "Current goal is : %f, %f, %f", goal -> x, goal -> y, goal -> z);

	current_goal = *goal;
	etat = true;

}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "Move");

	Move move;

	ros::spin(  );

	return 0;
}

		
