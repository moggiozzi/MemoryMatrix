#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "ColorRGB.h"
#include <string>

enum DisplayOrientation { DO_HORIZONTAL, DO_VERTICAL };
enum ControlMode {
	CM_ACCEL,
	CM_TOUCH,
	//CM_BUTTONS,
	CM_COUNT };

class Settings
{
  //������ ��������� � android:screenOrientation="landscape|portrait"
  DisplayOrientation defaultOrientation;
  //�������� ���������� �������
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

  static ColorRGB colorCellClosed; // ��������
  static ColorRGB colorCellEmpty;  // �������� �����
  static ColorRGB colorCellError;  // �������� ��������
  static ColorRGB colorBoard;      // �����
  static ColorRGB colorBackground; // ���
};

extern Settings settings;

#endif
