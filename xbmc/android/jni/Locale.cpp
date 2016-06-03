#include "Locale.h"
#include "jutils/jutils-details.hpp"
#include "Context.h"
#include "ClassLoader.h"

using namespace jni;
CJNILocale::CJNILocale(std::string language)
  :CJNIBase("java/util/Locale")
{
  m_object = new_object(GetClassName(), "<init>",
    "(Ljava/lang/String;)V", jcast<jhstring>(language));
  m_object.setGlobal();
}

CJNILocale::CJNILocale(std::string language, std::string country)
    :CJNIBase("java/util/Locale")
{
  m_object = new_object(GetClassName(), "<init>",
    "(Ljava/lang/String;Ljava/lang/String;)V", jcast<jhstring>(language),
    jcast<jhstring>(country));
  m_object.setGlobal();
}

std::string CJNILocale::getDisplayLanguage()
{
  return jcast<std::string>(call_method<jhstring>(m_object,
    "getDisplayLanguage", "(Ljava/util/Locale;)Ljava/lang/String;",
	this->get_raw()));
}

std::string CJNILocale::getDisplayCountry()
{
  return jcast<std::string>(call_method<jhstring>(m_object,
    "getDisplayCountry", "(Ljava/util/Locale;)Ljava/lang/String;",
	this->get_raw()));
}


std::string CJNILocale::getLanguage()
{
  return jcast<std::string>(call_method<jhstring>(m_object,
    "getLanguage", "()Ljava/lang/String;"));
}

std::string CJNILocale::getCountry()
{
  return jcast<std::string>(call_method<jhstring>(m_object,
    "getCountry", "()Ljava/lang/String;"));
}

void CJNILocale::setDefault( CJNILocale locale )
{
  call_static_method<void>("java/util/Locale", 
    "setDefault", "(Ljava/util/Locale;)V", 
	locale.get_raw());
  return call_static_method<void>("com/android/internal/app/LocalePicker",
    "updateLocale", "(Ljava/util/Locale;)V",
	locale.get_raw());
}

CJNILocale CJNILocale::getDefault()
{
  return call_static_method<jhobject>("java/util/Locale",
    "getDefault", "()Ljava/util/Locale;");
}


CJNILocale CJNILocale::getLocale(const std::string& strLocale)
{
   return CJNILocale(get_static_field<jhobject>("java/util/Locale", strLocale.c_str()));
}
