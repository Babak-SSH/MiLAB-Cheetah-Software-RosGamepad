/*! @file GameController.cpp
 *  @brief Code to read the Logitech F310 Game Controller
 *  Creates a DriverCommand object to be sent to the robot controller
 *  Used in the development simulator and in the robot control mode
 *
 *  NOTE: Because QT is weird, the updateDriverCommand has to be called from a
 * QT event. Running it in another thread will cause it to not work. As a
 * result, this only works if called in the update method of a QTObject
 */

#include "GameController.h"

#include <QtCore/QObject>
#include <QtGamepad/QGamepad>

/*!
 * By default, the game controller selects the "first" joystick, printing a
 * warning if there are multiple joysticks On Linux, this is /dev/input/js0 If
 * no joystick is found, it will print an error message, and will return zero.
 * It is possible to change/add a joystick later with findNewController
 */
GameController::GameController(QObject *parent) : QObject(parent) {
  findNewController();
}

/*!
 * Re-run the joystick finding code to select the "first" joystick. This can be
 * used to set up the joystick if the simulator is started without a joystick
 * plugged in
 */
void GameController::findNewController() {
  ros::master::V_TopicInfo master_topics;
  ros::master::getTopics(master_topics);

  for (ros::master::V_TopicInfo::iterator it = master_topics.begin() ; it != master_topics.end(); it++) {
    const ros::master::TopicInfo& info = *it;
    if (info.name == "/joy") {
      rosGamepad = true;
    }
  }
  delete _qGamepad;
  _qGamepad = nullptr;  // in case this doesn't work!

  printf("[Gamepad] Searching for gamepads, please ignore \"Device discovery cannot open device\" errors\n");
  auto gamepadList = QGamepadManager::instance()->connectedGamepads();
  printf("[Gamepad] Done searching for gamepads.\n");
  if (gamepadList.empty()) {
    printf(
        "[ERROR: GamePad] No controller was connected! All joystick "
        "commands will be zero!\n");
  } else {
    if (gamepadList.size() > 1) {
      printf(
          "[ERROR: GamePad] There are %d joysticks connected.  Using "
          "the first one.\n",
          gamepadList.size());
    } else {
      printf("[GamePad] Found 1 joystick\n");
    }

    _qGamepad = new QGamepad(*gamepadList.begin());
  }
}

/*!
 * Overwrite a driverCommand with the current joystick state.  If there's no
 * joystick, sends zeros
 * TODO: what happens if the joystick is unplugged?
 */
void GameController::updateGamepadCommand(GamepadCommand &gamepadCommand) {
  // if (_qGamepad) {
  //   gamepadCommand.leftBumper = _qGamepad->buttonL1();
  //   gamepadCommand.rightBumper = _qGamepad->buttonR1();
  //   gamepadCommand.leftTriggerButton = _qGamepad->buttonL2() != 0.;
  //   gamepadCommand.rightTriggerButton = _qGamepad->buttonR2() != 0.;
  //   gamepadCommand.back = _qGamepad->buttonSelect();
  //   gamepadCommand.start = _qGamepad->buttonStart();
  //   gamepadCommand.a = _qGamepad->buttonA();
  //   gamepadCommand.b = _qGamepad->buttonB();
  //   gamepadCommand.x = _qGamepad->buttonX();
  //   gamepadCommand.y = _qGamepad->buttonY();
  //   gamepadCommand.leftStickButton = _qGamepad->buttonL3();
  //   gamepadCommand.rightStickButton = _qGamepad->buttonR3();
  //   gamepadCommand.leftTriggerAnalog = (float)_qGamepad->buttonL2();
  //   gamepadCommand.rightTriggerAnalog = (float)_qGamepad->buttonR2();
  //   gamepadCommand.leftStickAnalog =
  //       Vec2<float>(_qGamepad->axisLeftX(), -_qGamepad->axisLeftY());
  //   gamepadCommand.rightStickAnalog =
  //       Vec2<float>(_qGamepad->axisRightX(), -_qGamepad->axisRightY());
  // }
  if (rosGamepad) {
    gamepadCommand.leftBumper = gamepadData.buttons[7];
    gamepadCommand.rightBumper = gamepadData.buttons[5];
    gamepadCommand.leftTriggerButton = gamepadData.buttons[8];
    gamepadCommand.rightTriggerButton = gamepadData.buttons[6];
    gamepadCommand.back = gamepadData.buttons[12];
    gamepadCommand.start = gamepadData.buttons[13];
    gamepadCommand.a = gamepadData.buttons[0];
    gamepadCommand.b = gamepadData.buttons[1];
    gamepadCommand.x = gamepadData.buttons[2];
    gamepadCommand.y = gamepadData.buttons[3];
    gamepadCommand.leftStickButton = gamepadData.buttons[10];
    gamepadCommand.rightStickButton = gamepadData.buttons[9];
    gamepadCommand.leftStickAnalog = Vec2<float>(-gamepadData.axes[0], gamepadData.axes[1]);
    gamepadCommand.rightStickAnalog = Vec2<float>(-gamepadData.axes[3], -gamepadData.axes[4]);
  } else {
    gamepadCommand.zero();  // no joystick, return all zeros
  }

  // printf("%s\n", gamepadCommand.toString().c_str());
}

GameController::~GameController() { delete _qGamepad; }