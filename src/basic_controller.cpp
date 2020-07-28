#include <iostream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Spawn.h"


class TurtleController
{
private:
    //The current nodeHandles
    //this-> = ros.NodeHandle();
    ros::NodeHandle n; 
    //initialising the publisher
    ros::Publisher cmd_vel_pub;

    float lin_speed;
    float ang_speed;

    //the name of the turtle we are controlling
    std::string turtle_name;

    geometry_msgs::Twist calculateCommand()
    {
        // creating a message of type  geometry_msgs::Twist()
        auto msg = geometry_msgs::Twist();
        msg.linear.x = this->lin_speed; // the turtle can only move in forward or backward , if you want to change it to y, then turn left or right then again it becomes the x axis
        msg.angular.z = this->ang_speed; // the turrtle ionly have a rotaion around the z axis , ie; normal to the plane
        return msg;
    }

    void spawn_turtle()
    {
        // create a service client
        ros::ServiceClient client = n.serviceClient<turtlesim::Spawn>("spawn");

        //call the service
        turtlesim::Spawn srv;
        srv.request.x = 5.0;
        srv.request.y = 5.0;
        srv.request.theta = 1;
        srv.request.name = this->turtle_name;
        client.call(srv);

    }

public:
    TurtleController(){
        // Initialize ROS
        this->n = ros::NodeHandle();

        //ros::NodeHandle n;

        //initialiase private ROS Nodehandle to change the parameters of the linear and andular velocities
        ros::NodeHandle nh("~");

        //read private parametrs from the launch file
        nh.param<float>("linear_speed", this->lin_speed, 1.0);
        nh.param<float>("angular_speed", this->ang_speed, 1.0);
        nh.param<std::string>("spawn_turtle_name", this->turtle_name, "");

        if (this->turtle_name.length()>1)
        {
            //give the turtle few seconds to wke up
            ros::Duration(5).sleep();
            this->spawn_turtle();

        }

        // Create a publisher object, able to push messages
        this->cmd_vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000); //1000 means we are creating a buffer for 1000 messages
    }

    void run(){
        // Send messages in a loop
        ros::Rate loop_rate(10);
        while (ros::ok())
        {
            auto msg = calculateCommand();

            this->cmd_vel_pub.publish(msg);
            ros::spinOnce();

            loop_rate.sleep(); // this is basically how we  defining the rate
        }
    }

};


int main(int argc, char **argv){
    // Initialize ROS
    ros::init(argc, argv, "talker");// talker-> is the name of the rosnode, it will be replaced by the name of the node in th launch file if any

    // Create our controller object and run it
    auto controller = TurtleController();
    controller.run();

    // And make good on our promise
    return 0;
}