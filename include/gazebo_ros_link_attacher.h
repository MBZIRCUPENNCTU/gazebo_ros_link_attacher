/*
 * Desc: Gazebo link attacher plugin.
 * Author: Sammy Pfeiffer (sam.pfeiffer@pal-robotics.com)
 * Date: 05/04/2016
 */

#ifndef GAZEBO_ROS_LINK_ATTACHER_HH
#define GAZEBO_ROS_LINK_ATTACHER_HH

#include <ros/ros.h>

#include <sdf/sdf.hh>
#include "gazebo/gazebo.hh"
#include <gazebo/physics/physics.hh>
#include "gazebo/physics/PhysicsTypes.hh"
#include <gazebo/transport/TransportTypes.hh>
#include <gazebo/common/Time.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/Events.hh>
#include "gazebo/msgs/msgs.hh"
#include "gazebo/transport/transport.hh"
#include "gazebo_ros_link_attacher/Attach.h"
#include "gazebo_ros_link_attacher/AttachRequest.h"
#include "gazebo_ros_link_attacher/AttachResponse.h"
#include "gazebo_ros_link_attacher/AttachTyped.h"
#include "gazebo_ros_link_attacher/AttachTypedRequest.h"
#include "gazebo_ros_link_attacher/AttachTypedResponse.h"
#include <mutex>

namespace gazebo {

typedef struct AtachDetachStruct {
	AtachDetachStruct(std::string model1, std::string link1, std::string model2, std::string link2, std::string joint_type) {
		this->model1 = model1;
		this->link1 = link1;
		this->model2 = model2;
		this->link2 = link2;
		this->joint_type = joint_type;
	}

	std::string model1;
	std::string link1;
	std::string model2;
	std::string link2;
	std::string joint_type;
} AtachDetachStruct;

class GazeboRosLinkAttacher: public WorldPlugin {
public:
	/// \brief Constructor
	GazeboRosLinkAttacher();

	/// \brief Destructor
	virtual ~GazeboRosLinkAttacher();

	/// \brief Load the controller
	void Load(physics::WorldPtr _world, sdf::ElementPtr /*_sdf*/);

	void OnUpdate();

	/// \brief Attach with a revolute joint
	bool attach(std::string model1, std::string link1, std::string model2, std::string link2, std::string joint_type);

	/// \brief Detach
	bool detach(std::string model1, std::string link1, std::string model2, std::string link2);

	/// \brief Internal representation of a fixed joint
	struct fixedJoint {
		std::string model1;
		physics::ModelPtr m1;
		std::string link1;
		physics::LinkPtr l1;
		std::string model2;
		physics::ModelPtr m2;
		std::string link2;
		physics::LinkPtr l2;
		physics::JointPtr joint;
	};

	bool getJoint(std::string model1, std::string link1, std::string model2, std::string link2, fixedJoint &joint);

private:
	ros::NodeHandle nh_;
	ros::ServiceServer attach_service_;
	ros::ServiceServer detach_service_;
	ros::ServiceServer attach_typed_service_;

	std::vector<event::ConnectionPtr> connections;

	std::vector<AtachDetachStruct> toDetachVector;
	std::vector<AtachDetachStruct> toAtachVector;
	std::mutex mtx_detach;
	std::mutex mtx_attach;

	void HandleAttaches();
	void HandleDetaches();

	bool attach_callback(gazebo_ros_link_attacher::Attach::Request &req, gazebo_ros_link_attacher::Attach::Response &res);bool detach_callback(
			gazebo_ros_link_attacher::Attach::Request &req, gazebo_ros_link_attacher::Attach::Response &res);

	bool attach_typed_callback(gazebo_ros_link_attacher::AttachTyped::Request &req, gazebo_ros_link_attacher::AttachTyped::Response &res);

	common::Time prevUpdateTime;
	common::Time updateRate;

	std::vector<fixedJoint> joints;

	/// \brief The physics engine.
	physics::PhysicsEnginePtr physics;

	/// \brief Pointer to the world.
	physics::WorldPtr world;

	std::vector<std::string> allowed_joint_types;

};

}

#endif

