
#ifdef HAS_VIDONME

#include "PackageInfo.h"
#include "android/jni/jutils/jutils-details.hpp"

CJNIPackageInfo::CJNIPackageInfo(const jni::jhobject &object): CJNIBase(object)
	, packageName ( jcast<std::string>(get_field<jhstring>( m_object, "packageName")))
	, versionName ( jcast<std::string>(get_field<jhstring>( m_object, "versionName")))
	, versionCode ( get_field<jint>( m_object, "versionCode"))
	, firstInstallTime( get_field<jlong>(m_object, "firstInstallTime"))
	,	lastUpdateTime(get_field<jlong>(m_object, "lastUpdateTime"))
{
	
}

#endif

