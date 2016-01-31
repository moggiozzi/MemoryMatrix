#include "Settings.h"

// todo color settings
ColorRGB Settings::colorCellClosed(0,0,0);
ColorRGB Settings::colorCellEmpty(0,0,0); 
ColorRGB Settings::colorCellError(0,0,0);
ColorRGB Settings::colorBoard(0,0,0);
ColorRGB Settings::colorBackground(0,0,0);

Settings settings;

Settings::Settings() :
defaultOrientation(DO_HORIZONTAL),//(DO_VERTICAL),
displayOrientation(DO_HORIZONTAL),//(DO_VERTICAL),
controlMode(CM_ACCEL),
controlModeName("ACCEL")
{
}

DisplayOrientation Settings::getDeviceDefaultOrientation()
{
  return defaultOrientation;
}

DisplayOrientation Settings::getOrientation()
{
  return displayOrientation;
}

std::string Settings::getOrientationName()
{
  if ( displayOrientation == DO_HORIZONTAL )
    return "H";
  else
    return "V";
}

ControlMode Settings::getControlMode()
{
  return controlMode;
}

void Settings::setControlModeName()
{
  switch(controlMode)
	{
	case CM_ACCEL:
		controlModeName = "ACCEL";
		break;
//	case CM_BUTTONS:
//		controlModeName = "BUTTONS";
//		break;
    case CM_TOUCH:
        controlModeName = "TOUCH";
        break;
	}
}

//void Settings::setControlMode( ControlMode newMode )
//{
//	controlMode = newMode;
//	setControlModeName();
//}

const std::string & Settings::getControlModeName()
{ 
  return controlModeName; 
}

void Settings::nextControlMode()
{
  controlMode = (ControlMode) ((controlMode + 1) % CM_COUNT);
  setControlModeName();
}
//#ifdef __ANDROID__
//#elif _WIN32
//#else
//#endif
