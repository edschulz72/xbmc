
#ifdef HAS_VIDONME

#pragma once

#include "cores/vdmplayer/IVD_player.h"
#include "cores/vdmplayer/IVD_playtool_decls.h"
#include "cores/vdmplayer/IVD_coreplayer_decls.h"
#include "utils/StringUtils.h"

#if defined(HAS_EGL)
#include <EGL/egl.h>
#endif

using namespace libvp;

class CVDMPlcoreConfig : public IVDPlayerConfig
{
public:
	CVDMPlcoreConfig();
	virtual ~CVDMPlcoreConfig();

	void SetAccountType(int nAccountType);
	void SetClientVersion(const char* strClientVersion);
	void SetUsername(const char* strUserName);
	void SetUserPassword(const char* strUserPassword);
	void SetUserEmail(const char* strUserEmail);

	void SetFontPath(const char* strFontPath);
	void SetPlcoreRuntimesPath(const char* strPlcoreHomePath);
	void SetLogPath(const char* strLogPath);
	void SetTempFolderPath(const char* strTempFolderPath);
	void SetFrameworksFolderPath(const char* strFrameworksFolderPath);
	void SetBDCachePath(const char* strBDCachePath);
	void SetBDJResourcePath(const char* strBDJResourcePath);
	void SetBDLibPath(const char* strBDLibPath);
	void SetFontName(const char* strFontName);

	virtual int GetAccountType();
	virtual const char* GetSpecifyName();
	virtual const char* GetClientAppName();
	virtual const char* GetClientPackageName();
	virtual const char* GetClientVersion();
	virtual const char* GetUsername();
	virtual const char* GetUserPassword();
	virtual const char* GetUserEmail();
	virtual const char* GetPath(VD_PathType type);
	virtual const char* GetFileName(VD_FileType type);
	virtual void* GetJavaVM();
	virtual void* GetJNIEnv();
	virtual void* GetObjDispManager();

private:
	int m_nAccountType;

	std::string m_strSpecifyName;
	std::string m_strClientAppName;
	std::string m_strClientPackageName;
	std::string m_strClientVersion;
	std::string m_strUserName;
	std::string m_strUserPassword;
	std::string m_strUserEmail;

	std::string m_strFontPath;
	std::string m_strPlcoreRuntimesPath;
	std::string m_strLogPath;
	std::string m_strTempFolderPath;
	std::string m_strFrameworksFolderPath;
	std::string m_strBDCachePath;
	std::string m_strBDJResourcePath;
	std::string m_strBDLibPath;

	std::string m_strFontName;
};

class CVDMPlayToolConfig : public IVDPlayToolConfig
{
public:
	CVDMPlayToolConfig();
	virtual ~CVDMPlayToolConfig();

	void SetRenderVendor(const char* strRenderVendor);
	void SetRenderRenderer(const char* strRenderRenderer);
	void SetRenderVersion(unsigned int major, unsigned int minor);
	void SetRenderCaps(unsigned int nRenderCaps);
	void SetMaxTextureSize(unsigned int nMaxTextureSize);
	void SetScreen(int nScreen);

	virtual const char* GetRenderVendor() const;
	virtual const char* GetRenderRenderer() const;
	virtual void GetRenderVersion(unsigned int& major, unsigned int& minor) const;
	virtual unsigned int GetRenderCaps() const;
	virtual unsigned int GetMaxTextureSize() const;
	virtual int GetScreen() const;

#if defined(HAS_DX)
	void SetD3d9(const LPDIRECT3D9 pD3D);
	void SetD3d9Device(const LPDIRECT3DDEVICE9 pD3DDevice);
	void SetAIdentifier(const D3DADAPTER_IDENTIFIER9& AIdentifier);
	void SetDefaultD3DUsage(DWORD nDefaultD3DUsage);
	void SetDefaultD3DPool(const D3DPOOL& defaultD3DPool);
	void SetD3DPP(const D3DPRESENT_PARAMETERS& d3dpp);

	virtual LPDIRECT3D9 GetD3d9() const;
	virtual LPDIRECT3DDEVICE9 GetD3d9Device() const;
	virtual const D3DADAPTER_IDENTIFIER9& GetAIdentifier() const;
	virtual DWORD GetDefaultD3DUsage() const;
	virtual D3DPOOL GetDefaultD3DPool() const;
	virtual D3DPRESENT_PARAMETERS GetD3DPP() const;
#endif

#if defined(HAS_GL)
	void SetRenderQuirks(unsigned int nRenderQuirks);
	virtual unsigned int GetRenderQuirks() const;
#endif

#if defined(TARGET_DARWIN_OSX)
	void GetGlContext(void* pGlContext);
	virtual void* GetGlContext() const;
#endif

#if defined(HAS_GL) || defined(HAS_GLES)
	void SetRenderExtensions(const char* strRenderExtensions);
	virtual const char* GetRenderExtensions() const;
#endif

#if defined(HAS_EGL)
	void SetEGLDisplay(const EGLDisplay& EGLDisplay);
	void SetEGLContext(const EGLContext& EGLContext);

	virtual EGLDisplay GetEGLDisplay();
	virtual EGLContext GetEGLContext();
#endif
	virtual int GetRenderWindowInitWidth() const;
	virtual int GetRenderWindowInitHeight() const;

private:
	std::string m_strRenderVendor;
	std::string m_strRenderRenderer;
	unsigned int m_nRenderVersionMajor;
	unsigned int m_nRenderVersionMinor;
	unsigned int m_nRenderCaps;
	unsigned int m_nMaxTextureSize;
	int m_nScreen;

#if defined(HAS_DX)
	LPDIRECT3D9 m_pD3D;
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	D3DADAPTER_IDENTIFIER9 m_AIdentifier;
	DWORD m_nDefaultD3DUsage;
	D3DPOOL m_defaultD3DPool;
	D3DPRESENT_PARAMETERS m_d3dpp;
#endif

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

class CVDMPlayCallback : public IVDPlayCallback
{
public:
	CVDMPlayCallback();
	virtual ~CVDMPlayCallback();

	virtual void OnPlayBackEnded();
	virtual void OnPlayBackStarted();
	virtual void OnPlayBackPaused();
	virtual void OnPlayBackResumed();
	virtual void OnPlayBackStopped();
	virtual void OnQueueNextItem();
	virtual void OnPlayBackSeek(int iTime, int seekOffset);
	virtual void OnPlayBackSeekChapter(int iChapter);
	virtual void OnPlayBackSpeedChanged(int iSpeed);

	virtual void OnPlayBackSeekEnd();
	virtual void OnPlayBackHwDecodeFailed();
	virtual void OnPlayBackDecodeModeNotify(bool Hw);
	virtual void OnOpenAC3();
};

class CVDMPlaytoolCallback : public IVDPlaytoolCallback
{
public:
	CVDMPlaytoolCallback();
	virtual ~CVDMPlaytoolCallback();

	virtual void OnThreadMessageCallback(VD_ThreadMessageCallback& cb, bool bWaitBeforeDone);
	virtual void OnRenderFlush();
	virtual void OnStackSeek(double time);
	virtual double OnGetStackTime();
	virtual void OnActionTriggerOsd();
	virtual void OnSwitchToFullscreen();
	virtual void OnSetViewPort(float x1, float y1, float x2, float y2);
	virtual void OnSetVideoResolution(VD_Resolution res, bool forceUpdate);
	virtual void OnSetFullScreenVideo(bool bFull);

#if defined(HAS_GLES)
	virtual void OnEnableGUIShader(VD_ShaderMethod method);
	virtual void OnDisableGUIShader();
	virtual int OnGUIShaderGetPos();
	virtual int OnGUIShaderGetCol();
	virtual int OnGUIShaderGetCoord0();
	virtual int OnGUIShaderGetUniCol();
	virtual int OnGUIShaderGetCoord1();
	virtual int OnGUIShaderGetCoord0Matrix();
#endif
};

class CVDMPlcoreCallback : public IVDPlcoreCallback
{
public:
  CVDMPlcoreCallback();
  virtual ~CVDMPlcoreCallback();

  virtual void OnError(VD_ErrorCallbackType type, const char* strLastErrorDescription);
  virtual void NotifyDBSCardStatus(VD_DBS_CasSmasrCardStatus event, void *pEventBody);
};

class CVDMPlayOptions : public IVDPlayOptions
{
public:
	CVDMPlayOptions();
	virtual ~CVDMPlayOptions();

	void SetStartTime(double dStartTime);
	void SetStartPercent(double dStartPercent);
	void SetState(const char* strState);
	void SetFullscreen(bool bIsFullscreen);
	void SetVideoOnly(bool bIsVideoOnly);
	void SetPlaylist(int nPlaylist);
	void SetPreSelectedAudioIdx(int nPreSelectedAudioIdx);
	void SetPreSelectedSubtitleIdx(int nPreSelectedSubtitleIdx);

	virtual double GetStartTime();
	virtual double GetStartPercent();
	virtual const char* GetState();
	virtual bool IsFullscreen();
	virtual bool IsVideoOnly();
	virtual int GetPlaylist();
	virtual int GetPreSelectedAudioIdx();
	virtual int GetPreSelectedSubtitleIdx();
	virtual bool IsFromUpperKodi(){ return true; }

private:
	double m_dStartTime;
	double m_dStartPercent;
	std::string m_strState;
	bool m_bIsFullscreen;
	bool m_bIsVideoOnly;
	int m_nPlaylist;
	int m_nPreSelectedAudioIdx;
	int m_nPreSelectedSubtitleIdx;
};

class CVDMFileItem : public IVDFileItem
{
public:
	CVDMFileItem();
	virtual ~CVDMFileItem();

	void SetPath(const char* strPath);
	void SetMimeType(const char* strMimeType);

	virtual const char* GetPath();
	virtual const char* GetMimeType();

private:
	std::string m_strPath;
	std::string m_strMimeType;
};

#endif
