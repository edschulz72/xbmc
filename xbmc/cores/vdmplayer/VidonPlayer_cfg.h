#pragma once

#include <string>
#include <jni.h>

#include "../IVD_player.h"
#include "../IVD_playtool.h"
#include "../IVD_coreplayer.h"
#include "../IVD_mediainfo.h"
#include "../IVD_mediatool.h"
//#include "../VidonDownLoadInterface.h"
using namespace libvp;

class CVDPlcoreCallback:public IVDPlcoreCallback
{
public:
  CVDPlcoreCallback() {};
  virtual ~CVDPlcoreCallback() {};

  virtual void OnError(VD_ErrorCallbackType type, const char* strLastErrorDescription);
private:
 jmethodID  m_OnPlayBackErrorID;
};

class CVDPlayerCallback : public IVDPlayCallback
{
public:
  CVDPlayerCallback();
  ~CVDPlayerCallback() {};

  void OnPlayBackEnded();
  void OnPlayBackStarted();
  void OnPlayBackPaused();
  void OnPlayBackResumed();
  void OnPlayBackStopped();
  void OnQueueNextItem()  {};
  void OnPlayBackSeek(int iTime, int seekOffset);
  void OnPlayBackSeekChapter(int iChapter) {};
  void OnPlayBackSpeedChanged(int iSpeed) {};
  void OnPlayBackFailed();
  void OnPlayBackDecodeModeNotify(bool Hw);
  void OnPlayBackSeekState(VD_SeekState state);
  void OnPlayBackHwDecodeFailed();

private:
  jmethodID m_OnPlayBackEndedID;
  jmethodID m_OnPlayBackStartedID;
  jmethodID m_OnPlayBackPausedID;
  jmethodID m_OnPlayBackResumedID;
  jmethodID m_OnPlayBackStoppedID;
  jmethodID m_OnQueueNextItemID;
  jmethodID m_OnPlayBackSeekID;
  jmethodID m_OnPlayBackSeekChapterID;
  jmethodID m_OnPlayBackSpeedChangedID;
  jmethodID m_OnPlayBackFailedID;
  jmethodID m_OnPlayBackDecodeModeNotify;
  jmethodID m_OnPlayBackSeekState;
  jmethodID m_OnPlayBackHwDecodeFailed;
};

class CVDPlayerConfig : public IVDPlayerConfig
{
public:
  CVDPlayerConfig(){
    m_nAccountType = ACCOUNT_TYPE_CHINA;
    m_strClientAppName = m_strClientPackageName = m_strClientVersion = m_strUserName = m_strUserPassword = m_strUserEmail = "Unknow";
    m_strSpecifyName = "vidonplayer";
  };
  virtual ~CVDPlayerConfig()
  {
  };

  virtual const char* GetSpecifyName(){return m_strSpecifyName.c_str();};
  virtual const char* GetClientAppName(){return m_strClientAppName.c_str();};
  virtual const char* GetClientPackageName(){return m_strClientPackageName.c_str();};
  virtual const char* GetClientVersion(){return m_strClientVersion.c_str();};

  virtual int GetAccountType(){return m_nAccountType;};
  virtual const char* GetUsername(){return m_strUserName.c_str();};
  virtual const char* GetUserPassword(){return m_strUserPassword.c_str();};
  virtual const char* GetUserEmail(){return m_strUserEmail.c_str();};

  virtual const char* GetPath(VD_PathType type);
  virtual const char* GetFileName(VD_FileType type);

  virtual void* GetJavaVM();
  virtual void* GetJNIEnv();
  virtual void* GetObjDispManager();

  std::string m_strSpecifyName;
  std::string m_strClientAppName;
  std::string m_strClientPackageName;
  std::string m_strClientVersion;
  int m_nAccountType;
  std::string m_strUserName;
  std::string m_strUserPassword;
  std::string m_strUserEmail;

  std::string m_strPlcoreRuntimePath;
  std::string m_strLogPath;
  std::string m_strFontPath;
  std::string m_strProfileFolderPath;
  std::string m_strFontName;
};

class CVDPlayToolConfig : public IVDPlayToolConfig
{
public:

  CVDPlayToolConfig(){m_hander = NULL; m_nWidth = m_nHeight = -1;};
  virtual ~CVDPlayToolConfig(){};
   virtual int GetRenderWindowInitWidth() const {return m_nWidth;};
   virtual int GetRenderWindowInitHeight() const{return m_nHeight;}
#if !defined(VDPLAYER_FOR_KODI)
   virtual void** GetRenderWindowHandle() const {return m_hander;};
#else
 
#if defined(HAS_DX)

  virtual LPDIRECT3D9 GetD3d9() const {return m_pD3D;};

  virtual LPDIRECT3DDEVICE9 GetD3d9Device() const {return m_pD3DDevice;};

  virtual const D3DADAPTER_IDENTIFIER9& GetAIdentifier() {return m_AIdentifier;};

  virtual DWORD GetDefaultD3DUsage() const {return m_nDefaultD3DUsage;};

  virtual D3DPOOL GetDefaultD3DPool() const {return m_defaultD3DPool;};

  virtual D3DPRESENT_PARAMETERS GetD3DPP() const {return m_d3dpp;};
#endif

  virtual unsigned int GetRenderCaps() const {return m_renderCaps;};

  virtual unsigned int GetMaxTextureSize() const {return m_maxTextureSize;};

  virtual int GetScreen() const {return 0;};

#if defined(HAS_GL)

  virtual unsigned int GetRenderQuirks() const {return m_renderQuirks;};
#endif


  virtual const char* GetRenderVendor() const {return "unknown";};

  virtual const char* GetRenderRenderer() const {return "unknown";};

  virtual void GetRenderVersion(unsigned int& major, unsigned int& minor) const {major = 0; minor=0;};

#if defined(TARGET_DARWIN_OSX)

  virtual void* GetGlContext() const {return m_pGlContext};
#endif

#if defined(HAS_GL) || defined(HAS_GLES)

  virtual const char* GetRenderExtensions() const {return m_strRenderExtensions.c_str();};
#endif

#if defined(HAS_EGL)

  virtual EGLDisplay GetEGLDisplay() {return m_EGLDisplay;};

  virtual EGLContext GetEGLContext() {return m_EGLContext;};
#endif
#endif // VDPLAYER_FOR_KODI

  void SetRenderWindowHandle(void**p) { m_hander = p;}
  void SetRenderWindowInitWidth(int w) { m_nWidth = w;}
  void SetRenderWindowInitHeight(int h) { m_nHeight = h;}

  void** m_hander;
  int m_nWidth;
  int m_nHeight;
#if defined(HAS_DX)
	LPDIRECT3D9 m_pD3D;
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	D3DADAPTER_IDENTIFIER9 m_AIdentifier;
	DWORD m_nDefaultD3DUsage;
	D3DPOOL m_defaultD3DPool;
	D3DPRESENT_PARAMETERS m_d3dpp;
#endif

unsigned int m_renderCaps;
unsigned int m_renderQuirks;
unsigned int        m_maxTextureSize;

#if defined(HAS_GL)
	unsigned int m_nRenderQuirks;
#endif

#if defined(TARGET_DARWIN_OSX)
	void* m_pGlContext;
#endif

#if defined(HAS_GL) || defined(HAS_GLES)
	std::string m_strRenderExtensions;
#endif

#if defined(HAS_EGL)
	EGLDisplay m_EGLDisplay;
	EGLContext m_EGLContext;
#endif
};


class CVDPlaytoolCallback : public IVDPlaytoolCallback
{
public:
  CVDPlaytoolCallback() {};
  virtual ~CVDPlaytoolCallback() {};
#ifdef VDPLAYER_FOR_KODI
	// Inside this, 
	//-----------------------------------------------------------------
	// kodi client has to add following code
	//
	// ThreadMessageCallback callbackData;
	// callbackData.callback = cb.callback;
	// callbackData.userptr = cb.userptr;

	// ThreadMessage msg;
	// msg.dwMessage = TMSG_CALLBACK;
	// msg.lpVoid = (void*)&callbackData;

	// wait for it.
	// CApplicationMessenger::Get().SendMessage(msg, bWaitBeforeDone);
	virtual void OnThreadMessageCallback(VD_ThreadMessageCallback& cb, bool bWaitBeforeDone ) {};


	// Inside this, 
	//-----------------------------------------------------------------
	// kodi client has to add following code
	//
	// ThreadMessage msg = {TMSG_RENDERER_FLUSH};
	// CApplicationMessenger::Get().SendMessage(msg, false);
	virtual void OnRenderFlush() {};

	//To deal with "stack playback"'s seek
	//Kodi client should call g_application.SeekTime() inside this.
	virtual void OnStackSeek(double time) {};

	//To get "stack playback"'s time
	//Kodi client should call g_application.GetTime() inside this.
	virtual double OnGetStackTime() {return 0.0;};

	// Inside this Kodi client should call "CApplicationMessenger::Get().SendAction(CAction(ACTION_TRIGGER_OSD), WINDOW_INVALID, false);".
	virtual void OnActionTriggerOsd() {};

	//Kodi client should call CApplicationMessenger::Get().SwitchToFullscreen() inside this method.
	virtual void OnSwitchToFullscreen() {};

	//Kodi client should call g_Windowing.SetViewPort() inside this method.
	virtual void OnSetViewPort(float x1, float y1, float x2, float y2) {};
	
	//Kodi client should call g_graphicsContext.SetVideoResolution() in its render thread.
	virtual void OnSetVideoResolution(VD_Resolution res, bool forceUpdate) {};

	//Kodi client should call g_graphicsContext.SetFullScreenVideo() in its render thread.
	virtual void OnSetFullScreenVideo(bool bFull) {};
	

#if defined(HAS_GLES)
	//Kodi client should call g_Windowing.EnableGUIShader() inside this method.
	virtual void OnEnableGUIShader(VD_ShaderMethod method) {};
	//Kodi client should call g_Windowing.DisableGUIShader() inside this method.
	virtual void OnDisableGUIShader() {};
	//Kodi client should call g_Windowing.GUIShaderGetPos() inside this method.
	virtual int OnGUIShaderGetPos() {return 0;};
	//Kodi client should call g_Windowing.GUIShaderGetCol() inside this method.
	virtual int OnGUIShaderGetCol() {return 0;};
	//Kodi client should call g_Windowing.GUIShaderGetCoord0() inside this method.
	virtual int OnGUIShaderGetCoord0() {return 0;};
	//Kodi client should call g_Windowing.GUIShaderGetUniCol() inside this method.
	virtual int OnGUIShaderGetUniCol() {return 0;};
	//Kodi client should call g_Windowing.GUIShaderGetCoord1() inside this method.
	virtual int OnGUIShaderGetCoord1() {return 0;};
	//Kodi client should call g_Windowing.GUIShaderGetCoord0Matrix() inside this method.
	virtual int OnGUIShaderGetCoord0Matrix() {return 0;};

#endif
#endif //VDPLAYER_FOR_KODI
};

class CVDMediatoolConfig : public IVDMediatoolConfig
{
public:

  CVDMediatoolConfig();
  virtual ~CVDMediatoolConfig(){}
  virtual const char* GetSourcePath(){ return m_strSourcePath.c_str(); }
  virtual int GetSourceType(){ return m_nSourceType; }
  virtual bool IsCancel(){ return m_bCancle; }
  virtual const char* GetThumbnailPath(int playlist = -1);
  virtual const char* GetMovieImagePath(){ return m_strImgePath.c_str(); }
  virtual int GetThumbnailPercentTime(){ return m_nThumbnailPercentTime; }
  virtual void GetThumbnailSize(int& nWidth, int& nHeight);
  virtual const char* GetMultiThumbnailPath(int nSeekTo, int& nIntervalTime, int playlist = -1);
  virtual int GetMinTimeLengthOfPlaylist(){ return m_nMinTimeLength; }

  int m_nThumbnailWidth;
  int m_nThumbnailHeight;
  int m_nThumbnailPercentTime;
  int m_nSourceType;
  int m_nIntervalTime;  //the measure is second
  int m_nMinTimeLength;

  std::string m_strThumnailPath;
  std::string m_strImgePath;
  std::string m_strSourcePath;
  bool m_bCancle;
};
//class DownloadCallback :public IDownloadCallback
//{
//public:
//  DownloadCallback(jobject obj);
//  ~DownloadCallback();
//
//  void OnDownloadStarted();
//  void OnDownloadStoped();
//
//  void OnDownloadPaused();
//  void OnDownloadResumed();
//
//  void OnDownloadFailed(DOWNLOAD_ERROR_CODE errodcode);
//  void OnDownloadFinished();
//private:
//  jmethodID m_OnDownloadStartedID;
//  jmethodID m_OnDownloadStopedID;
//  jmethodID m_OnDownloadPausedID;
//  jmethodID m_OnDownloadResumedID;
//  jmethodID m_OnDownloadFailedID;
//  jmethodID m_OnDownloadFinishedID;
//  jobject mObj;
//};

struct MediaInfo
{
  int64_t filesize;
  int duration;
  int16_t width;
  int16_t height;
  MediaInfo()
  {
    filesize = 0;
    duration = 0;
    width = 0;
    height = 0;
  };
};
