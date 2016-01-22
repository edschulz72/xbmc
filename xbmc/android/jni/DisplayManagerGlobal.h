#pragma once

#include <jni.h>

#include <android/jni/JNIBase.h>

const int DISPLAY_TYPE_BUILT_IN = 1;

const int DISPLAY_CMD_SET3DMODE               = 0x01;
const int DISPLAY_CMD_SETOUTPUTMODE           = 0x06;
const int DISPLAY_CMD_SETPERCENT              = 0x07;
const int DISPLAY_CMD_SETSATURATION           = 0x08;
const int DISPLAY_CMD_SETHUE                  = 0x09;
const int DISPLAY_CMD_SETCONTRAST             = 0x0a;
const int DISPLAY_CMD_SETBRIGHT               = 0x0b;

const int DISPLAY_CMD_IS_SUPPORT_HDMI_MODE    = 0x19;
const int DISPLAY_CMD_GETSUPPORT3DMODE        = 0x20;
const int DISPLAY_CMD_GETOUTPUTTYPE           = 0x21;
const int DISPLAY_CMD_GETOUTPUTMODE           = 0x22;
const int DISPLAY_CMD_GETSATURATION           = 0x23;
const int DISPLAY_CMD_GETHUE                  = 0x24;
const int DISPLAY_CMD_GETCONTRAST             = 0x25;
const int DISPLAY_CMD_GETBRIGHT               = 0x26;

class CJNIDisplayManagerGlobal:CJNIBase
{
public:
  CJNIDisplayManagerGlobal(const jni::jhobject& object):CJNIBase(object){};
  ~CJNIDisplayManagerGlobal() {};

  static CJNIDisplayManagerGlobal getInstance();

  int getDisplayParameter(int displaytype, int cmd, int para0, int para1);
  int setDisplayParameter(int displaytype, int cmd, int para0, int para1, int para2);

private:

};
