#include "DisplayManagerGlobal.h"

#include "Context.h"
#include "jutils/jutils.hpp"
#include "jutils/jutils-details.hpp"

using namespace jni;

CJNIDisplayManagerGlobal CJNIDisplayManagerGlobal::getInstance()
{
  return call_static_method<jhobject>("android/hardware/display/DisplayManagerGlobal",
    "getInstance", "()Landroid/hardware/display/DisplayManagerGlobal;");
}

int CJNIDisplayManagerGlobal::getDisplayParameter( int displaytype, int cmd, int para0, int para1 )
{
  return call_method<jint>(m_object,
    "getDisplayParameter", "(IIII)I",
    displaytype, cmd, para0, para1);
}

int CJNIDisplayManagerGlobal::setDisplayParameter( int displaytype, int cmd, int para0, int para1, int para2 )
{
  return call_method<jint>(m_object,
    "setDisplayParameter", "(IIIII)I",
    displaytype, cmd, para0, para1, para2);
}
