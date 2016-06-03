
#ifdef HAS_VIDONME

#pragma once

#include "JNIBase.h"
#include "string.h"
#include "ApplicationInfo.h"
using namespace  jni;

class CJNIPackageInfo : public CJNIBase
{
public:
  CJNIPackageInfo(const jni::jhobject &object);
  ~CJNIPackageInfo(){};

	std::string								packageName;
	std::string								versionName;
	int												versionCode;
	long											firstInstallTime;
	long											lastUpdateTime;


private:
	CJNIPackageInfo();
};


#endif