#include <ros/ros.h>
#include <local_planner/local_planner.hpp>

int main(int argc, char** argv)
{
	ros::init(argc, argv, "local_planner");
	LocalPlanner planner {"local_planner", std::stof(argv[1]), std::stof(argv[2])};

	ros::spin();

	return 0;
}
