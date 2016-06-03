#pragma once

#include "JNIBase.h"
#include "List.h"

class CJNILocale : public CJNIBase
{
public:
  CJNILocale(const jni::jhobject &object) : CJNIBase(object) {};
  ~CJNILocale() {};

  CJNILocale(std::string language);
  CJNILocale(std::string language, std::string country);
	
  std::string getDisplayLanguage();
  std::string getDisplayCountry();
  std::string getLanguage();  
  std::string getCountry();

  static CJNILocale getDefault();
  static void setDefault(CJNILocale locale); 
  static CJNILocale getLocale(const std::string& strLocale);
};



