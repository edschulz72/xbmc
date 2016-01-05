
#ifdef HAS_VIDONME

#if defined(HAS_EGL)
#include <EGL/egl.h>
#endif

#include "VDMPlayerConfig.h"
#include "Application.h"
#include "ApplicationMessenger.h"
#include "guilib/GraphicContext.h"
#include "windowing/WindowingFactory.h"

#include "cores/IPlayer.h"
#include "cores/vdmplayer/VidonCommon.h"
#include "cores/vdmplayer/IVD_player.h"
#include "cores/vdmplayer/IVD_playtool.h"
#include "cores/vdmplayer/IVD_coreplayer.h"
#include "cores/vdmplayer/VDMPlayerConfig.h"

#if defined(TARGET_ANDROID)
#include "android/activity/XBMCApp.h"
#endif

#if defined(TARGET_ANDROID)
#include "android/VidonPlayer_jni.h"
#include "android/jni/JNIThreading.h"
#endif

CVDMPlcoreConfig::CVDMPlcoreConfig()
{
	int m_nAccountType = ACCOUNT_TYPE_CHINA;

	m_strSpecifyName = "VDPlayer";
	m_strClientAppName = "XBMC";
	m_strClientPackageName = "org.vidonme.xbmc";

	m_strClientVersion.clear();
	m_strUserName.clear();
	m_strUserPassword.clear();
	m_strUserEmail.clear();

	m_strFontPath.clear();
	m_strLogPath.clear();
	m_strPlcoreRuntimesPath.clear();
	m_strTempFolderPath.clear();
	m_strProfileFolderPath.clear();
	m_strFrameworksFolderPath.clear();
	m_strHomePath.clear();
	m_strBDCachePath.clear();
	m_strBDJResourcePath.clear();
	m_strBDLibPath.clear();
	m_strFontName.clear();
}

CVDMPlcoreConfig::~CVDMPlcoreConfig()
{
}

void CVDMPlcoreConfig::SetAccountType(int nAccountType)
{
	m_nAccountType = nAccountType;
}

void CVDMPlcoreConfig::SetClientVersion(const char* strClientVersion)
{
	m_strClientVersion = strClientVersion;
}

void CVDMPlcoreConfig::SetUsername(const char* strUserName)
{
	m_strUserName = strUserName;
}

void CVDMPlcoreConfig::SetUserPassword(const char* strUserPassword)
{
	m_strUserPassword = strUserPassword;
}

void CVDMPlcoreConfig::SetUserEmail(const char* strUserEmail)
{
	m_strUserEmail = strUserEmail;
}

void CVDMPlcoreConfig::SetFontPath(const char* strFontPath)
{
	m_strFontPath = strFontPath;
}

void CVDMPlcoreConfig::SetPlcoreRuntimesPath(const char* strPlcoreHomePath)
{
	m_strPlcoreRuntimesPath = strPlcoreHomePath;
}

void CVDMPlcoreConfig::SetLogPath(const char* strLogPath)
{
	m_strLogPath = strLogPath;
}

void CVDMPlcoreConfig::SetTempFolderPath(const char* strTempFolderPath)
{
	m_strTempFolderPath = strTempFolderPath;
}

void CVDMPlcoreConfig::SetProfileFolderPath(const char* strProfileFolderPath)
{
	m_strProfileFolderPath = strProfileFolderPath;
}

void CVDMPlcoreConfig::SetFrameworksFolderPath(const char* strFrameworksFolderPath)
{
	m_strFrameworksFolderPath = strFrameworksFolderPath;
}

void CVDMPlcoreConfig::SetHomePath(const char* strHomePath)
{
	m_strHomePath = strHomePath;
}

void CVDMPlcoreConfig::SetBDCachePath(const char* strBDCachePath)
{
	m_strBDCachePath = strBDCachePath;
}

void CVDMPlcoreConfig::SetBDJResourcePath(const char* strBDJResourcePath)
{
	m_strBDJResourcePath = strBDJResourcePath;
}

void CVDMPlcoreConfig::SetBDLibPath(const char* strBDLibPath)
{
	m_strBDLibPath = strBDLibPath;
}

void CVDMPlcoreConfig::SetFontName(const char* strFontName)
{
	m_strFontName = strFontName;
}

int CVDMPlcoreConfig::GetAccountType()
{
	return m_nAccountType;
}

const char* CVDMPlcoreConfig::GetSpecifyName()
{
	return m_strSpecifyName.c_str();
}

const char* CVDMPlcoreConfig::GetClientAppName()
{
	return m_strClientAppName.c_str();
}

const char* CVDMPlcoreConfig::GetClientPackageName()
{
	return m_strClientPackageName.c_str();
}

const char* CVDMPlcoreConfig::GetClientVersion()
{
	return m_strClientVersion.c_str();
}

const char* CVDMPlcoreConfig::GetUsername()
{
	return m_strUserName.c_str();
}

const char* CVDMPlcoreConfig::GetUserPassword()
{
	return m_strUserPassword.c_str();
}

const char* CVDMPlcoreConfig::GetUserEmail()
{
	return m_strUserEmail.c_str();
}

const char* CVDMPlcoreConfig::GetPath(VD_PathType type)
{
	switch (type)
	{
	case VD_PATH_TYPE_FONT_FILE:
	{
		return m_strFontPath.c_str();
	}
	case VD_PATH_TYPE_LOG:
	{
		return m_strLogPath.c_str();
	}
	case VD_PATH_TYPE_RUNTIMES:
	{
		return m_strPlcoreRuntimesPath.c_str();
	}
	case VD_PATH_TYPE_TEMPFOLDER:
	{
		return m_strTempFolderPath.c_str();
	}
	case VD_PATH_TYPE_FRAMEWORKS_FOLDER:
	{
		return m_strFrameworksFolderPath.c_str();
	}
	case VD_PATH_TYPE_BD_CACHE:
	{
		return m_strBDCachePath.c_str();
	}
	case VD_PATH_TYPE_BDJ_RESOURCE:
	{
		return m_strBDJResourcePath.c_str();
	}
	case VD_PATH_TYPE_BD_LIB:
	{
		return m_strBDLibPath.c_str();
	}
	default:
		return "";
	}
}

const char* CVDMPlcoreConfig::GetFileName(VD_FileType type)
{
	switch (type)
	{
	case VD_FILE_TYPE_FONT_FILE:
	{
		return m_strFontName.c_str();
	}
	default:
	{
		return "";
	}
	}
}

void* CVDMPlcoreConfig::GetJavaVM()
{
#if defined(TARGET_ANDROID)
	return xbmc_jvm();
#else
	return 0;
#endif
}

void* CVDMPlcoreConfig::GetJNIEnv()
{
#if defined(TARGET_ANDROID)
	return xbmc_jnienv();
#else
	return 0;
#endif
}

void* CVDMPlcoreConfig::GetObjDispManager()
{
#if defined(TARGET_ANDROID)
#if 1
    JNIEnv *env = NULL;
    ANativeActivity* nativeActivity = CXBMCApp::GetCurrentActivity();
    nativeActivity->vm->AttachCurrentThread (&env, NULL);
    jobject oActivity = nativeActivity->clazz;
    jclass cActivity = env->GetObjectClass(oActivity);

    jmethodID midActivityGetSystemService = env->GetMethodID(cActivity, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jstring sDisplayManagerService = env->NewStringUTF("display"); // Display_Manager
    jobject oDisplayManager = env->CallObjectMethod(oActivity, midActivityGetSystemService, sDisplayManagerService);
    AndroidRuntime::Get().m_objDispManager = env->NewGlobalRef(oDisplayManager);
    env->DeleteLocalRef(oDisplayManager);
#endif
	return AndroidRuntime::Get().m_objDispManager;
#else
	return 0;
#endif
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CVDMPlayToolConfig::CVDMPlayToolConfig()
{
}

CVDMPlayToolConfig::~CVDMPlayToolConfig()
{
}

void CVDMPlayToolConfig::SetRenderVendor(const char* strRenderVendor)
{
	m_strRenderVendor = strRenderVendor;
}

void CVDMPlayToolConfig::SetRenderRenderer(const char* strRenderRenderer)
{
	m_strRenderRenderer = strRenderRenderer;
}

void CVDMPlayToolConfig::SetRenderVersion(unsigned int major, unsigned int minor)
{
	m_nRenderVersionMajor = major;
	m_nRenderVersionMinor = major;
}

void CVDMPlayToolConfig::SetRenderCaps(unsigned int nRenderCaps)
{
	m_nRenderCaps = nRenderCaps;
}

void CVDMPlayToolConfig::SetMaxTextureSize(unsigned int nMaxTextureSize)
{
	m_nMaxTextureSize = nMaxTextureSize;
}

void CVDMPlayToolConfig::SetScreen(int nScreen)
{
	m_nScreen = nScreen;
}

const char* CVDMPlayToolConfig::GetRenderVendor() const
{
	return m_strRenderVendor.c_str();
}

const char* CVDMPlayToolConfig::GetRenderRenderer() const
{
	return m_strRenderRenderer.c_str();
}

void CVDMPlayToolConfig::GetRenderVersion(unsigned int& major, unsigned int& minor) const
{
	major = m_nRenderVersionMajor;
	minor = m_nRenderVersionMinor;
}

unsigned int CVDMPlayToolConfig::GetRenderCaps() const
{
	return m_nRenderCaps;
}

unsigned int CVDMPlayToolConfig::GetMaxTextureSize() const
{
	return m_nMaxTextureSize;
}

int CVDMPlayToolConfig::GetScreen() const
{
	return m_nScreen;
}

#if defined(HAS_DX)

void CVDMPlayToolConfig::SetD3d9(const LPDIRECT3D9 pD3D)
{
	m_pD3D = pD3D;
}

void CVDMPlayToolConfig::SetD3d9Device(const LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
}

void CVDMPlayToolConfig::SetAIdentifier(const D3DADAPTER_IDENTIFIER9& AIdentifier)
{
	m_AIdentifier = AIdentifier;
}

void CVDMPlayToolConfig::SetDefaultD3DUsage(DWORD nDefaultD3DUsage)
{
	m_nDefaultD3DUsage = nDefaultD3DUsage;
}

void CVDMPlayToolConfig::SetDefaultD3DPool(const D3DPOOL& defaultD3DPool)
{
	m_defaultD3DPool = defaultD3DPool;
}

void CVDMPlayToolConfig::SetD3DPP(const D3DPRESENT_PARAMETERS& d3dpp)
{
	m_d3dpp = d3dpp;
}

LPDIRECT3D9 CVDMPlayToolConfig::GetD3d9() const
{
	return m_pD3D;
}

LPDIRECT3DDEVICE9 CVDMPlayToolConfig::GetD3d9Device() const
{
	return m_pD3DDevice;
}

const D3DADAPTER_IDENTIFIER9& CVDMPlayToolConfig::GetAIdentifier() const
{
	return m_AIdentifier;
}

DWORD CVDMPlayToolConfig::GetDefaultD3DUsage() const
{
	return m_nDefaultD3DUsage;
}

D3DPOOL CVDMPlayToolConfig::GetDefaultD3DPool() const
{
	return m_defaultD3DPool;
}

D3DPRESENT_PARAMETERS CVDMPlayToolConfig::GetD3DPP() const
{
	return m_d3dpp;
}

#endif

#if defined(HAS_GL)

void CVDMPlayToolConfig::SetRenderQuirks(unsigned int nRenderQuirks)
{
	m_nRenderQuirks = nRenderQuirks;
}

unsigned int CVDMPlayToolConfig::GetRenderQuirks() const
{
	return m_nRenderQuirks;
}
#endif

#if defined(TARGET_DARWIN_OSX)
void CVDMPlayToolConfig::GetGlContext(void* pGlContext)
{
	m_pGlContext = pGlContext;
}

void* CVDMPlayToolConfig::GetGlContext() const
{
	return m_pGlContext;
}
#endif

#if defined(HAS_GL) || defined(HAS_GLES)
void CVDMPlayToolConfig::SetRenderExtensions(const char* strRenderExtensions)
{
	m_strRenderExtensions = strRenderExtensions;
}

const char* CVDMPlayToolConfig::GetRenderExtensions() const
{
	return m_strRenderExtensions.c_str();
}
#endif

#if defined(HAS_EGL)
void CVDMPlayToolConfig::SetEGLDisplay(const EGLDisplay& EGLDisplay)
{
	m_EGLDisplay = EGLDisplay;
}

void CVDMPlayToolConfig::SetEGLContext(const EGLContext& EGLContext)
{
	m_EGLContext = EGLContext;
}

EGLDisplay CVDMPlayToolConfig::GetEGLDisplay()
{
	return m_EGLDisplay;
}

EGLContext CVDMPlayToolConfig::GetEGLContext()
{
	return m_EGLContext;
}
#endif

int CVDMPlayToolConfig::GetRenderWindowInitWidth() const
{
	int nWidth = g_graphicsContext.GetWidth();
	return nWidth;
}

int CVDMPlayToolConfig::GetRenderWindowInitHeight() const
{
	int nHeight = g_graphicsContext.GetHeight();
	return nHeight;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CVDMPlayCallback::CVDMPlayCallback()
{
}

CVDMPlayCallback::~CVDMPlayCallback()
{
}

void CVDMPlayCallback::OnPlayBackEnded()
{
	g_application.OnPlayBackEnded();
}

void CVDMPlayCallback::OnPlayBackStarted()
{
	g_application.OnPlayBackStarted();
}

void CVDMPlayCallback::OnPlayBackPaused()
{
	g_application.OnPlayBackPaused();
}

void CVDMPlayCallback::OnPlayBackResumed()
{
	g_application.OnPlayBackResumed();
}

void CVDMPlayCallback::OnPlayBackStopped()
{
	g_application.OnPlayBackStopped();
}

void CVDMPlayCallback::OnQueueNextItem()
{
	g_application.OnQueueNextItem();
}

void CVDMPlayCallback::OnPlayBackSeek(int iTime, int seekOffset)
{
	g_application.OnPlayBackSeek(iTime, seekOffset);
}

void CVDMPlayCallback::OnPlayBackSeekChapter(int iChapter)
{
	g_application.OnPlayBackSeekChapter(iChapter);
}

void CVDMPlayCallback::OnPlayBackSpeedChanged(int iSpeed)
{
	g_application.OnPlayBackSpeedChanged(iSpeed);
}

void CVDMPlayCallback::OnPlayBackSeekEnd()
{
}

void CVDMPlayCallback::OnPlayBackHwDecodeFailed()
{
}

void CVDMPlayCallback::OnPlayBackDecodeModeNotify(bool Hw)
{
}

void CVDMPlayCallback::OnOpenAC3()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CVDMPlaytoolCallback::CVDMPlaytoolCallback()
{
}

CVDMPlaytoolCallback::~CVDMPlaytoolCallback()
{
}

void CVDMPlaytoolCallback::OnThreadMessageCallback(VD_ThreadMessageCallback& cb, bool bWaitBeforeDone)
{
	ThreadMessageCallback callbackData;
	callbackData.callback = cb.callback;
	callbackData.userptr = cb.userptr;

	ThreadMessage msg;
	msg.dwMessage = TMSG_CALLBACK;
	msg.lpVoid = (void*)&callbackData;

	CApplicationMessenger::Get().SendMessage(msg, bWaitBeforeDone);
}

void CVDMPlaytoolCallback::OnRenderFlush()
{
	ThreadMessage msg = { TMSG_RENDERER_FLUSH };
	CApplicationMessenger::Get().SendMessage(msg, false);
}

void CVDMPlaytoolCallback::OnStackSeek(double time)
{
	g_application.SeekTime(time);
}

double CVDMPlaytoolCallback::OnGetStackTime()
{
	return g_application.GetTime();
}

void CVDMPlaytoolCallback::OnActionTriggerOsd()
{
	CApplicationMessenger::Get().SendAction(CAction(ACTION_TRIGGER_OSD), WINDOW_INVALID, false);
}

void CVDMPlaytoolCallback::OnSwitchToFullscreen()
{
	CApplicationMessenger::Get().SwitchToFullscreen();
}

void CVDMPlaytoolCallback::OnSetViewPort(float x1, float y1, float x2, float y2)
{
	//CRect rect(x1, y1, x2, y2);
	//g_Windowing.SetViewPort(rect);
}

void CVDMPlaytoolCallback::OnSetVideoResolution(VD_Resolution res, bool forceUpdate)
{
	ThreadMessage msg = { TMSG_SETVIDEORESOLUTION };
	msg.param1 = RESOLUTION(res);
	msg.param2 = forceUpdate ? 1 : 0;
	CApplicationMessenger::Get().SendMessage(msg, false);
}

void CVDMPlaytoolCallback::OnSetFullScreenVideo(bool bFull)
{
	ThreadMessage msg = { TMSG_SETFULLSCREEN };
	msg.param1 = bFull ? 1 : 0;
	CApplicationMessenger::Get().SendMessage(msg, false);
}

#if defined(HAS_GLES)
void CVDMPlaytoolCallback::OnEnableGUIShader(VD_ShaderMethod method)
{
	ESHADERMETHOD es_method;
	switch (method)
	{
	case VD_SHADER_METHOD_DEFAULT:
	{
		es_method = SM_DEFAULT;
	}
	case VD_SHADER_METHOD_TEXTURE:
	{
		es_method = SM_TEXTURE;
	}
	case VD_SHADER_METHOD_MULTI:
	{
		es_method = SM_MULTI;
	}
	case VD_SHADER_METHOD_FONTS:
	{
		es_method = SM_FONTS;
	}
	case VD_SHADER_METHOD_TEXTURE_NOBLEND:
	{
		es_method = SM_TEXTURE_NOBLEND;
	}
	case VD_SHADER_METHOD_MULTI_BLENDCOLOR:
	{
		es_method = SM_MULTI_BLENDCOLOR;
	}
	case VD_SHADER_METHOD_TEXTURE_RGBA:
	{
		es_method = SM_TEXTURE_RGBA;
	}
	case VD_SHADER_METHOD_TEXTURE_RGBA_OES:
	{
		es_method = SM_TEXTURE_RGBA_OES;
	}
	case VD_SHADER_METHOD_TEXTURE_RGBA_BLENDCOLOR:
	{
		es_method = SM_TEXTURE_RGBA_BLENDCOLOR;
	}/*
	case VD_SHADER_METHOD_TEXTURE_RGBA_BOB:
	{
	es_method = SM_TEXTURE_RGBA_BOB;
	}
	case VD_SHADER_METHOD_TEXTURE_RGBA_BOB_OES:
	{
	es_method = SM_TEXTURE_RGBA_BOB_OES;
	}*/
	case VD_SHADER_METHOD_ESHADERCOUNT:
	{
		es_method = SM_ESHADERCOUNT;
	}
	default:
		es_method = SM_DEFAULT;
	}

	g_Windowing.EnableGUIShader(es_method);
}

void CVDMPlaytoolCallback::OnDisableGUIShader()
{
	g_Windowing.DisableGUIShader();
}

int CVDMPlaytoolCallback::OnGUIShaderGetPos()
{
	return g_Windowing.GUIShaderGetPos();
}

int CVDMPlaytoolCallback::OnGUIShaderGetCol()
{
	return g_Windowing.GUIShaderGetCol();
}

int CVDMPlaytoolCallback::OnGUIShaderGetCoord0()
{
	return g_Windowing.GUIShaderGetCoord0();
}

int CVDMPlaytoolCallback::OnGUIShaderGetUniCol()
{
	return g_Windowing.GUIShaderGetUniCol();
}

int CVDMPlaytoolCallback::OnGUIShaderGetCoord1()
{
	return g_Windowing.GUIShaderGetCoord1();
}

int CVDMPlaytoolCallback::OnGUIShaderGetCoord0Matrix()
{
	return g_Windowing.GUIShaderGetCoord0Matrix();
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CVDMPlcoreCallback::CVDMPlcoreCallback()
{
}

CVDMPlcoreCallback::~CVDMPlcoreCallback()
{
}

void CVDMPlcoreCallback::OnError(VD_ErrorCallbackType type, const char* strLastErrorDescription)
{

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CVDMPlayOptions::CVDMPlayOptions()
{
	m_strState.clear();

	m_dStartTime = 0;
	m_dStartPercent = 0;
	m_bIsFullscreen = 0;
	m_bIsVideoOnly = 0;
	m_nPlaylist = -1;
	m_nPreSelectedAudioIdx = 0;
	m_nPreSelectedSubtitleIdx = 0;
}

CVDMPlayOptions::~CVDMPlayOptions()
{
}

void CVDMPlayOptions::SetStartTime(double dStartTime)
{
	m_dStartTime = dStartTime;
}

void CVDMPlayOptions::SetStartPercent(double dStartPercent)
{
	m_dStartPercent = dStartPercent;
}

void CVDMPlayOptions::SetState(const char* strState)
{
	m_strState = strState;
}

void CVDMPlayOptions::SetFullscreen(bool bIsFullscreen)
{
	m_bIsFullscreen = bIsFullscreen;
}

void CVDMPlayOptions::SetVideoOnly(bool bIsVideoOnly)
{
	m_bIsVideoOnly = bIsVideoOnly;
}

void CVDMPlayOptions::SetPlaylist(int nPlaylist)
{
	m_nPlaylist = nPlaylist;
}

void CVDMPlayOptions::SetPreSelectedAudioIdx(int nPreSelectedAudioIdx)
{
	m_nPreSelectedAudioIdx = nPreSelectedAudioIdx;
}

void CVDMPlayOptions::SetPreSelectedSubtitleIdx(int nPreSelectedSubtitleIdx)
{
	m_nPreSelectedSubtitleIdx = nPreSelectedSubtitleIdx;
}

double CVDMPlayOptions::GetStartTime()
{
	return m_dStartTime;
}

double CVDMPlayOptions::GetStartPercent()
{
	return m_dStartPercent;
}

const char* CVDMPlayOptions::GetState()
{
	return m_strState.c_str();
}

bool CVDMPlayOptions::IsFullscreen()
{
	return m_bIsFullscreen;
}

bool CVDMPlayOptions::IsVideoOnly()
{
	return m_bIsVideoOnly;
}

int CVDMPlayOptions::GetPlaylist()
{
	return m_nPlaylist;
}

int CVDMPlayOptions::GetPreSelectedAudioIdx()
{
	return m_nPreSelectedAudioIdx;
}

int CVDMPlayOptions::GetPreSelectedSubtitleIdx()
{
	return m_nPreSelectedSubtitleIdx;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


CVDMFileItem::CVDMFileItem()
{
	m_strPath.clear();
	m_strMimeType.clear();
}

CVDMFileItem::~CVDMFileItem()
{
}

void CVDMFileItem::SetPath(const char* strPath)
{
	m_strPath = strPath;
}

void CVDMFileItem::SetMimeType(const char* strMimeType)
{
	m_strMimeType = strMimeType;
}

const char* CVDMFileItem::GetPath()
{
	return m_strPath.c_str();
}

const char* CVDMFileItem::GetMimeType()
{
	return m_strMimeType.c_str();
}


#endif
