/*
 * DummyMRT.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: farbod
 */

#include <ocs2_mpc/MPC_Settings.h>
#include <ocs2_quadruped_interface/QuadrupedDummyNode.h>
#include <ros/init.h>

#include "ocs2_anymal_bear/AnymalBearInterface.h"

int main(int argc, char* argv[]) {
  {
    std::vector<std::string> programArgs{};
    ::ros::removeRosArgs(argc, argv, programArgs);
    if (programArgs.size() <= 1) {
      throw std::runtime_error("No task file specified. Aborting.");
    }
    const std::string taskName(programArgs[1]);
  }

  // Initialize ros node
  ros::init(argc, argv, "anymal_bear_mrt");
  ros::NodeHandle nodeHandle;

  auto anymalInterface = anymal::getAnymalBearInterface(anymal::getTaskFileFolderBear(taskName));
  ocs2::MPC_Settings mpcSettings;
  mpcSettings.loadSettings(anymal::getTaskFilePathBear(taskName));
  quadrupedDummyNode(nodeHandle, *anymalInterface, &anymalInterface->getRollout(), mpcSettings.mrtDesiredFrequency_,
                     mpcSettings.mpcDesiredFrequency_);

  return 0;
}
