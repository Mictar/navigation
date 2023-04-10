#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>

#include <robot_msgs/NavAction.h>
//#include <functional>
#include <nav_msgs/Odometry.h>
#include <tf/tf.h>
#include <complex>
#include <tuple>


class LocalPlanner
{
	private:
		ros::NodeHandle node;
		
		// definition d'un action server
		actionlib::SimpleActionServer<robot_msgs::NavAction> act;

		// definition du fonction callback pour le topic odom
		void  odomCallback(const nav_msgs::Odometry::ConstPtr& msg);
		
		// definition d'un subscriber pour le topic odom
		ros::Subscriber odom;

		// fonction de rotation d'un angle precis
		//void rotation(double angle);

		// fonction d'acceleration d'une distance precisse
		//void acceleration(double distance);

		std::pair<float, float> move( std::complex<float>, float, float, float );
		// definition du position et orientation du robot

		void run(std::complex<float>);

		double angle, pose_x, pose_y;
		const float vitesse = 0.06;
		

		// definition d'un publisher sur le topic cmd_vel
		ros::Publisher pub_cmd;

		void execute(const robot_msgs::NavGoal::ConstPtr& goal);
	public:
		LocalPlanner(std::string name, float, float);
};

