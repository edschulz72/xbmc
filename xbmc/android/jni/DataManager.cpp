#include "DataManager.h"
#include "Settings.h"
#include "Context.h"
#include "ContentResolver.h"
#include "AlarmManager.h"
#include "TimeZone.h"
#include "Calendar.h"
#include "jutils/jutils-details.hpp"

#define  AUTO_TIME      "auto_time"
#define  AUTO_TIME_ZONE "auto_time_zone"

using namespace jni;
bool CJNIDataManager::getAutoTime()
{
  return CJNISettings::Global::getInt(CJNIContext::getContentResolver(), AUTO_TIME);
}

bool CJNIDataManager::getAutoTimeZone()
{
  return CJNISettings::Global::getInt(CJNIContext::getContentResolver(), AUTO_TIME_ZONE);
}

void CJNIDataManager::setAutoTime( bool autoTime )
{
  CJNISettings::Global::putInt(CJNIContext::getContentResolver(), AUTO_TIME, autoTime);
}

void CJNIDataManager::setAutoTimeZone( bool autoTimeZone )
{
  CJNISettings::Global::putInt(CJNIContext::getContentResolver(), AUTO_TIME_ZONE, autoTimeZone);
}

std::vector<CJNITimeZone> CJNIDataManager::getTimeZones()
{
  const char* const IDs[] = 
  {
    "Pacific/Majuro",
    "Pacific/Midway",
    "Pacific/Honolulu",
    "America/Anchorage",
    "America/Los_Angeles",
    "America/Tijuana",
    "America/Phoenix",
    "America/Chihuahua",
    "America/Denver",
    "America/Costa_Rica",
    "America/Chicago",
    "America/Mexico_City",
    "America/Regina",
    "America/Bogota",
    "America/New_York",
    "America/Caracas",
    "America/Barbados",
    "America/Halifax",
    "America/Manaus",
    "America/Santiago",
    "America/St_Johns",
    "America/Sao_Paulo",
    "America/Argentina/Buenos_Aires",
    "America/Godthab",
    "America/Montevideo",
    "Atlantic/South_Georgia",
    "Atlantic/Azores",
    "Atlantic/Cape_Verde",
    "Africa/Casablanca",
    "Europe/London",
    "Europe/Amsterdam",
    "Europe/Belgrade",
    "Europe/Brussels",
    "Europe/Sarajevo",
    "Africa/Windhoek",
    "Africa/Brazzaville",
    "Asia/Amman",
    "Europe/Athens",
    "Asia/Beirut",
    "Africa/Cairo",
    "Europe/Helsinki",
    "Asia/Jerusalem",
    "Europe/Minsk",
    "Africa/Harare",
    "Asia/Baghdad",
    "Europe/Moscow",
    "Asia/Kuwait",
    "Africa/Nairobi",
    "Asia/Tehran",
    "Asia/Baku",
    "Asia/Tbilisi",
    "Asia/Yerevan",
    "Asia/Dubai",
    "Asia/Kabul",
    "Asia/Karachi",
    "Asia/Oral",
    "Asia/Yekaterinburg",
    "Asia/Calcutta",
    "Asia/Colombo",
    "Asia/Katmandu",
    "Asia/Almaty",
    "Asia/Rangoon",
    "Asia/Krasnoyarsk",
    "Asia/Bangkok",
    "Asia/Shanghai",
    "Asia/Hong_Kong",
    "Asia/Irkutsk",
    "Asia/Kuala_Lumpur",
    "Australia/Perth",
    "Asia/Taipei",
    "Asia/Seoul",
    "Asia/Tokyo",
    "Asia/Yakutsk",
    "Australia/Adelaide",
    "Australia/Darwin",
    "Australia/Brisbane",
    "Australia/Hobart",
    "Australia/Sydney",
    "Asia/Vladivostok",
    "Pacific/Guam",
    "Asia/Magadan",
    "Pacific/Auckland",
    "Pacific/Fiji",
    "Pacific/Tongatapu",
	NULL,
  };

  //CJNITimeZone::getAvailableIDs();
  std::vector<CJNITimeZone> zones;
  for (std::size_t index = 0; IDs[index] != NULL; ++index)
  {
    zones.push_back(CJNITimeZone::getTimeZone(IDs[index]));
  }
  return zones;
}

CJNITimeZone CJNIDataManager::getCurrTimeZone() 
{
  return CJNITimeZone::getDefault();
}

void CJNIDataManager::setCurrTimeZone( const CJNITimeZone& timeZone )
{
  CJNITimeZone::setDefault(timeZone);
  CJNIAlarmManager alarmManager(CJNIContext::getSystemService("alarm"));
  alarmManager.setTimeZone(timeZone.getID());
}

void CJNIDataManager::getCurrTime(int& year, int& month, int& day, int& hourOfDay, int& minute)
{
  CJNICalendar::getInstance().get(year, month, day, hourOfDay, minute);

  month += 1;
}

void CJNIDataManager::setCurrTime(int year, int month, int day, int hourOfDay, int minute)
{
  month -= 1;

  CJNICalendar calendar = CJNICalendar::getInstance();
  calendar.set(year, month, day, hourOfDay, minute);
  
  CJNIAlarmManager alarmManager(CJNIContext::getSystemService("alarm"));
  alarmManager.setTime(calendar.getTimeInMillis());
}

bool CJNIDataManager::saveAudioDeviceActive(const std::vector<std::string>& devices, const std::string& state )
{
  std::string strDevices;
  for(std::size_t index = 0; index < devices.size(); ++index)
  {      
      if(strDevices.empty())
      {
          strDevices = devices[index];
      }
      else
      {
          strDevices.append("," + devices[index]);
      }
  }
  return CJNISettings::System::putString(CJNIContext::getContentResolver()
      , CJNISettings::System::AUDIO_OUTPUT_CHANNEL, strDevices);
}


