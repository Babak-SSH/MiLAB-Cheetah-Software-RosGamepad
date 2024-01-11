/*! @file main.cpp
 *  @brief Main function for simulator
 */

#include "Collision/CollisionPlane.h"
#include "DrawList.h"
#include "Dynamics/Cheetah3.h"
#include "Dynamics/DynamicsSimulator.h"
#include "Dynamics/FloatingBaseModel.h"
#include "Dynamics/MiniCheetah.h"
#include "Dynamics/Quadruped.h"
#include "Graphics3D.h"
#include "SimControlPanel.h"
#include "Simulation.h"
#include "Utilities/utilities.h"
#include "Utilities/SegfaultHandler.h"
#include "GameController.h"

#include <QApplication>
#include <QSurfaceFormat>

#include <stdio.h>
#include <unistd.h>
#include <thread>

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>

sensor_msgs::Joy gamepadData;

void handleMsg(const sensor_msgs::Joy& msg) {
  gamepadData = msg;
}

/*!
 * Setup QT and run a simulation
 */
int main(int argc, char *argv[]) {
  install_segfault_handler(nullptr);
  // set up Qt
  QApplication a(argc, argv);

  ros::init(argc, argv, "gamepad_listener", ros::InitOption::NoSigintHandler);
  ros::NodeHandle nh;
  ros::Subscriber sub = nh.subscribe("/joy", 1, &handleMsg);
  ros::AsyncSpinner spinner(2); // Use 4 threads
  spinner.start();

  // open simulator UI
  SimControlPanel panel;
  panel.show();

  // run the Qt program
  a.exec();

  ros::shutdown();
  return 0;
}
