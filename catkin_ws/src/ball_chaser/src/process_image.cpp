#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // Done: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("chasing ball request received"); 
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z =ang_z;
    if (!client.call(srv))
        ROS_ERROR("Failed to call service drive to target");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    ROS_INFO_STREAM("entered process image ");
    int white_pixel = 255;
    int left_pixel = img.width / 3; 
    int right_pixel = img.width - left_pixel; 
    
    // Done: Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < img.height * img.step; i+=3) {
	
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) {
	    ROS_INFO_STREAM(std::to_string((i % img.height)));
	    ROS_INFO_STREAM("Ball detected");
            // Then, identify if this pixel falls in the left, mid, or right side of the image
	    if ( (i % img.height) < left_pixel) {
		ROS_INFO_STREAM("Moving the robot to the left ");
		///turn left 
		drive_robot(0.0,0.5);
		break;
		
	    }
	    else if ( (i % img.height) > right_pixel ) {
	      /// turn right 
		ROS_INFO_STREAM("Moving the robot to the right ");
		drive_robot(0.0, -0.5);
		break;
	    }
	    else{
	    // go straight 
		
		ROS_INFO_STREAM("Moving the robot straight");
		drive_robot(0.5, 0.0);
		break;
	    } 
	}
	if (i == (img.height * img.step -1)) {
	     /// no ball detected 
	ROS_INFO_STREAM("Stop the robot");
	drive_robot( 0.0, 0.0); 
	}
    }
        
 
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
