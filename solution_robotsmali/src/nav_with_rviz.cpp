#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Point.h>


class NavRviz
{
	private:
		ros::NodeHandle node;
		ros::Publisher pub_goal;
		ros::Subscriber subs_rviz_goal;
		void rvizGoalCallback(const geometry_msgs::PoseStamped::ConstPtr& pose_map)
		{
			auto pose = geometry_msgs::Point();
			pose.x = pose_map -> pose.position.x;
			pose.y = pose_map -> pose.position.y;

			pub_goal.publish(pose);
		}
	public:
		NavRviz( std::string goal_topic="move_goal",
			 std::string rviz_goal_topic="move_base_simple/goal") {
			
			ROS_INFO("Demarrage du server de navigation par rviz");
			pub_goal = node.advertise<geometry_msgs::Point>(
					goal_topic,
					10);
			subs_rviz_goal = node.subscribe(
					rviz_goal_topic,
					10,
					&NavRviz::rvizGoalCallback, this
					);

		}
};

int main(int argc, char** argv)
{
	ros::init(argc, argv, "RvizGoalSender");
	NavRviz nav;

	ros::spin();

	return 0;
}
