#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <tf/tf.h>



class Convert
{
	private:
		ros::NodeHandle node;
		ros::Subscriber sub;
		ros::Publisher pub;
		void odomCallback(const nav_msgs::Odometry::ConstPtr&);


	public:
		Convert(std::string, std::string);
};

Convert::Convert(std::string odom_topic="/odom", std::string cmdvel_topic="/pose2d")
{
	ROS_INFO("Demarrage du convertisseur quaternion -> euler ");
	sub = node.subscribe(
			odom_topic,
			10,
			&Convert::odomCallback, 
			this);

	pub = node.advertise<geometry_msgs::Pose2D>(cmdvel_topic, 10);

}

void Convert::odomCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	auto pose = geometry_msgs::Pose2D();

	pose.x = msg -> pose.pose.position.x;
	pose.y = msg -> pose.pose.position.y;

	tf::Quaternion q(
			msg -> pose.pose.orientation.x,
			msg -> pose.pose.orientation.y,
			msg -> pose.pose.orientation.z,
			msg -> pose.pose.orientation.w);
	tf::Matrix3x3 m(q);

	double roll, pitch, yaw;
	
	m.getRPY(roll, pitch, yaw);

	pose.theta = yaw;

	pub.publish(pose);
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "ConvertQ2E");
	Convert conv;

	ros::spin();

	return 0;
}
