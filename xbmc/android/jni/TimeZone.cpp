#include "TimeZone.h"
#include "JNIBase.h"
#include "jutils/jutils-details.hpp"
//#include "utils/log.h"

using namespace jni;

CJNITimeZone::CJNITimeZone()
  :CJNIBase("java/util/TimeZone")
{
  m_object = new_object(GetClassName());
  m_object.setGlobal();
}

CJNITimeZone CJNITimeZone::getDefault()
{
  return call_static_method<jhobject>("java/util/TimeZone",
    "getDefault", "()Ljava/util/TimeZone;");
}

void CJNITimeZone::setDefault( const CJNITimeZone & timeZone )
{
  return call_static_method<void>("java/util/TimeZone",
    "setDefault", "(Ljava/util/TimeZone;)V", timeZone.get_raw());
}

std::string CJNITimeZone::getDisplayName() const
{
  return jcast<std::string>(call_method<jhstring>(m_object,
    "getDisplayName", "()Ljava/lang/String;"));
}

std::string CJNITimeZone::getID() const
{
  return jcast<std::string>(call_method<jhstring>(m_object,
    "getID", "()Ljava/lang/String;"));
}

//void CJNITimeZone::setID(const std::string& strId)
//{
//  return call_method<void>(m_object, "setID", "(Ljava/lang/String;)V", jcast<jhstring>(strId));
//}

std::vector<std::string> CJNITimeZone::getAvailableIDs()
{
  return jcast<std::vector<std::string>>(call_static_method<jhobjectArray>("java/util/TimeZone",
      "getAvailableIDs", "()[Ljava/lang/String;"));
}

CJNITimeZone CJNITimeZone::getTimeZone(const std::string& id)
{
  return call_static_method<jhobject>("java/util/TimeZone"
	, "getTimeZone", "(Ljava/lang/String;)Ljava/util/TimeZone;"
	, jcast<jhstring>(id));
}

