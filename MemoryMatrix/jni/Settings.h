#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <string>

enum DisplayOrientation { DO_HORIZONTAL, DO_VERTICAL };
enum ControlMode {
	CM_ACCEL,
	CM_TOUCH,
	//CM_BUTTONS,
	CM_COUNT };

class Settings
{
  //должен совпадать с android:screenOrientation="landscape|portrait"
  DisplayOrientation defaultOrientation;
  //желаемая ориентация дисплея
  DisplayOrientation displayOrientation;
  ControlMode controlMode;
  std::string controlModeName;
  void setControlModeName();
public:
  Settings();
  DisplayOrientation getDeviceDefaultOrientation();
  
  DisplayOrientation getOrientation();
  std::string getOrientationName();

  ControlMode getControlMode();
  const std::string & getControlModeName();
  void nextControlMode();
};

extern Settings settings;

#endif
