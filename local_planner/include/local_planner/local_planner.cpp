#include <local_planner/local_planner.hpp>

LocalPlanner::LocalPlanner(std::string name, float x, float y): act(node, name, boost::bind(&LocalPlanner::execute, this, _1), false)
{
	act.start();

	odom = node.subscribe("odom", 1000, &LocalPlanner::odomCallback, this );
	
	pub_cmd = node.advertise<geometry_msgs::Twist>("cmd_vel", 10);

	run( std::complex<float>(x, y) );
	//ROS_INFO("Demarrage du planificateur local");
}

void LocalPlanner::execute(const robot_msgs::NavGoal::ConstPtr & goal)
{

}

void LocalPlanner::odomCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	/*ROS_INFO("pose : %f %f %f", 
			msg -> pose.pose.position.x,
		       	msg->pose.pose.position.y, 
			msg->pose.pose.position.z);
	 ROS_INFO("orientation : %f, %f, %f", 
			msg -> pose.pose.orientation.y,
			msg -> pose.pose.orientation.z,
			msg -> pose.pose.orientation.w);*/

	tf::Quaternion q(
        msg->pose.pose.orientation.x,
        msg->pose.pose.orientation.y,
        msg->pose.pose.orientation.z,
        msg->pose.pose.orientation.w);
    	
	tf::Matrix3x3 m(q);
    	
	double roll, pitch, yaw;
    	
	m.getRPY(roll, pitch, yaw);
	
	pose_x = msg -> pose.pose.position.x;
	pose_y = msg -> pose.pose.position.y;
	angle = yaw;

	//ROS_INFO(" pose : %f %f orientation : %f", pose_x, pose_y,  yaw);
}

std::pair<float, float> LocalPlanner::move(std::complex<float> goal, float preci_angle=0.05, float preci_goal=0.1, float rotate_const=0.3)
{
	/*
	 * fonction de regulation de la trajectoire du but
	 */

	float angle_goal = std::arg(goal);
	
	float regulator = rotate_const * (angle_goal - angle);
	
	ROS_INFO(" x : %f, y : %f", goal.real(), goal.imag() );

	if (std::abs(goal.real() ) < preci_goal && std::abs(goal.imag() ) < preci_goal)	
	{
		return {0.0, 0.0};
	}
	
	else if (std::abs(regulator) < preci_angle)
	{
		return {vitesse, 0.0};
	}
	
	else
		return {0.0, regulator};

}
void LocalPlanner::run(std::complex<float> goal) 
{
	ros::Rate rate(10);

	auto msg = geometry_msgs::Twist();
	while(true)
	{


		std::complex<float> goal_pose = {goal.real() - pose_x, goal.imag() - pose_y};
		auto cmd_vel = move(goal_pose);
		if ( cmd_vel.first == 0.0 && cmd_vel.second == 0.0)
			break;

		//ROS_INFO("moving robot : cmd_vel linear: %f cmd_vel angular: %f", cmd_vel.first, cmd_vel.second);

		msg.linear.x = cmd_vel.first;
		msg.angular.z = cmd_vel.second;
		pub_cmd.publish(msg);
		ros::spinOnce();
		rate.sleep();
	}

	//ROS_INFO("Stop robot");
}
		
/*void LocalPlanner::rotation(double angle)
{
	while(true);
	ROS_INFO(" position: %f %f, orientation: %f", pose_x, pose_y, angle);
}
void LocalPlanner::acceleration(double distance)
{

}*/
