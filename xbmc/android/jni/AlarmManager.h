#pragma once

#include "JNIBase.h"

class CJNIAlarmManager : public CJNIBase
{
public:
  CJNIAlarmManager(const jni::jhobject &object) : CJNIBase(object) {};
  ~CJNIAlarmManager() {};

  void setTime(int64_t millis);
  void setTimeZone(std::string timeZone); 

private:
  CJNIAlarmManager();
};