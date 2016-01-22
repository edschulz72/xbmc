#pragma once

#include "JNIBase.h"

class CJNITimeZone : public CJNIBase
{
public:
  CJNITimeZone(const jni::jhobject &object) : CJNIBase(object) {};
  ~CJNITimeZone() {};

  static CJNITimeZone getDefault();  
  static void setDefault(const CJNITimeZone & timeZone);

  std::string getDisplayName() const;
  std::string getID() const;
  //void setID(const std::string& strId);
 
  static std::vector<std::string> getAvailableIDs();
  static CJNITimeZone getTimeZone(const std::string& id);
public:
  CJNITimeZone();
};
