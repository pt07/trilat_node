#include "trilateration_node.h"


TrilaterationNode::TrilaterationNode() : nh(ros::this_node::getName())
{
	measurementSub = nh.subscribe("/gps_measurements", 1000, &TrilaterationNode::measurementsCallback, this);

	markerPub = nh.advertise<visualization_msgs::Marker>("/visualization_marker", 1000);

	estRecPub = nh.advertise<trilateration::receiver>("/estimatedReceiver", 1000);

}

TrilaterationNode::~TrilaterationNode() { }

void TrilaterationNode::measurementsCallback(const trilateration::satMeasurementArray::ConstPtr &msg)
{
	std::cout << "New measurement received";

	// Delete old measurements
	measurements.clear();

	for (int i = 0; i < msg->measurements.size(); ++i) {
		const trilateration::satMeasurement data = msg->measurements[i];

		//ROS_INFO_STREAM("x: " << data.x << "\ty: " << data.y << "\tz: " << data.z << "\tpseudorange: " << data.pseudorange);

		SatelliteMeasurement m = {
			Point<double>(data.x, data.y, data.z),
			data.pseudorange
		};

		measurements.push_back(m);
	}

	process();
}

void TrilaterationNode::process()
{
	estReceiver = tr.computePosition(measurements);
	std::cout << " ---> Estimation:\t" << estReceiver.toString() << std::endl;

	// Sets the guess for the next simulation in the actual position
	tr.setInitialReceiverGuess(estReceiver);

	publishSatellites();
	publishEstReceiver();
}

void TrilaterationNode::publishEstReceiver()
{
	visualization_msgs::Marker m;
	trilateration::receiver r;

	r.bias = estReceiver.bias;

	m.header.frame_id = "my_frame";
	m.header.stamp = ros::Time::now();

	// Set the namespace and id for this marker.  This serves to create a unique ID
	// Any marker sent with the same namespace and id will overwrite the old one
	m.ns = "receiver_est";
	m.id = 0;

	// Set the marker type.
	m.type = visualization_msgs::Marker::SPHERE;

	// Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
	m.action = visualization_msgs::Marker::ADD;

	// Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
	m.pose.position.x = r.x = estReceiver.pos.getX();
	m.pose.position.y = r.y = estReceiver.pos.getY();
	m.pose.position.z = r.z = estReceiver.pos.getZ();
	m.pose.orientation.x = 0.0;
	m.pose.orientation.y = 0.0;
	m.pose.orientation.z = 0.0;
	m.pose.orientation.w = 1.0;

	// Set the scale of the marker -- 1x1x1 here means 1m on a side
	m.scale.x = 1.0;
	m.scale.y = 1.0;
	m.scale.z = 1.0;

	// Set the color -- be sure to set alpha to something non-zero!
	m.color.r = 0.0f;
	m.color.g = 1.0f;
	m.color.b = 0.0f;
	m.color.a = 1.0;

	m.lifetime = ros::Duration();

	markerPub.publish(m);
	estRecPub.publish(r);
}

void TrilaterationNode::publishSatellites()
{
	for (int i = 0; i < measurements.size(); ++i) {
		visualization_msgs::Marker m;
		m.header.frame_id = "my_frame";
		m.header.stamp = ros::Time::now();

		// Set the namespace and id for this marker.  This serves to create a unique ID
		// Any marker sent with the same namespace and id will overwrite the old one
		m.ns = "satellite";
		m.id = i;

		// Set the marker type.
		m.type = visualization_msgs::Marker::CUBE;

		// Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
		m.action = visualization_msgs::Marker::ADD;

		// Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
		m.pose.position.x = measurements[i].pos.getX();
		m.pose.position.y = measurements[i].pos.getY();
		m.pose.position.z = measurements[i].pos.getZ();
		m.pose.orientation.x = 0.0;
		m.pose.orientation.y = 0.0;
		m.pose.orientation.z = 0.0;
		m.pose.orientation.w = 1.0;

		// Set the scale of the marker -- 1x1x1 here means 1m on a side
		m.scale.x = 1.0;
		m.scale.y = 1.0;
		m.scale.z = 1.0;

		// Set the color -- be sure to set alpha to something non-zero!
		m.color.r = 1.0f;
		m.color.g = 1.0f;
		m.color.b = 0.0f;
		m.color.a = 1.0;

		m.lifetime = ros::Duration();

		markerPub.publish(m);
	}
}
