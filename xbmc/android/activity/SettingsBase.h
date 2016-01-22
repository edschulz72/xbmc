#pragma once

#include <string>
#include <vector>

class CSettingsBase
{
public:
  struct LangNode
  {
    std::string strKey;
	std::string strDisplay;
	LangNode(const std::string& key
	  , const std::string& display)
	  : strKey(key)
	  , strDisplay(display)
	{
	}
  };
  typedef std::vector<LangNode> LangNodes;
  static LangNodes getLanguageList( void );
  static LangNode getCurrLanguage( void );
  static void setCurrLanguage(const std::string& strKey);
public:
  struct DisplayOutputModeNode
  {
    int nKey;
	std::string strDisplayName;  
	DisplayOutputModeNode(const int key
		, const std::string& display)
		: nKey(key)
		, strDisplayName(display)
	{
	
	}
  };
  typedef std::vector<DisplayOutputModeNode> DisplayOutputModeNodes;
  static DisplayOutputModeNodes GetDisplayOutputModeList( void );
  static DisplayOutputModeNode GetCurrDisplayOutputMode( void );
  static void SetCurrDisplayOutputMode(const int nKey);
  static bool saveCurrDisplayOutputMode(const int nKey);
  static void initCurrDisplayOutputMode();
  static void setDisplayPercent(const int percent);
  static int getDisplayPercent();
  static void setDisplayContrast(const int contrast);
  static int getDisplayContrast();
  static void setDisplayBright(const int bright);
  static int getDisplayBright();
  static bool getEnablePassThrough();
  static void setEnablePassThrough(const bool enable);
  static bool isSysTime24();
};
