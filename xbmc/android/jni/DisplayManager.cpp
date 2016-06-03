#include "DisplayManager.h"

#include "Context.h"

#include "jutils/jutils.hpp"
#include "jutils/jutils-details.hpp"

using namespace jni;

int CJNIDisplayManager::getDisplayOutput(int displayType)
{
  return call_method<jint>(m_object,
  	"getDisplayOutput", "(I)I",
	(jint)displayType);
}

int CJNIDisplayManager::setDisplayOutput(int displayType, int format)
{
  return call_method<jint>(m_object,
    "setDisplayOutput", "(II)I",
	(jint)displayType, (jint)format);
}

int CJNIDisplayManager::setDisplayPercent( int displayType, int percent )
{
  return call_method<jint>(m_object,
    "setDisplayPercent", "(II)I",
    (jint)displayType, (jint)percent);
}

int CJNIDisplayManager::getDisplayContrast( int displayType )
{
  return call_method<jint>(m_object,
    "getDisplayContrast", "(I)I",
    (jint)displayType);
}

int CJNIDisplayManager::setDisplayContrast( int displayType, int contrast )
{
  return call_method<jint>(m_object,
    "setDisplayContrast", "(II)I",
    (jint)displayType, (jint)contrast);
}

int CJNIDisplayManager::getDisplayBright( int displayType )
{
  return call_method<jint>(m_object,
    "getDisplayBright", "(I)I",
    (jint)displayType);
}

int CJNIDisplayManager::setDisplayBright( int displayType, int bright )
{
  return call_method<jint>(m_object,
    "setDisplayBright", "(II)I",
    (jint)displayType, (jint)bright);
}
