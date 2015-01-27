#ifndef __LIBPLAYERCORE_IPLAYER_H__
#define __LIBPLAYERCORE_IPLAYER_H__

#include <map>
#include <string>
#include <vector>


//==========================================================================================================================//
// Indexs
// A. enums 
// B. callback define
// C. data struct
// D. other data struct 
// E. class CCorePlayer



//==========================================================================================================================//
// A. enums

#define MAINMOVIE_PLAYLIST 0xFFFFF

#define MODE_NORMAL		   0
#define MODE_STRETCH_4x3   2
#define MODE_WIDE_ZOOM     3
#define MODE_STRETCH_16x9  4
#define MODE_ORIGINAL      5

//2D
#define ALLWINNER_DISPLAYMODE_2D_LEFT 			        0x1             
#define ALLWINNER_DISPLAYMODE_2D_RIGHT			        (0x1<<1)
#define ALLWINNER_DISPLAYMODE_2D_FULL   		        (0x1<<2)

//3D
#define ALLWINNER_DISPLAYMODE_3D_LEFT_RIGHT		        (0x1<<3)
#define ALLWINNER_DISPLAYMODE_3D_TOP_BOTTOM             (0x1<<4)
#define ALLWINNER_DISPLAYMODE_3D_DUAL_STREAM            (0x1<<5)
#define ALLWINNER_DISPLAYMODE_3D_MODE_LINE_INTERLEAVE   (0x1<<6)
#define ALLWINNER_DISPLAYMODE_3D_MODE_COLUME_INTERLEAVE (0x1<<7)

//3D ANAGLAGH
#define ALLWINNER_DISPLAYMODE_3D_MODE_SIDE_BY_SIDE      (0x1<<8)
#define ALLWINNER_DISPLAYMODE_3D_MODE_TOP_TO_BOTTOM     (0x1<<9)

#define ALLWINNER_DISPLAYMODE_3D_ANAGLAGH_RED_BLUE      (0x1<<0) 
#define ALLWINNER_DISPLAYMODE_3D_ANAGLAGH_RED_GREEN     (0x1<<1)
#define ALLWINNER_DISPLAYMODE_3D_ANAGLAGH_RED_CYAN      (0x1<<2)
#define ALLWINNER_DISPLAYMODE_3D_ANAGLAGH_COLOR         (0x1<<3)
#define ALLWINNER_DISPLAYMODE_3D_ANAGLAGH_HALF_COLOR    (0x1<<4)
#define ALLWINNER_DISPLAYMODE_3D_ANAGLAGH_OPTIMIZED     (0x1<<5)
#define ALLWINNER_DISPLAYMODE_3D_ANAGLAGH_YELLOW_BLUE   (0x1<<6)

//==========================================================================================================================//
// B. callback define

typedef void (*PFN_OnPlayBackEnded)(void* owner);
typedef void (*PFN_OnPlayBackStarted)(int type, void* owner);
typedef void (*PFN_OnPlayBackPaused)(void* owner);
typedef void (*PFN_OnPlayBackResumed)(void* owner);
typedef void (*PFN_OnPlayBackStopped)(void* owner);
typedef void (*PFN_OnPlayBackFailed)(int code, void* owner);
typedef void (*PFN_OnQueueNextItem)(void* owner);
typedef void (*PFN_OnPlayBackSeek)(int iTime, int seekOffset, void* owner);
typedef void (*PFN_OnPlayBackSeekChapter)(int iChapter, void* owner);
typedef void (*PFN_OnPlayBackSpeedChanged)(int iSpeed, void* owner);
typedef void (*PFN_OnCaptureResult)(char* sfile, bool bok, bool bbuffer, int nwidth, int height, void* owner);
typedef void (*PFN_OnNewVideoFrame)(void* owner);
typedef void (*PFN_OnPlaylistOpened)(void* owner);
typedef void (*PFN_OnVideoCodecOpened)(void* owner, int ndevice);
typedef void (*PFN_OnSetCursor)(void* owner, bool bSelected);

typedef void (*PFN_ONAudioCallbackInitialize)(void* owner, int iChannels, int iSamplesPerSec, int iBitsPerSample);
typedef void (*PFN_ONAudioCallbackAudioData)(void* owner, const float* pAudioData, int iAudioDataLength);
typedef void (*PFN_OnCacheState)(double cache_bytes, double cache_level, double cache_delay, void* owner);

struct ICorePlayerCallback
{
  ICorePlayerCallback()
  {
    cbowner = NULL;
    pfn_OnPlayBackEnded = NULL;
    pfn_OnPlayBackStarted = NULL;
    pfn_OnPlayBackPaused = NULL;
    pfn_OnPlayBackResumed = NULL;
    pfn_OnPlayBackStopped = NULL;
    pfn_OnPlayBackFailed = NULL;
    pfn_OnQueueNextItem = NULL;
    pfn_OnPlayBackSeek = NULL;
    pfn_OnPlayBackSeekChapter = NULL;
    pfn_OnPlayBackSpeedChanged = NULL;
    pfn_OnCaptureResult = NULL;
    pfn_OnCacheState = NULL ;
    pfn_OnNewVideoFrame = NULL;
    pfn_OnPlaylistOpened = NULL;
    pfn_OnVideoCodecOpened = NULL;
    pfn_OnSetCursor = NULL;
  }


  PFN_OnPlayBackEnded		pfn_OnPlayBackEnded;
  PFN_OnPlayBackStarted	pfn_OnPlayBackStarted;
  PFN_OnPlayBackPaused	pfn_OnPlayBackPaused;
  PFN_OnPlayBackResumed	pfn_OnPlayBackResumed;
  PFN_OnPlayBackStopped	pfn_OnPlayBackStopped;
  PFN_OnPlayBackFailed	pfn_OnPlayBackFailed;
  PFN_OnQueueNextItem		pfn_OnQueueNextItem;
  PFN_OnPlayBackSeek		pfn_OnPlayBackSeek;
  PFN_OnPlayBackSeekChapter	pfn_OnPlayBackSeekChapter;
  PFN_OnPlayBackSpeedChanged	pfn_OnPlayBackSpeedChanged;
  PFN_OnCaptureResult         pfn_OnCaptureResult;
  PFN_OnCacheState            pfn_OnCacheState;
  PFN_OnNewVideoFrame		pfn_OnNewVideoFrame;
  PFN_OnPlaylistOpened	pfn_OnPlaylistOpened;
  PFN_OnVideoCodecOpened	pfn_OnVideoCodecOpened;
  PFN_OnSetCursor             pfn_OnSetCursor;
  void*                       cbowner;
};

struct ICorePlayerAudioCallback
{

  ICorePlayerAudioCallback()
  {
    pfn_initialize = NULL;
    pfn_audiodata = NULL;
    cbowner = NULL;
  }

  PFN_ONAudioCallbackInitialize pfn_initialize;
  PFN_ONAudioCallbackAudioData  pfn_audiodata;

  void*                         cbowner;
};


//==========================================================================================================================//
// C. data struct

// __OUTER_URL_SUPPORT__
// read, stream interface. and this data-struct is be used as baseclass

struct CCoreStreamURL
{
  const char* name;

  //read File interface

  int (*url_open)(CCoreStreamURL* h, const char* url, int flags);
  int (*url_read)(CCoreStreamURL* h, unsigned char* buf, int size);
#if defined(TARGET_ANDROID_GENERIC)
  int64_t(*url_seek)(CCoreStreamURL* h, int64_t pos, int whence);
  int64_t(*url_timeseek)(CCoreStreamURL* h, int ms);
#else
  long long(*url_seek)(CCoreStreamURL* h, long long pos, int whence);
  long long(*url_timeseek)(CCoreStreamURL* h, int ms);
#endif
  int (*url_close)(CCoreStreamURL* h);

  //stream interface

  bool (*url_is_eof)(CCoreStreamURL* h);
  int (*url_get_totaltime_ms)(CCoreStreamURL* h);
  int (*url_get_time_ms)(CCoreStreamURL* h);

  int (*url_get_chapter)(CCoreStreamURL* h);
  int (*url_get_chapter_count)(CCoreStreamURL* h);
  bool(*url_seek_chapter)(CCoreStreamURL* h, int ch);

#if defined(TARGET_ANDROID_GENERIC)
  int64_t(*url_get_length)(CCoreStreamURL* h);
#else
  long long (*url_get_length)(CCoreStreamURL* h);
#endif

};

// player options







//////////////////////////////////////////////////////////////////////////

enum
{
  PLAYING_MODE_NONE,
  PLAYING_MODE_PLAYERCORE,
  PLAYING_MODE_SYSTEM,
};

enum
{
  RENDER_DATIO_SOURCE,
  RENDER_DATIO_SCREEN,
};

enum ExSubtitleAlign
{
  ExSUBTITLE_ALIGN_MANUAL = 0,
  ExSUBTITLE_ALIGN_BOTTOM_INSIDE,
  ExSUBTITLE_ALIGN_BOTTOM_OUTSIDE,
  ExSUBTITLE_ALIGN_TOP_INSIDE,
  ExSUBTITLE_ALIGN_TOP_OUTSIDE
};

enum MediaStreamType
{
  MediaStream_NONE,    // if unknown
  MediaStream_AUDIO,   // audio stream
  MediaStream_VIDEO,   // video stream
  MediaStream_DATA,    // data stream
  MediaStream_SUBTITLE,// subtitle stream
  MediaStream_TELETEXT // Teletext data stream
};

enum
{
  PLAYER_TYPE_VIDEO,
  PLAYER_TYPE_AUDIO
};

#define MAINMOVIE_PLAYLIST 0xFFFFF

struct IPlayerOptions
{
  IPlayerOptions()
  {
    starttime = 0LL;
    startpercent = 0LL;
    //		identify = false;
    fullscreen = false;
    video_only = false;
    url = NULL;
    playlist = -1;
    spropertys = NULL;
    strcpy(subtitlepath, "");

    restoreAudioId    = -1;
    restoreSubtitleId = -1;
  }

  double  starttime; /* start time in seconds */
  double  startpercent; /* start time in percent */
  //	bool    identify;  /* identify mode, used for checking format and length of a file */
  //	CStdString state;  /* potential playerstate to restore to */
  bool    fullscreen; /* player is allowed to switch to fullscreen */
  bool    video_only; /* player is not allowed to play audio streams, video streams only */
  char    username[255];
  char    password[255];

  CCoreStreamURL* url;

  int     playlist;				//specify dvd or bluray playlist
  bool    b3dmode;                            //using 3D mode to play it if support it (player need using libplayercore provide function to check it)
  int     restoreAudioId;
  int     restoreSubtitleId;
  char    subtitlepath[255*2];
  const char* spropertys;

  int displayMode;

};

// img items
#define MAX_PATH_NUM 1024

struct WaittingImgItems
{
  int  index;
  char strimgpath[MAX_PATH_NUM];
  int  timeout;
  WaittingImgItems* next;
};

typedef struct
{
  //StreamType type;
  int		 type;
  int          type_index;
  std::string  filename;
  std::string  filename2;  // for vobsub subtitles, 2 files are necessary (idx/sub)
  std::string  language;
  std::string  name;
  //CDemuxStream::EFlags flags;
  int		     flags;
  int          source;
  int          id;
  std::string  codec;
  int          channels;

  //add detailed info.write by supy
  int			 width;
  int			 height;
  double		 framerate;
  int			 samplerate;
  int			 bitrate;

} MediaStreamInfo;










//////////////////////////////////////////////////////////////////////////




//file :
//x: /xxxx/xx.mkv
//x: /xxxx/xx.iso          --  auto check Bluray / DVD / ?
//
//folder:
//x: /xxxx /               --  auto check folder
//x: /xxxx / BDMV /        --  BLURAY / AVCHD
//x: /xxxx / BDAV /        --  BDAV
//x: /xxxx / VIDEO_TS /    --  DVD
//
//disc:
//x:/                      --  auto check Bluray / DVD / ?

//default is playing menu, if want to play specify playlist, need specify playlist in IPlayerOptions
//if want playing mainmovie, set playlist = INT_MAX


struct SourceFileItem
{
  //source information 

  const char*     pszFile;
  const char*     pszMimeType;
  int             sourceType;

  //player information 

  int             playerType;


  //options 

  IPlayerOptions* pOptions;

  // extra information, mainly from fileItem class

  int startOffset;    //for some special audio case, like ape, flac which using cue to describe the file
  int endOffset;
  int startPartNumber;

  bool bMainTitle;
};

typedef struct
{
  //UI idx
  int fontsize;
  int fontstyle;
  int fontcolor;
  int fontalign;

  //other attribute 
  //TODO

}SubAttribute;

//==========================================================================================================================//
// D. other data struct 

enum
{
  PLAYERCORE_MODE_DEFAULT,                        //playercore mode, using for player app
  PLAYERCORE_MODE_PLUGIN_PLAYER,                  //it's the external player 
  PLAYERCORE_MODE_PREVIEW,                        //it's the preview engien, using to support other app
  PLAYERCORE_MODE_MEDIACORE,                      //using as mediacore, not playing function
};

enum
{
  PLAYERCORE_FONT_STYLE_NORMAL = 0,
  PLAYERCORE_FONT_STYLE_BOLD,
  PLAYERCORE_FONT_STYLE_ITALICS,
};

enum
{
  PLAYERCORE_ALIGN_MANUAL = 0,
  PLAYERCORE_ALIGN_BOTTOM_INSIDE,
  PLAYERCORE_ALIGN_BOTTOM_OUTSIDE,
  PLAYERCORE_ALIGN_TOP_INSIDE,
  PLAYERCORE_ALIGN_TOP_OUTSIDE
};

enum
{
  PLAYERCORE_LOG_APP_DISABLE    = -1,         // all log output disable
  PLAYERCORE_LOG_APP_NOLIMITED  = 0,          // all log will be output
  PLAYERCORE_LOG_APP_PREVIEW    = 1,          // only app preview log will be output
  PLAYERCORE_LOG_APP_PLAY2      = 2,          // only PLAY2 log will be output
  PLAYERCORE_LOG_APP_VIDONME    = 3,          // only VIDONME log will be output
  PLAYERCORE_LOG_APP_MOBILE     = 4,          // only ios/android log will be output
};

enum 
{
  PLAYERCORE_LOG_MODE_INTERNAL_ORIGIN,          //internal log mode, using new file everytime and the old file name will be changed
  PLAYERCORE_LOG_MODE_INTERNAL_LIMITED_SIZE,    //internal log mode, only one file, when log file size is > limited size, create new file
  PLAYERCORE_LOG_MODE_OUTER,
};

enum 
{
  PLAYERCORE_LOG_LEVEL_RELEASE    = 0,
  PLAYERCORE_LOG_LEVEL_DEBUG      = 1,
};

//authentication status
enum auth_status
{
  auth_status_connect_failed    = 0,   //网络连接错误
  auth_status_success			  = 1,   //认证成功  与老版本兼容
  auth_status_force_update      = 2,   //强制升级
  auth_status_unknown_error     = 3,   //未知错误

  // (100-199为用户账号类型错误)
  auth_status_invalid_username  = 100, //用户不存在
  auth_status_invalid_password  = 101, //密码错误
  auth_status_unauth_user       = 102, //未验证用户
  auth_status_illegal_user      = 103, //非法用户

  //以后扩展
  ///(200-299为chip id类型错误)
  //(300-299为xbmc 版本类型错误)
  //(400-499为sdk  版本类型错误)

};

enum
{
  PLAYER_SOURCE_TYPE_FILE,
  PLAYER_SOURCE_TYPE_FOLDER,
  PLAYER_SOURCE_TYPE_DISC,
};

#if defined(TARGET_WINDOWS) || defined(OS_WIN) || defined(OS_MACOSX)
//windows have the define in windef.h
#else
typedef unsigned int COLORREF;
#endif

#define RGBA(r,g,b,a)          ((COLORREF)((((unsigned char)(b)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(r))<<16)))) | ((unsigned int)((unsigned int)(a) << 24))



//authentication gui prompt callback add by yong.li 2014.5.19
typedef void (*PFN_GUI_PROMPT_CALLBACK)(auth_status status );

//log callback, loglevel = xbmc loglevel
typedef void (*PFN_LOG_CALLBACK)(int loglevel, const char* pstr, int console_id);

//real_log, using for show realtime log
typedef void (*PFN_REAL_LOG_CALLBACK)(int loglevel, const char* pstr);

#if defined(TARGET_ANDROID)
typedef int GLint;
typedef GLint(*PFN_OUTER_GUISHADER_CALLBACK)(int param1, int param2);
typedef int (*PFN_OUTER_GUISETTING_GETINT_CALLBACK)(const char* strSetting);
typedef int (*PFN_OUTER_SETDISPLAY3DMODE_CALLBACK)(int type);
#endif

//set mouse status callback, coming from bluray-hdmv menu
typedef void (*PFN_MOUSE_STATUS_CALLBACK)(bool);

//set bdj callback
typedef void (*PFN_BDJ_GOTOMENU_FINISH_CALLBACK)(int);
typedef void (*PFN_BDJ_SET_TITLE_CALLBACK)(int);
typedef void (*PFN_BDJ_STOPWAIT_CALLBACK)();
typedef void (*PFN_BDJ_KEYBOARD_EVENT_CALLBACK)(unsigned int keycode);
typedef void (*PFN_BDJ_WAIT_EVENT_CALLBACK)();
typedef void (*PFN_BDJ_MENU_EVENT_CALLBACK)();

//outer filesystem interface

#ifndef int64_t
typedef long long int64_t;
#endif

struct ExternalFileHandle;
struct ExternalDirectoryHandle;

typedef struct stExternalFilesystem
{

  //file interface

  ExternalFileHandle* (*Open)(const char* path);
  void (*Close)(ExternalFileHandle* h);

  int64_t (*Seek)(ExternalFileHandle* h, int64_t iFilePosition, int iWhence);
  unsigned int (*Read)(ExternalFileHandle* h, void* lpBuf, int64_t uiBufSize, int flags);
  int (*Stat)(ExternalFileHandle* h, struct __stat64* buffer);
  int (*Truncate)(ExternalFileHandle* h, int64_t size);
  int64_t (*GetLength)(ExternalFileHandle* h);
  int64_t (*GetPosition)(ExternalFileHandle* h);
  int (*GetChunkSize)(ExternalFileHandle* h);

  bool (*Exists)(const char* path);
  int (*StatEx)(const char* path, struct __stat64* buffer);


  //directory interface

  ExternalDirectoryHandle* (*GetDirectory)(const char* path);
  int (*GetDirectoryItemCount)(ExternalDirectoryHandle* handle);
  const char* (*GetDirectoryItemLabel)(ExternalDirectoryHandle* handle, int index);
  const char* (*GetDirectoryItemAttr)(ExternalDirectoryHandle* handle, int index, bool* bIsFolder);
  void (*CloseGetDirectory)(ExternalDirectoryHandle* handle);


  bool (*CreateDirectory)(const char* path);
  bool (*ExistsDirectory)(const char* path);
  bool (*RemoveDirectory)(const char* path);
  bool (*IsAllowedDirectory)(const char* filePath);

} ExternalFilesystem;


//playercore config

typedef struct stPlayercoreConfig
{
  stPlayercoreConfig()
  {
    pfn_gui_prompt_callback = NULL;

    fontsize = 30;
    fontdistance = 5;
    bfontborder = true;
    bOverrideASS = false;

    fontstyle = PLAYERCORE_FONT_STYLE_NORMAL;
    fontcolor = RGBA(255, 255, 0, 255);
    BorderColor = RGBA(255, 255, 255, 255);
    fontposition = PLAYERCORE_ALIGN_BOTTOM_OUTSIDE;
    strcpy(charset, "GB18030");
    strcpy(fontpath, "default");

    //log setting
    log_app = PLAYERCORE_LOG_APP_DISABLE;
    log_mode = PLAYERCORE_LOG_MODE_INTERNAL_ORIGIN;
    log_level = PLAYERCORE_LOG_LEVEL_RELEASE;
    log_filesize_mb = 5;                                    //5MB
    memset(pszLogFilename, 0, 255);
    pfn_log_callback = NULL;
    pfn_real_log_callback = NULL;
    bEnableConsoleLogger = false;

    pfn_mouse_status = NULL;
    externalFilesystem = NULL;

    //user information 
    bInternationalAccount = true;
    memset(pszUserName, 0 , 255);
    memset(pszUserPassword, 0 , 255);
    memset(pszAppVersion, 0 , 255);
    memset(pszUserEmail, 0 , 255);

    //package information
    memset(pszPackageName, 0, 255);
    memset(pszAppName, 0, 255);
  }
  //playercore using which working mode
  int working_mode;

  //specify path
  //MAX_PATH
  char pszHomePath[255];				//root folder, is the same with .exe, using this path to resolve the bin-path
  char pszUserFolderPath[255];		//log, other userdata will be put this file, for windows : common is ApplicationData / App
  char pszTempFolderPath[255];		//temp file folder

  //outer window handle, for windows is HWND, TODO: mac,ios,android
  void* outerWindowHandle;

  //start window position. size
  int width;
  int height;

  //add by yong.li for authentication
  PFN_GUI_PROMPT_CALLBACK pfn_gui_prompt_callback;

#if defined(TARGET_ANDROID)
  PFN_OUTER_GUISHADER_CALLBACK pfn_guishader_callback;
  PFN_OUTER_GUISETTING_GETINT_CALLBACK pfn_guisetting_getint_callback;
#endif

  //hardware accelerate mask, check __HARDWARE_ACCELERATE_CONTROL__ will find changed code
#define DISABLE_HARDWARE_ACCELERATE             0x0000
#define ENABLE_HARDWARE_ACCELERATE_DXVA         0x0001
#define ENABLE_HARDWARE_ACCELEARTE_VDA          0x0002

  int	 hardwareAccelerateMask;

  //using win32, maybe some client not define TARGET_WINDOWS
#if defined (_WIN32)
  void* d3d9;
  void* d3d9device;
#endif

  int   fontsize;
  int   fontstyle;
  int   fontdistance;
  unsigned int fontcolor;
  unsigned int BorderColor;
  int   fontposition;                             ////0 Fixed, 1 Bottom of video, 2 Below video, 3 Top of video, 4 Above video
  char  charset[255];
  char  fontpath[255];
  bool  bfontborder;
  bool  bOverrideASS;                              //add by yong.li vidon xbmc option 

  int     log_app;                                    //which app call this playercore, and just output specify log (PLAYERCORE_LOG_APP_DVDFAB)
  int     log_mode;
  int     log_level;                                  //log level current only support two mode {release=0, debug>=1}    
  int     log_filesize_mb;                            //user specify the log limited filesize, size unit is MB, default value is 5MB
  char    pszLogFilename[255];                        //user specify the log filename, if not specify it, will using plcore as log filename
  bool	bEnableConsoleLogger;                       //special mode: option for enable/disable console logger, only valid in windows/debug

  PFN_LOG_CALLBACK		      pfn_log_callback;       //log callback , check __LOG_CALLBACK__ will find all changed code
  PFN_REAL_LOG_CALLBACK	      pfn_real_log_callback;  //realtime logcallback, only enable for debug mode
  PFN_MOUSE_STATUS_CALLBACK     pfn_mouse_status;       //mouse status callback

  PFN_BDJ_GOTOMENU_FINISH_CALLBACK    pfn_bdj_gotomenu_finish;
  PFN_BDJ_SET_TITLE_CALLBACK          pfn_bdj_set_title;
  PFN_BDJ_STOPWAIT_CALLBACK           pfn_bdj_stop_wait;
  PFN_BDJ_KEYBOARD_EVENT_CALLBACK     pfn_bdj_keyboard_event;
  PFN_BDJ_WAIT_EVENT_CALLBACK         pfn_bdj_wait_event;
  PFN_BDJ_MENU_EVENT_CALLBACK         pfn_bdj_menu_event;

  ExternalFilesystem*         externalFilesystem;     //ExternalFile filesystem support

  //user information 

  bool bInternationalAccount;
  char pszUserName[255];
  char pszUserPassword[255];
  char pszAppVersion[255];
  char pszUserEmail[255];

  //client information
  char pszPackageName[255];
  char pszAppName[255];

} PlayercoreConfig;

//Audio settings API

enum
{
  AUDIO_OUT_TYPE_DECIED_BY_DEVICE     = 0x0000,
  AUDIO_OUT_TYPE_ANALOG		        = 0x0001,							//	Analog Output
  AUDIO_OUT_TYPE_DIGITAL		        = 0x0002,							//	Spdif / coax
  AUDIO_OUT_TYPE_HDMI			        = 0x0004,							//  hdmi
  AUDIO_OUT_TYPE_DISPLAYPORT	        = 0x0008,							//	displayport
};

enum
{
  AUDIO_CH_LAYOUT_INVALID = -1,

  AUDIO_CH_LAYOUT_1_0 = 0,
  AUDIO_CH_LAYOUT_2_0,
  AUDIO_CH_LAYOUT_2_1,
  AUDIO_CH_LAYOUT_3_0,
  AUDIO_CH_LAYOUT_3_1,
  AUDIO_CH_LAYOUT_4_0,
  AUDIO_CH_LAYOUT_4_1,
  AUDIO_CH_LAYOUT_5_0,
  AUDIO_CH_LAYOUT_5_1,
  AUDIO_CH_LAYOUT_7_0,
  AUDIO_CH_LAYOUT_7_1,

  AUDIO_CH_LAYOUT_MAX
};

enum
{
  PASSTHROUGH_CODEC_NONE			= 0x0000,
  PASSTHROUGH_CODEC_AC3				= 0x0001,
  PASSTHROUGH_CODEC_DTS				= 0x0002,
  PASSTHROUGH_CODEC_AAC				= 0x0004,
  PASSTHROUGH_CODEC_LPCM			= 0x0008,
  PASSTHROUGH_CODEC_TRUEHD		= 0x0010,
  PASSTHROUGH_CODEC_DTSHD			= 0x0020,
};

enum
{
  AUDIO_SINK_NONE,
  AUDIO_SINK_DIRECTSOUND,
  AUDIO_SINK_WASAPI,
};

enum
{
  DEVICE_TYPE_COMMON,
  DEVICE_TYPE_PASSTHROUGH,
  DEVICE_TYPE_ALL,
};

struct AudioDevice
{
  int			outType;
  char*		pszDeviceName;				//device name : using deviceName can sure which device is our selected and using
  char*		pszDisplayDeviceName;
};

//change to big value to support more device
#define MAX_DEVICES 40

struct AudioSetting
{
  AudioSetting()
  {
    supportOutTypes = AUDIO_OUT_TYPE_ANALOG;
    defaultDeviceIndex = -1;
    deviceCounts = 0;

    stereoToAll = false;
    speakers = AUDIO_CH_LAYOUT_INVALID;
    supportPassCodec = PASSTHROUGH_CODEC_NONE;
  };

  //basic information

  int	supportOutTypes;
  int defaultDeviceIndex;
  int deviceCounts;
  AudioDevice* pDevices[MAX_DEVICES];

  //global setting

  bool	stereoToAll;						// stereo to all speaker
  int		speakers;								// specify the speaker

  int		supportPassCodec;				// outer receiver support which type codec to passthrough

};

//common setting

enum SettingeKey
{
  //////////////////////////////////////////////////////////
  //basic setting 

  BASIC_SETTING_NONE                              = 0x0000,

  //network proxy 
  NETWORK_USE_HTTP_PROXY,
  NETWORK_HTTP_PROXY_SERVER,
  NETWORK_HTTP_PROXY_PORT,
  NETWORK_HTTP_PROXY_USERNAME,
  NETWORK_HTTP_PROXY_PASSWORD,

  //dvd auto menu, skip to introduction to menu
  BA_DVD_AUTOMENU,

  //////////////////////////////////////////////////////////
  //advance setting 

  ADVANCED_NONE                                   = 0x1000,



  //////////////////////////////////////////////////////////
  //custom setting 

  CUSTOMSIZE_SETTING_NONE                         = 0x2000,

  //audio, sub logic
  CS_DEFAULT_AUDIO_LANGUAGE,
  CS_DEFAULT_SUBTITLE_LANGUAGE,   
  CS_SUBTITLE_SEARCH_PATH,

  //smb interface
  CS_SAMBA_PASSWORD,
  CS_SAMBA_USERNAME,

};

//xsource setting

namespace XSource
{
  enum VIDEO_RES
  {
    VIDEO_RES_UNKNOWN					= 0,
    VIDEO_RES_480I						= 1,    // 720 x 480   interlace
    VIDEO_RES_576I						= 2,    // 720 x 576   interlace
    VIDEO_RES_480P						= 3,    // 720 x 480
    VIDEO_RES_1080I						= 4,    // 1920 x 1080 interlace
    VIDEO_RES_720						= 5,    // 1280 x 720
    VIDEO_RES_1080P						= 6,    // 1920 x 1080
    VIDEO_RES_576P						= 7,    // 720 x 576

    VIDEO_RES_704_480,
    VIDEO_RES_704_576,
    VIDEO_RES_352_480,
    VIDEO_RES_352_576,
    VIDEO_RES_352_240,
    VIDEO_RES_352_288,
  };

  enum VIDEO_AP
  {
    ASPECT_UNKNOWN					= 0,
    ASPECT_4X3						= 2,
    ASPECT_16X9						= 3,
    ASPECT_2D35X1                   = 4,    //2.35 : 1
    ASPECT_1D66X1                   = 5,    //1.66 : 1
    ASPECT_1D85X1                   = 6,    //1.85 : 1
    ASPECT_2D20X1                   = 7,    //2.20 : 1
  };

  enum VIDEO_FPS
  {
    FPS_UNKNOWN						= 0,
    FPS_23D976						= 1,    // 24000    / 1001
    FPS_24							= 2,    // 24       / 1
    FPS_25							= 3,    // 25       / 1
    FPS_29D97						= 4,    // 30000    / 1001
    FPS_30							= 5,    // 30       / 1
    FPS_50							= 6,    // 50       / 1
    FPS_59D94						= 7,    // 60000    / 1001
    FPS_60							= 8,    // 60       / 1
  };

  enum AUDIO_SAMPLING
  {
    SAMPLING_UNKNOWN				= 0,
    SAMPLING_48KHZ					= 1,
    SAMPLING_96KHZ					= 4,
    SAMPLING_192KHZ					= 5,
    SAMPLING_48_192KHZ				= 12,
    SAMPLING_48_96KHZ				= 14,
  };

  typedef std::map<std::string, std::string> MetaMap;



  enum
  {
    PLS_STREAM_TYPE_UNDEFINED,
    PLS_STREAM_TYPE_VIDEO,
    PLS_STREAM_TYPE_AUDIO,
    PLS_STREAM_TYPE_SUBPICTURE,
  };

  class CPLStream
  {
  public:

    CPLStream();
    virtual ~CPLStream();

    std::string m_language;
    std::string m_codec;
    int         m_id;               //stream_id, pid, not index 
    int         m_streamType;       
    int         m_bitrate;          //average bitrate

    //video attribute

    VIDEO_RES   m_res;              // value = VIDEO_xxx
    VIDEO_AP    m_aspect_ratio;     // value = ASPECT_xxx
    VIDEO_FPS   m_fpsFormat;        // value = FPS_xxx          , for dvd, this value is not Reliable
    int         m_width;
    int         m_height;
    bool        m_isInterlacedFrame;// this video source is interlaced
    bool        m_is10bitCodec;     // this video codec is 10bits 

    int         m_fpsValue;
    int         m_fpsScale;

    //audio attribute

    int              m_channels;         // value = channel count
    AUDIO_SAMPLING   m_samplerate;       // value = SAMPLING_xxx

  };

  typedef std::vector<CPLStream*> PLStreamPtrs;


  class CPlaylist
  {
  public:

    CPlaylist();
    ~CPlaylist();

  public:

    PLStreamPtrs m_streams;

    int m_durationMs;				//duration in millsecond
    int	m_Playlist;					//playlist value
    int	m_index;					//index in all playlist
    int m_angles;					//angle count of this playlist

    bool m_b3D;						//3D playlist
    bool m_bMainMovie;			    //Mainmovie
    bool m_bMenu;					//it's menu playlist

    int m_chapterCount;			    //

    int64_t m_totalSize;
    int m_averageBitrate;           //average bitrate for this playlist

  };

  typedef std::vector<CPlaylist*> PlaylistPtrs;


};


//==========================================================================================================================//
// E. class CCorePlayer

struct SPlayerAudioStreamInfo;
struct SPlayerVideoStreamInfo;
struct SPlayerSubtitleStreamInfo;
class IPlayerCallbackBridge;
class CCriticalSection;
struct C3DContext;

class CCorePlayer
{
public:

  CCorePlayer(ICorePlayerCallback* callback);
  virtual ~CCorePlayer();

  virtual bool InitDisplayDevice(void* outerwindow, int width, int height);

  virtual void RegisterAudioCallback(ICorePlayerAudioCallback* audiocallback);
  virtual void UnRegisterAudioCallback();

  virtual bool OpenFile(SourceFileItem* sourceItem);
  virtual bool CloseFile();

  virtual void GetVideoStreamInfo(SPlayerVideoStreamInfo &info,int maxStrSize);
  virtual void GetAudioStreamInfo(int index, SPlayerAudioStreamInfo &info,int maxStrSize);
  virtual void GetSubtitleStreamInfo(int index, SPlayerSubtitleStreamInfo &info,int maxStrSize);

  virtual bool CanOpenFile(const char* pszFile, const char* pszMimeType, IPlayerOptions* pOptions);

  virtual bool QueueNextFile(SourceFileItem* sourceItem);
  virtual void OnNothingToQueueNotify();

  virtual void Pause();
  virtual void Resume();
  virtual void Prev();
  virtual void Next();

#if defined (TARGET_DARWIN_IOS)
  virtual void Hangup(bool bhangup);
#endif

  virtual bool IsPlaying();
  virtual bool IsPaused();

  virtual bool HasVideo();
  virtual bool HasAudio();

  virtual double GetDuration();
  virtual double GetCurrentTime();

  //Pausing player before seek can make seek faster.
  //There are two situation:
  //1,Single seek.You should pause player before seek and resume player after seek.
  //2,Continuous seek.You should pause player before the first seek and resume after the last seek.

  //Seeking will faster if you set bAccurate to false, but that seeking is not accurate.
  virtual void SeektoTime(double time,bool bAccurate = true);
  virtual bool CaptureScreen(char* filename, int width, int height);

  virtual void ChangeDisplayMode(int mode);
  virtual void SetAudioVolume(float fVolume, bool isPercentage = true);
  virtual int	 GetAudioVolume();

  virtual void SetAudioLanguage(int nAudioIndex, int nAudioStreamID);
  virtual int  GetAudioLanguage();
  virtual void SetSubPicture(int nSubIndex,   int nSubStreamID);
  virtual int  GetSubPicture();
  virtual void CloseSubPicture();
  virtual void SetSubtitleVisible(bool bVisible);

  virtual void SetFullScreen(bool bShow);
  virtual bool GetFullScreen();
  virtual void ReSize(int cx, int cy);


  //function used by osx
  virtual void ReSizeOSX(int cx, int cy);
  virtual void FadeToBlack(void);
  virtual void FadeFromBlack(void);
  virtual void ReflushRenderer(void);
  //TARGET_DARWIN_OSX

  virtual void SetDisplayRatio(bool bRatio);
  virtual bool SetDisplayRatio(int nstyle);

  virtual void Move();

  virtual void RenderVideoStream();
  virtual void SetMute(bool bmute);

  virtual int  GetSubtitleCount();
  virtual int  GetSubtitleCurrentStream();
  virtual void GetSubtitleName(int iStream, char* strStreamName);
  virtual void GetSubtitleLanguage(int iStream, char* strStreamLang);
  virtual bool SelectSubtitle(int iStream);
  virtual bool SetSubtitleVisible(bool bvisable, char* strsubfile);
  virtual bool GetSubtitleVisible();

  virtual bool SetExternalSubtitlePath(const char* szsubpath);
  virtual bool SetExternalSubtitlePosition(int mode, int x = 0, int y = 0);

  virtual int  GetAudioStreamCount();
  virtual int  GetAudioCurrentStream();
  virtual void GetAudioStreamName(int iStream, char* strStreamName);
  virtual void GetAudioStreamLanguage(int iStream, char* strStreamLanguage);
  virtual bool SetAudioStream(int iStream);

  virtual void  SetPlaySpeed(float iSpeed);
  virtual float GetPlaySpeed();

  virtual int  GetChapterCount();
  virtual int  GetChapter();
  virtual void GetChapterName(char* strChapterName);
  virtual int  SeekChapter(int iChapter);

  //__LIBPLAYCORE_3D_SUPPORT__
  virtual bool Get3DGlobalEn();
  virtual void Set3DSourceType(int type);
  virtual int  Get3DSourceType();
  virtual int  GetSupportedPlayModes();
  virtual bool Set3DPlayMode(int mode);
  virtual int  Get3DPlayMode();
  //virtual void Get3DMode(C3DContext* ctx);

  virtual void  SetVideoCrop(int top, int bottom, int left, int right);
  virtual bool  GetVideoCrop(int* top, int* bottom, int* left, int* right);

  //add by jun.jiang 2014/4/25 Fix jira DMPFORMAC-284
  virtual void  SetDisplayHue(float fHue);/* 0-100 */
  virtual float GetDisplayHue();
  virtual void  SetDisplaySaturation(float fSaturation);/* 0-100 */
  virtual float GetDisplaySaturation();
  //end add

  virtual void  SetDisplayBrightness(float fBrightness);/* 0-100 */
  virtual float GetDisplayBrightness();

  virtual void  SetDisplayContrast(float fContrast);/* 0-100 */
  virtual float GetDisplayContrast();

  virtual void  SetZoomAmount(float Value);/* 0-100 */
  virtual float GetZoomAmount();

  virtual void  SetPixelRatio(float Value);/* 0-100 */
  virtual float GetPixelRatio();

  virtual void  SetRotationDegree(int ndegree);/*0,90,180,270*/
  virtual int   GetRotationDegree();

  virtual void  SetGetMirrorStyle(int nstyle);/*0(none)，1(horizontal)，2(vertical)*/
  virtual int   GetMirrorStyle();

  virtual void  SetVerticalShift(float Value);/*0(none)，1(horizontal)，2(vertical)*/
  virtual float GetVerticalShift();

  virtual void  SetAVDelay(float fdelay);
  virtual float GetAVDelay();

  virtual void  SetSubTitleDelay(float fdelay);
  virtual float GetSubTitleDelay();

  virtual void  SetVolumeAmplification(float va);
  virtual float GetVolumeAmplification();

  virtual float GetVideoAspectRatio();

  enum DEINTERLACEMODE
  {
    DEINTERLACEMODE_OFF = 0,
    DEINTERLACEMODE_AUTO = 1,
    DEINTERLACEMODE_FORCE = 2
  };
  virtual void SetDeinterlace(int nmode);

  virtual void EnableThreadRenderVideo(bool benable);

  virtual std::vector<MediaStreamInfo> GetStreamInfo(int type , bool bcurrent = false);
  virtual MediaStreamInfo* GetCurrentStreamInfo( int type);
  virtual void GetAllStreamInfo();

  //TODO, more data for action, please check CDVDPlayer::OnAction()
  //current only add action for bdj-bluray
  enum
  {
    PLAYERCORE_ACTION_NONE,
    PLAYERCORE_ACTION_MOVE_LEFT,
    PLAYERCORE_ACTION_MOVE_RIGHT,
    PLAYERCORE_ACTION_MOVE_UP,
    PLAYERCORE_ACTION_MOVE_DOWN,
    PLAYERCORE_ACTION_ENTER,
    PLAYERCORE_ACTION_MOUSE_MOVE,
    PLAYERCORE_ACTION_MOUSE_CLICK,
    PLAYERCORE_ACTION_NEXT_ITEM,
    PLAYERCORE_ACTION_PREV_ITEM,
    PLAYERCORE_ACTION_CHANNEL_UP,
    PLAYERCORE_ACTION_CHANNEL_DOWN,
    PLAYERCORE_ACTION_SHOWVIDEOMENU,
    //TODO, more action
  };

  virtual bool OnAction(int id, float x = 0, float y = 0);
  virtual bool ShowVideoMenu(bool bPop = false);
  virtual void AddWaittingImgInfo(WaittingImgItems* imgitem);

  //Android Allwinner using settings
  //TARGET_ANDROID
  //for source type
  enum
  {
    //* for 2D pictures.
    PLAYCORE_CEDARV_3D_MODE_NONE                             = 0,

    //* for double stream video like MVC and MJPEG.
    PLAYCORE_CEDARV_3D_MODE_DOUBLE_STREAM,

    //* for single stream video.
    PLAYCORE_CEDARV_3D_MODE_SIDE_BY_SIDE,
    PLAYCORE_CEDARV_3D_MODE_TOP_TO_BOTTOM,
    PLAYCORE_CEDARV_3D_MODE_LINE_INTERLEAVE,
    PLAYCORE_CEDARV_3D_MODE_COLUME_INTERLEAVE
  };

  //for output type
  enum
  {
    CEDARX_DISPLAY_3D_MODE_2D,
    CEDARX_DISPLAY_3D_MODE_3D,
    CEDARX_DISPLAY_3D_MODE_HALF_PICTURE,
    CEDARX_DISPLAY_3D_MODE_ANAGLAGH,
  };

  virtual bool SetInputDimensionType(int type);
  virtual bool SetOutputDimensionType(int type);
  virtual int  SetAllWinnerDisplayMode (int mode);
  //TARGET_ANDROID END

  virtual bool IsInMenu();

  //TARGET_WINDOWS, TARGET_OSX
  virtual void ShowHint(int x, int y, const char* smsg,int nfontszie, unsigned int color, unsigned int bordercolor);

  enum
  {
    DEVICE_INTEL,
    DEVICE_CUDA,
  };
  virtual bool GetVideoAccelerate(int nstype);

  virtual void GetAudioInfo(char * strAudioInfo,int &nlength);
  virtual void GetVideoInfo(char * strVideoInfo,int &nlength);
  virtual void GetGeneralInfo( char * strGeneralInfo,int &nlength);
  virtual void Seek(bool bPlus, bool bLargeStep);
  virtual void GetPlayerState(char * strState,int &nlength);
  virtual bool SetPlayerState(char* strState);
  virtual const char * GetCurrentSubtitle();
  virtual bool IsCaching();
  virtual int GetCacheLevel();
  virtual float GetCachePercentage();

  virtual int AddSubtitle(const char * strSubPath);

  virtual int GetPlayingPlaylist();                                       //Get current playing title

  virtual void UpdateSubtitleSetting(SubAttribute *subAttribute = NULL);

public:

  bool m_bManualClose;

protected:

  void ParsePropertys();

protected:

  int				        m_playing_mode;
  IPlayerCallbackBridge*	        m_callbackBridge;
  bool			        m_bPlaying;
  CCriticalSection*		        m_ObjSection;
  int                                 m_iPlaySpeed;
  int                                 m_PlayerType;

  std::string			        m_cursubtitlecontent;
  std::string				m_propertys;
  std::map<std::string, std::string>  m_mapproperty;

  std::vector<MediaStreamInfo>        m_videostreams;
  std::vector<MediaStreamInfo>        m_audiostreams;
  std::vector<MediaStreamInfo>        m_subtitlestreams;


    bool            m_b_has_watermark;
    unsigned char*  m_p_watermark_data;
    int             m_watermark_x;
    int             m_watermark_y;
    int             m_watermark_w;
    int             m_watermark_h;

public:

  virtual const char* GetSubtitlePath(int iStream);
  virtual void AddWaterMark(unsigned char* rgba, int width, int height, int x, int y);
  virtual void ClearWaterMark();

  virtual int  GetBDCurrentAngle();
  virtual int  GetBDAngleCount();
  virtual bool SelectBDAngle(unsigned int angle);

  virtual void SetRemoveFbiWarning(bool bGotoFbiWarning);
  virtual void SetGotoMenu(bool bGotoMenu);
  virtual bool isGotoMenuFinish();

public:

  static void SetDefaultAudioLanguage(const char* lang);
  static void SetDefaultSutitleLanguage(const char* lang);
  static void SetSubtitleSearchPath(const char* pszPath);

  void        SetSubtitleCharSet(const char* charset);
  const char* GetSubtitleCharSet();
};

#endif //__LIBPLAYERCORE_IPLAYER_H__
