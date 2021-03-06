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

#define MAX_ROWS 8
#define MAX_COLS 8

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
  static ColorRGB colorCellEmpty;
  static ColorRGB colorCellFull;
  static ColorRGB colorCellError;  // �������� ��������
  static ColorRGB colorBoard;      // �����
  static ColorRGB colorBackground; // ���
  static ColorRGB colorText;
  static ColorRGB colorResultScore;
};

extern Settings settings;

#endif
