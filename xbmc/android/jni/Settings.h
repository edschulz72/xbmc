#pragma once

#include "JNIBase.h"

class CJNIContentResolver;
class CJNISettings
{
public:
  class Global
  {
  public:
    static int getInt(const CJNIContentResolver & cr, const std::string & name);
    static int getInt(const CJNIContentResolver & cr, const std::string & name, const int def);
    static bool putInt(const CJNIContentResolver & cr, const std::string & name, int value);
  };

  class System
  {
  public:
    static const char* const AUDIO_OUTPUT_CHANNEL;
    static const char* const DISPLAY_OUTPUT_FORMAT;
    static const char* const DATE_FORMAT;
    static const char* const TIME_12_24;
    
  public:
    static int getInt(const CJNIContentResolver & cr, const std::string & name);
    static int getInt(const CJNIContentResolver & cr, const std::string & name, const int def);
    static bool putInt(const CJNIContentResolver & cr, const std::string & name, int value);
    static bool putString(const CJNIContentResolver & cr, const std::string & name, const std::string& value);
    static std::string getString(const CJNIContentResolver & cr, const std::string & name);
  };
};
