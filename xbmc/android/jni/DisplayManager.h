#pragma once

#include <jni.h>

#include <android/jni/JNIBase.h>

//const int DISPLAY_TYPE_BUILT_IN = 1;

class CJNIDisplayManager:CJNIBase
{
public:
  CJNIDisplayManager(const jni::jhobject& object):CJNIBase(object){};
  ~CJNIDisplayManager() {};
public:
  int getDisplayOutput(int displayType);
  int setDisplayOutput(int displayType, int format);
  int setDisplayPercent(int displayType, int percent);

  int getDisplayContrast(int displayType);
  int setDisplayContrast(int displayType, int contrast);

  int getDisplayBright(int displayType);
  int setDisplayBright(int displayType, int bright);
};
