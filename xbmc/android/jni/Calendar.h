#pragma once

#include "JNIBase.h"

class CJNICalendar : public CJNIBase
{
public:
  CJNICalendar(const jni::jhobject &object) : CJNIBase(object) {};
  ~CJNICalendar() {};

  static CJNICalendar getInstance();

  void set(int year, int month, int day, int hourOfDay, int minute);
  void get(int& year, int& month, int& day, int& hourOfDay, int& minute);
  int64_t getTimeInMillis();

private:
  CJNICalendar();
};
