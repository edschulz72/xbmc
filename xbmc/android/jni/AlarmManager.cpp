#include "AlarmManager.h"
#include "jutils/jutils-details.hpp"

using namespace jni;

CJNIAlarmManager::CJNIAlarmManager()
  :CJNIBase("android/app/AlarmManager")
{
  m_object = new_object(GetClassName());
  m_object.setGlobal();
}

void CJNIAlarmManager::setTime( int64_t millis )
{
  return call_method<void>(m_object,
    "setTime", "(J)V", (jlong)millis);
}

void CJNIAlarmManager::setTimeZone( std::string timeZone )
{
  call_method<void>(m_object,
    "setTimeZone", "(Ljava/lang/String;)V", 
	jcast<jhstring>(timeZone));
}
