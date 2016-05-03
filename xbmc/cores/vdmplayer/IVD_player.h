#ifndef __IVD_PLAYER_H__
#define __IVD_PLAYER_H__

#include <stddef.h>
//==========================================================================================================================//
// 
// NOTE:
// 1.client has to make sure any string that pass to plcore is UTF-8
//
// Indexs
// A. public API



//==========================================================================================================================//
// A. public API

#if defined(_WIN32) && defined(_WINDLL)
#define	DLL_PLCORE_API __declspec(dllexport)
#else
#define DLL_PLCORE_API
#endif

namespace libvp
{

class IVDPlayer;
class IVDPlayerConfig;

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Create VDPlayer object, just empty object, need using after Initialize
  * @return VDPlayer object
  */

DLL_PLCORE_API IVDPlayer*  CreateVDPlayer();

/** 
  * Release VDPlayer object 
  * @param IVDPlayer pointer which created by libplayercore library
  */

DLL_PLCORE_API void      ReleaseVDPlayer( IVDPlayer* pVDPlayer );


#ifdef __cplusplus
}
#endif


//==========================================================================================================================//
// IVDPlayer interface  
// 
// Indexs
// A. enum define
// B. class IVDPlayer
// C. interface IVDPlayerConfig


class IVDPlayerConfig;
class IVDPlayTool;
class IVDPlayToolConfig;
class IVDMediatool;
class IVDMediatoolConfig;
class IVDAddontool;
class IVDAddontoolConfig;
class IVDAESinkPlcore;
class IVDAESinkPlcoreConfig;
class IVDPlaytoolCallback;

//==========================================================================================================================//
//A. Enum define

enum 
{
	VD_SOURCE_TYPE_UNKNOWN,
	VD_SOURCE_TYPE_FILE,
	VD_SOURCE_TYPE_FOLDER,
	VD_SOURCE_TYPE_DISC,
};


enum VD_PathType 
{
  // plcore look for font file in here.
  VD_PATH_TYPE_FONT_FILE,

  // Plcore look for runtimes support in here.
  VD_PATH_TYPE_RUNTIMES,

  // plcore output log file in here.
  VD_PATH_TYPE_LOG,

  // plcore output some temp file in here
  VD_PATH_TYPE_TEMPFOLDER,

  // MAC client may should set this.
  VD_PATH_TYPE_FRAMEWORKS_FOLDER,

  // BD cache folder
  VD_PATH_TYPE_BD_CACHE,

  // BDJ resource folder
  VD_PATH_TYPE_BDJ_RESOURCE,

  // BDJ lib path
  VD_PATH_TYPE_BD_LIB,
};

enum VD_FileType 
{
	VD_FILE_TYPE_FONT_FILE, //Note: KODI UI setting "Font to use for text subtitles" should bind with this
	//Todo: add other file type
};

enum VD_PlayerType
{
  VD_PLAYER_PLCORE,
  VD_PLAYER_SYS,
};


enum VD_Mode
{
  VD_MODE_FULL = 0x000F, // Means client want to use all tools, this is plcore's default mode
  VD_MODE_PLAYTOOL = 0x0001, // Means client only want to use playtool
  VD_MODE_MEDIATOOL = 0x0002, // Means client only want to use mediatool

  // warning!!!
  // Do not use PL_MODE_ADDONTOOL and PL_MODE_AESINK for now.
  // Contact plcore developer if you want to use them.
  // Because plcore code does not handle them yet.
  VD_MODE_ADDONTOOL = 0x0004, // Means client only want to use addontool
  VD_MODE_AESINK = 0x0008, // Means client only want to use aesink
};

//==========================================================================================================================//
// B. IVDPlayer Interface


enum VD_ErrorCallbackType
{
  VD_ERROR_CALLBACK_TYPE_WRONG_USAGE, // Indicates client use plcore's interface in wrong way.
};
enum VD_DBS_CasSmasrCardStatus
{
  VD_DBS_CARD_IN,
  VD_DBS_CARD_OUT,
  VD_DBS_CARD_INVERSE,
  VD_DBS_CARD_CHECKING,
  VD_DBS_CARD_BUSY,
  VD_DBS_CARD_ERROR,
};
class IVDPlcoreCallback
{
public:
  IVDPlcoreCallback() {};
  virtual ~IVDPlcoreCallback() {};

  virtual void OnError(VD_ErrorCallbackType type, const char* strLastErrorDescription) = 0;
  virtual void NotifyDBSCardStatus(VD_DBS_CasSmasrCardStatus event,  void *pEventBody) = 0;
};

struct VDPlcoreDBStarEntitleInfo
{
  char m_OperatorID;
  int m_ID;
  char* m_ProductStartTime;
  char* m_ProductEndTime;
  char* m_WatchStartTime;
  char* m_WatchEndTime;
  int m_LimitTotaltValue;
  int m_LimitUsedValue;

  VDPlcoreDBStarEntitleInfo* next;

  VDPlcoreDBStarEntitleInfo()
  {
    m_OperatorID = 0;
    m_ID = 0;
    m_ProductStartTime = NULL;
    m_ProductEndTime = NULL;
    m_WatchStartTime = NULL;
    m_WatchEndTime = NULL;
    m_LimitTotaltValue = 0;
    m_LimitUsedValue = 0;
    next = NULL;
  }

  ~VDPlcoreDBStarEntitleInfo()
  {
    delete[] m_ProductStartTime;
    delete[] m_ProductEndTime;
    delete[] m_WatchStartTime;
    delete[] m_WatchEndTime;
    delete next;
  }
};
class IVDPlayer
{
public:

  IVDPlayer() {}
  virtual ~IVDPlayer() {}

  //////////////////////////////////////////////////////////////////////////
  //basic api

  /**
    * Initializes the IVDPlayer, client need call this Initialize
    * if failed, need release the object
    * @param client need implement IVDPlayerConfig interface and send to plcore to init
    */
  virtual bool Initialize( IVDPlayerConfig* pConfig, IVDPlcoreCallback* pVDPlayerCallback ) = 0;


  //////////////////////////////////////////////////////////////////////////
  //play tool api
  
  /**
    * create Play tool object
	* Caller must keep pConfig alive before call ReleasePlayer
    * @param client send object who is inherit IVDPlayToolConfig
    * @return if failed then return NULL 
    */
	virtual IVDPlayTool* CreatePlayTool(VD_PlayerType type,IVDPlayToolConfig* pConfig, IVDPlaytoolCallback* pCallback) = 0;


  /**
    * release Player object
    * @param IVDPlayTool object which is created by VDPlayer
    */
  virtual void ReleasePlayTool( IVDPlayTool* pTool ) = 0;


  //////////////////////////////////////////////////////////////////////////
  //media tool api
  

  /**
    * create mediatool object
    * @param client send object who is inherit IVDMediatoolConfig
    * @return if failed then return NULL 
    */
  virtual IVDMediatool* CreateMediatool( IVDMediatoolConfig* pConfig ) = 0;


  /**
    * release IVDMediatool object
    * @param IVDMediatool object which is created by VDPlayer
    */
  virtual void ReleaseMediatool( IVDMediatool* pTool ) = 0;


  //////////////////////////////////////////////////////////////////////////
  //addontool api
  
  /**
    * create IVDAddontool object
    * @param client send object who is inherit IVDAddontoolConfig
    * @return if failed then return NULL 
    */
  virtual IVDAddontool* CreateAddontool(/* IVDAddontoolConfig* pConfig */) = 0;


  /**
    * release IVDAddontool object
    * @param IVDAddontool object which is created by VDPlayer
    */
  virtual void ReleaseAddontool( IVDAddontool* pTool ) = 0;



  //////////////////////////////////////////////////////////////////////////
  // AESinkPlcore
  
  /**
    * create IVDAESinkPlcore
    * @param client send object who is inherit IVDAESinkPlcoreConfig
    * @return if failed then return NULL 
    */
  virtual IVDAESinkPlcore* CreateAESinkPlcore( IVDAESinkPlcoreConfig* pConfig ) = 0;


  /**
    * release IVDAESinkPlcore object
    * @param IVDAESinkPlcore object which is created by VDPlayer
    */
  virtual void ReleaseAESinkPlcore( IVDAESinkPlcore* pAESink ) = 0;

  // nLevel can be: LOGDEBUG   0, LOGINFO    1, LOGNOTICE  2, LOGWARNING 3, LOGERROR   4, LOGSEVERE  5, LOGFATAL   6
  // Release mode would not output LOGDEBUG log.
  virtual void OutputToLogFile( int nLevel, const char* strContent ) = 0;

  virtual char* GetDBStarCardSN() = 0;

  virtual int GetDBStarEntitleCounts() = 0;

  virtual VDPlcoreDBStarEntitleInfo* GetDBStarEntitleInfos() = 0;
protected:

  

};

//==========================================================================================================================//
// C. interface IVDPlayerConfig
// attention: client need implement class and using IVDPlayerConfig as base class


class IVDPlayerConfig
{
public:

	IVDPlayerConfig() {}
	virtual ~IVDPlayerConfig() {}

	//////////////////////////////////////////////////////////////////////////
	// client information

	/**
		* Get specify name
		* @return "VDPlayer" or others
		*/
	virtual const char* GetSpecifyName() = 0;


	/**
		* Get client app name, and tell plcore who is calling him now
		* it's the standard product name
		* @return "Player" or "XBMC"
		*/
	virtual const char* GetClientAppName() = 0;


	/**
		* Get client package name
		* it's the name which is used to identify the app, major use it in Android,
		* if not then return the same name with the clientAppName
		* @return "org.vidonme.xbmc" or others
		*/
	virtual const char* GetClientPackageName() = 0;


	/**
		* Get client version by string
		* @return "1002" or "2"
		*/
	virtual const char* GetClientVersion() = 0;


	//////////////////////////////////////////////////////////////////////////
	// user information 

	/**
	* Get account type
	* @return enum vaule
	*/
	enum
	{
		ACCOUNT_TYPE_INTERNATIONAL,
		ACCOUNT_TYPE_CHINA,
	};


	virtual int GetAccountType() = 0;


	/**
		* Get user name
		* @return "jack" or others
		*/
	virtual const char* GetUsername() = 0;


	/**
		* Get user password
		* Attention: the password should not be the plaintext
		* @return "$FDSFWERF$EW" or others
		*/
	virtual const char* GetUserPassword() = 0;


	/**
	* Get user email
	* @return "jack@gmail.com" or others
	*/
	virtual const char* GetUserEmail() = 0;


	//////////////////////////////////////////////////////////////////////////
	// basic setting: path

	/**
	* Get the paths that the playcore will use(e.g. font file path)
	*/
	virtual const char* GetPath(VD_PathType type) = 0;


	/**
	* Get the file names that playcore will use(e.g. font file)
	*/
	virtual const char* GetFileName(VD_FileType type) = 0;

	/*
	 *android need this, static_cast<JavaVM*>, static_cast<JNIEnv*>
	 */
	virtual void* GetJavaVM(){ return 0; };

	virtual void* GetJNIEnv(){ return 0; };

  virtual int GetPlcoreMode() { return (int)VD_MODE_FULL; }

	virtual void* GetObjDispManager(){ return 0;};

protected:

};

}

#endif //__IVD_PLAYER_H__
