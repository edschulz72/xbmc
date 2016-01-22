#include "Calendar.h"
#include "JNIBase.h"
#include "jutils/jutils-details.hpp"

using namespace jni;

#define CALENDAR_YEAR            0x01
#define CALENDAR_MONTH           0x02
#define CALENDAR_DAY_OF_MONTH    0x05
#define CALENDAR_HOUR_OF_DAY     0x0b
#define CALENDAR_MINUTE          0x0c
#define CALENDAR_SECOND          0x0d

CJNICalendar::CJNICalendar()
    :CJNIBase("java/util/Calendar")
{
  m_object = new_object(GetClassName());
  m_object.setGlobal();
}

CJNICalendar CJNICalendar::getInstance()
{
  return call_static_method<jhobject>("java/util/Calendar",
    "getInstance", "()Ljava/util/Calendar;");
}

void CJNICalendar::set( int year, int month, int day, int hourOfDay, int minute )
{
  return call_method<void>(m_object,
    "set", "(IIIII)V", year, month, day, hourOfDay, minute);
}

void CJNICalendar::get( int& year, int& month, int& day, int& hourOfDay, int& minute )
{
  year      = call_method<jint>(m_object, "get", "(I)I", CALENDAR_YEAR);
  month     = call_method<jint>(m_object, "get", "(I)I", CALENDAR_MONTH);
  day       = call_method<jint>(m_object, "get", "(I)I", CALENDAR_DAY_OF_MONTH);
  hourOfDay = call_method<jint>(m_object, "get", "(I)I", CALENDAR_HOUR_OF_DAY);
  minute    = call_method<jint>(m_object, "get", "(I)I", CALENDAR_MINUTE);
}


int64_t CJNICalendar::getTimeInMillis()
{
  return call_method<jlong>(m_object, "getTimeInMillis", "()J");
}
