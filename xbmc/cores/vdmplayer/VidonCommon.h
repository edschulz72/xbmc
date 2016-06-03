#ifndef __VIDON_COMMON_H__
#define __VIDON_COMMON_H__

namespace libvp
{

enum PLAYER_TYPE
{
  PLAYER_CORE,
  PLCYER_LOCAL,
};

enum SOURCE_TYPE
{
  SOURCE_NET,
  SOURCE_LOCAL,
};

enum SUBTITLE_TYPE
{
  SUBT_UNINITIALIZED,
  SUBT_NO,
  SUBT_INNER,
  SUBT_OUTER,
  SUBT_RENDER,
};

struct struVidonDevice
{
  const char*		szDeviceName;
  const char*		szDeviceVersion;
  const char*   szDeviceType;
  const char*		szClientName;
  const char*		szClientVersion;
  const char*   max_resolution;
  const char*   deviceresolution;
  const char*		szNetworkMode; 
  double	fFps;
};

struct struVidonSetting
{
  const char*		szAudioLanguage;
  const char*		szSubtitleLanguage;
  const char*	  szQuality;
  const char*   szAudioSelect;
  const char*   szSubTitleSelect;
  int           nSubtitleType;
  const char*		szOutSubPath;
  const char*		szOutSubCodePage;
  bool		      bAudioHD;
  int			      nMaxBitrate;
  bool			    bCloseSubtitle;
};

struct vidonPlayerParam
{
  const char*       szIP;
  unsigned int      nVtxPort;
  unsigned int      nTcpPort;
  unsigned int      tokenLocal;
  unsigned int      tokenRemote;

  const char*       szMovie;
  const char*       szMode;

  PLAYER_TYPE       playerType;
  SOURCE_TYPE       sourceType;

  bool              bIsad;

  const char*       strSaveFilePath;
  int64_t			      nRemainLen;
  bool              bDownloadOutSubtitle;
  bool              bBreakPoint;
  const char*       strOriChannelid;

  const char*       strlogPath;

  int               nMenuMode;
  int               nPlayList;

  bool              bSetOutsubtitleForBOX;
  struVidonDevice   deviceParam;
  struVidonSetting  settingParam;
};

struct vidonOutInfo
{
  const char *szOutType;
  const char *szResolution;
  int         nBitrate;
  int64_t     nStartTime;
  int         nOutQuality;//0 convert,1 copy 
};

}
#endif