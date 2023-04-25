#include <ros/ros.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/PoseArray.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/Pose2D.h>
#include <std_msgs/Bool.h>


class NavPoints
{
	private:
		ros::NodeHandle node;

		ros::Subscriber goal_point;

		ros::Subscriber goal_state;

		ros::Publisher goal_move;

		ros::Rate rate {10};
	
		bool etat = false;
		
		std::vector<std::pair<double, double> > current_points;

		void pointsGaolCallback(const geometry_msgs::PoseArray::ConstPtr& pose_array)
		{
			std::vector< std::pair<double, double> > points;

			std::cout << " Current points\n";
			for( auto valeur: pose_array -> poses)
			{
				points.push_back( {valeur.position.x, valeur.position.y } );

				std::cout << valeur.position.x << "  " << valeur.position.y << " \n";

			}
			current_points = points;

			if ( !etat )
				move(), etat = true;


		}

		void move()
		{

				ROS_INFO(" Current point %f %f", current_points[0].first, current_points[0].second);

                                auto next_point = geometry_msgs::Point();
                                next_point.x = current_points[0].first;
                                next_point.y = current_points[0].second;
                                goal_move.publish(next_point);

                                rate.sleep();
                                current_points.erase( current_points.begin() );
		}

		void stateGaolCallback(const std_msgs::Bool::ConstPtr& state)
		{
			if ( ! current_points.empty() &&  state -> data)
			{
				ROS_INFO(" Current point %f %f", current_points[0].first, current_points[0].second);
			
				auto next_point = geometry_msgs::Point();
				next_point.x = current_points[0].first;
				next_point.y = current_points[0].second;
				goal_move.publish(next_point);

				rate.sleep();
				current_points.erase( current_points.begin() );
			}
			//else if ( ! etat )


			/*for(auto valeur: current_points)
			{
				ROS_INFO(" x : %f y: %f ", valeur.first, valeur.second);
			}*/
			
		}


	public:
		NavPoints()
		{
			ROS_INFO("Demarrage du navigateur par point");
			goal_point = node.subscribe(
					"goal_array",
					10,
					&NavPoints::pointsGaolCallback,
					this
					);

			goal_move = node.advertise<geometry_msgs::Point>("move_goal", 10);
			
			goal_state = node.subscribe(
				"goal_succe",
				10,
				&NavPoints::stateGaolCallback,
				this);	
		}

};

int main(int argc, char** argv)
{
	ros::init(argc, argv, "NavPoints");
	NavPoints nav;
	ros::spin();
	
	return 0;
}
