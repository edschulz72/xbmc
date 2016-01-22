#include "Settings.h"
#include "ContentResolver.h"
#include "jutils/jutils-details.hpp"

using namespace jni;
int CJNISettings::Global::getInt( const CJNIContentResolver & cr, const std::string & name )
{
  return (call_static_method<jint>("android/provider/Settings$Global",
    "getInt", "(Landroid/content/ContentResolver;Ljava/lang/String;)I",cr.get_raw(), jcast<jhstring>(name)));
}

int CJNISettings::Global::getInt( const CJNIContentResolver & cr, const std::string & name, const int def )
{
  return (call_static_method<jint>("android/provider/Settings$Global",
    "getInt", "(Landroid/content/ContentResolver;Ljava/lang/String;I)I",cr.get_raw(), jcast<jhstring>(name), def));
}

bool CJNISettings::Global::putInt( const CJNIContentResolver & cr, const std::string & name, int value )
{
  return (call_static_method<jboolean>("android/provider/Settings$Global",
    "putInt", "(Landroid/content/ContentResolver;Ljava/lang/String;I)Z",cr.get_raw(), jcast<jhstring>(name), value));
}

const char* const CJNISettings::System::AUDIO_OUTPUT_CHANNEL = "audio_output_channel";
const char* const CJNISettings::System::DISPLAY_OUTPUT_FORMAT = "display_output_format";
const char* const CJNISettings::System::TIME_12_24 = "time_12_24";
const char* const CJNISettings::System::DATE_FORMAT = "date_format";

int CJNISettings::System::getInt( const CJNIContentResolver & cr, const std::string & name )
{
  return (call_static_method<jint>("android/provider/Settings$System",
    "getInt", "(Landroid/content/ContentResolver;Ljava/lang/String;)I",cr.get_raw(), jcast<jhstring>(name)));
}

int CJNISettings::System::getInt( const CJNIContentResolver & cr, const std::string & name, const int def )
{
  return (call_static_method<jint>("android/provider/Settings$System",
    "getInt", "(Landroid/content/ContentResolver;Ljava/lang/String;I)I",cr.get_raw(), jcast<jhstring>(name), def));
}

bool CJNISettings::System::putInt( const CJNIContentResolver & cr, const std::string & name, int value )
{
  return (call_static_method<jboolean>("android/provider/Settings$System",
    "putInt", "(Landroid/content/ContentResolver;Ljava/lang/String;I)Z",cr.get_raw(), jcast<jhstring>(name), value));
}

bool CJNISettings::System::putString( const CJNIContentResolver & cr, const std::string & name, const std::string& value )
{
    return (call_static_method<jboolean>("android/provider/Settings$System",
        "putString", "(Landroid/content/ContentResolver;Ljava/lang/String;Ljava/lang/String;)Z",
        cr.get_raw(), jcast<jhstring>(name), jcast<jhstring>(value)));
}

std::string CJNISettings::System::getString( const CJNIContentResolver & cr, const std::string & name)
{
    return jcast<std::string>(call_static_method<jhstring>("android/provider/Settings$System",
        "getString", "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;",
        cr.get_raw(), jcast<jhstring>(name)));
}
