#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <cassert>

#include "SettingsBase.h"
#include "utils/log.h"

#include "android/jni/Context.h"
#include "android/jni/Locale.h"
#include "android/jni/DisplayManager.h"
#include "android/jni/DisplayManagerGlobal.h"
#include "android/jni/Settings.h"
#include "android/jni/ContentResolver.h"

struct LangKey
{
  const char* key;
  const char* lang;
  const char* country;
  bool display_country;
};

LangKey lang_keys[]=
{
    {"en_US", "en", "US", false},
	{"fr_FR", "fr", "FR", false},
    {"zh_CN", "zh", "CN", true},
	{"zh_TW", "zh", "TW", true},
	{"de_DE", "de", "DE", false},
	{"ja_JP", "ja", "JP", false},
	{"es_ES", "es", "ES", false},
	{"pt_PT", "pt", "PT", false},
//	{"it_IT", "it", "IT", false},
	{"ru_RU", "ru", "RU", false},
	{"ko_KR", "ko", "KR", false},
	{"da_DK", "da", "DK", false},
	{"vi_VN", "vi", "VN", false},
  {"th_TH", "th", "TH", false},
	{NULL, NULL, NULL, false},
};

CSettingsBase::LangNodes CSettingsBase::getLanguageList( void )
{
  LangNodes langNodes;
  for (int i = 0; lang_keys[i].key != NULL; i++)
  {
    CJNILocale locale(lang_keys[i].lang, lang_keys[i].country);

    std::string display = locale.getDisplayLanguage();
	if (lang_keys[i].display_country)
	  display+="("+locale.getDisplayCountry()+")";
	langNodes.push_back(LangNode(lang_keys[i].key, display));
  }
  return langNodes;
}

CSettingsBase::LangNode CSettingsBase::getCurrLanguage( void )
{
  CJNILocale locale = CJNILocale::getDefault();
  std::string strKey = locale.getLanguage()+"_"+locale.getCountry();
  return LangNode(strKey, locale.getDisplayLanguage());
}

void CSettingsBase::setCurrLanguage(const std::string& strKey)
{
  for (int i = 0; lang_keys[i].key != NULL; ++i)
  {
    if (strKey == lang_keys[i].key)
	{
	  CJNILocale locale(lang_keys[i].lang, lang_keys[i].country);
	  CJNILocale::setDefault(locale);
	  return;
	}
  }
  CLog::Log(LOGNOTICE, "the language is invalid key(%s)!", strKey.c_str());
  //assert(false);
}


struct DisplayOutputModeKey
{
  int key;
  const char* display;
};

DisplayOutputModeKey display_output_mode_key[] =
{
  //{0x0402, "HDMI 480P"},
  //{0x0403, "HDMI 576P"},
  {0x0404, "HDMI 720P 50Hz"},
  {0x0405, "HDMI 720P 60Hz"},
  {0x0406, "HDMI 1080i 50Hz"},
  {0x0407, "HDMI 1080i 60Hz"},
  {0x0408, "HDMI 1080P 24Hz"},
  {0x0409, "HDMI 1080P 50Hz"},
  {0x040A, "HDMI 1080P 60Hz"},
  {0x0000, NULL},
};


CSettingsBase::DisplayOutputModeNodes CSettingsBase::GetDisplayOutputModeList( void )
{
  DisplayOutputModeNodes modes;
  for (int index = 0; display_output_mode_key[index].key != 0x0000; ++index)
  {
    const DisplayOutputModeKey& mode = display_output_mode_key[index];
	modes.push_back(DisplayOutputModeNode(mode.key, mode.display));  
  }
  return modes;
}

CSettingsBase::DisplayOutputModeNode CSettingsBase::GetCurrDisplayOutputMode( void )
{
  //CJNIDisplayManager displayManager(CJNIContext::getSystemService("android/hardware/display/DisplayManager"));
  CJNIDisplayManager displayManager(CJNIContext::getSystemService("display"));
  int key = displayManager.getDisplayOutput(DISPLAY_TYPE_BUILT_IN);
  for (int index = 0; display_output_mode_key[index].key != 0x0000; ++index)
  {
    const DisplayOutputModeKey& mode = display_output_mode_key[index];
	if ( mode.key == key)
	{
		return DisplayOutputModeNode(key, mode.display);
	} 
  }
  CLog::Log(LOGNOTICE, "the DisplayOutputMode is invalid key(%04x)!", key);
  //assert(false);

  const DisplayOutputModeKey& mode = display_output_mode_key[1];
  return DisplayOutputModeNode(mode.key, mode.display);
}

void CSettingsBase::SetCurrDisplayOutputMode(const int nKey)
{
  //CJNIDisplayManager displayManager(CJNIContext::getSystemService("android/hardware/display/DisplayManager"));
  CJNIDisplayManager displayManager(CJNIContext::getSystemService("display"));
  displayManager.setDisplayOutput(DISPLAY_TYPE_BUILT_IN, nKey);
}

bool CSettingsBase::saveCurrDisplayOutputMode(const int nKey)
{
  char szKey[32];
  snprintf(szKey, sizeof(szKey), "%x", nKey/* % 0xFF*/);
  return CJNISettings::System::putString(CJNIContext::getContentResolver()
	, CJNISettings::System::DISPLAY_OUTPUT_FORMAT, szKey);
}

void CSettingsBase::initCurrDisplayOutputMode()
{
  const std::string strValue = CJNISettings::System::getString(CJNIContext::getContentResolver()
	, CJNISettings::System::DISPLAY_OUTPUT_FORMAT);
  if (strValue.empty()) return;

  int nKey = 0x0404;
  sscanf(strValue.c_str(), "%x", &nKey);
  CLog::Log(LOGNOTICE, "initCurrDisplayOutputMode value(%s)!", strValue.c_str());
  SetCurrDisplayOutputMode(nKey);
}

void CSettingsBase::setDisplayPercent( const int percent )
{
  CJNISettings::System::putInt(CJNIContext::getContentResolver(), "display_area_ratio", percent);

  CJNIDisplayManager displayManager(CJNIContext::getSystemService("display"));
  displayManager.setDisplayPercent(DISPLAY_TYPE_BUILT_IN, percent);
}

int CSettingsBase::getDisplayPercent()
{
  return CJNISettings::System::getInt(CJNIContext::getContentResolver(), "display_area_ratio", 100);
}

bool CSettingsBase::getEnablePassThrough()
{
  return CJNISettings::System::getInt(CJNIContext::getContentResolver(), "enable_pass_through");
}

void CSettingsBase::setEnablePassThrough(const bool enable)
{
  CJNISettings::System::putInt(CJNIContext::getContentResolver(), "enable_pass_through", enable);
}

void CSettingsBase::setDisplayContrast( const int contrast )
{
  CJNISettings::System::putInt(CJNIContext::getContentResolver(), "color_contrast", contrast);

  CJNIDisplayManager displayManager(CJNIContext::getSystemService("display"));
  displayManager.setDisplayContrast(DISPLAY_TYPE_BUILT_IN, contrast);
}

int CSettingsBase::getDisplayContrast()
{
  return CJNISettings::System::getInt(CJNIContext::getContentResolver(), "color_contrast", 100);
}

void CSettingsBase::setDisplayBright( const int bright )
{
  CJNISettings::System::putInt(CJNIContext::getContentResolver(), "color_brightness", bright);

  CJNIDisplayManager displayManager(CJNIContext::getSystemService("display"));
  displayManager.setDisplayBright(DISPLAY_TYPE_BUILT_IN, bright);
}

int CSettingsBase::getDisplayBright()
{
  return CJNISettings::System::getInt(CJNIContext::getContentResolver(), "color_brightness", 100);
}

bool CSettingsBase::isSysTime24()
{
  const std::string strValue = CJNISettings::System::getString(CJNIContext::getContentResolver()
	, CJNISettings::System::TIME_12_24);

  if (!strValue.empty())
  {
    if(strValue == "24")
	return true;	
  }
  return false;
}

