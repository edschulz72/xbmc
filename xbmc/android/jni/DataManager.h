#pragma once

#include "JNIBase.h"

class CJNITimeZone;
class CJNICalendar;
class CJNIDataManager
{
public:
  static bool getAutoTime();
  static bool getAutoTimeZone();

  static void setAutoTime(bool autoTime);
  static void setAutoTimeZone(bool autoTimeZone);

  static std::vector<CJNITimeZone> getTimeZones();
  static CJNITimeZone getCurrTimeZone();

  static void setCurrTimeZone(const CJNITimeZone& timeZone);

  static void getCurrTime(int& year, int& month, int& day, int& hourOfDay, int& minute);
  static void setCurrTime(int year, int month, int day, int hourOfDay, int minute);

  static bool saveAudioDeviceActive(const std::vector<std::string>& devices, const std::string& state );
private:
  CJNIDataManager() {};
  ~CJNIDataManager() {};
};
