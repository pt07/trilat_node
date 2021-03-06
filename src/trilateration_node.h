#ifndef TRILATERATION_NODE_H
#define TRILATERATION_NODE_H

#include <ros/ros.h>

#include <visualization_msgs/Marker.h>
#include "trilateration/satMeasurement.h"
#include "trilateration/satMeasurementArray.h"
#include "trilateration/receiver.h"

#include "../include/trilateration/src/trilateration.h"
#include "../include/trilateration/src/structs.h"


class TrilaterationNode
{
protected:
	// Trilateration object
	Trilateration tr;

	// Result of the trilateration
	Receiver estReceiver;

	// Received measurements
	std::vector<SatelliteMeasurement> measurements;


	// ROS node handle
	ros::NodeHandle nh;

	// Subscriber (measurements)
	ros::Subscriber measurementSub;

	// Publisher (markers)
	ros::Publisher markerPub;

	// Publisher (estimated receiver)
	ros::Publisher estRecPub;

public:
	TrilaterationNode();
	~TrilaterationNode();

	void measurementsCallback(const trilateration::satMeasurementArray::ConstPtr &msg);

protected:
	void process();

	void publishEstReceiver();
	void publishSatellites();
};

#endif // TRILATERATION_NODE_H
