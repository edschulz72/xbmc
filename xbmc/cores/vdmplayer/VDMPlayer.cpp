
#ifdef HAS_VIDONME

#if defined(TARGET_ANDROID)
#include "android/activity/XBMCApp.h"
#endif

#include "VDMPlayer.h"
#include "FileItem.h"
#include "VDMPlayerConfig.h"
#include "ApplicationMessenger.h"
#include "settings/Settings.h"
#include "settings/AdvancedSettings.h"
#include "settings/DisplaySettings.h"
#include "settings/MediaSettings.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/Resolution.h"
#include "guilib/GUIWindowManager.h"
#include "windowing/WindowingFactory.h"
#include "cores/VideoRenderers/RenderManager.h"
#include "cores/DataCacheCore.h"
#include "input/Key.h"
#include "dialogs/GUIDialogBusy.h"
#include "utils/log.h"
#include "utils/URIUtils.h"
#include "URL.h"
#include "PasswordManager.h"
#include "LangInfo.h"

IVDPlayer* m_pPlcore;
CVDMPlcoreConfig* m_pConfig;
CVDMPlcoreCallback* s_pPlcoreCallback = NULL;

#if defined(TARGET_ANDROID)
#include "android/jni/JNIThreading.h"
#include "android/VidonPlayer_jni.h"
#endif

CVDMPlayer::CVDMPlayer(IPlayerCallback& callback)
	: IPlayer(callback),
	CThread("CVDMPlayer")
{
	m_bIsPlaying = false;
	m_bStreamInfoChanged = false;

	m_pPlayTool = 0;
	m_pCorePlayer = 0;
	m_pConfig = 0;
	m_pPlcorePlayerCallback = 0;
	m_pPlayToolConfig = 0;
	m_pPlcorePlaytoolCallback = 0;
	s_pPlcoreCallback = 0;
	m_nAudioStream = -1;
}

CVDMPlayer::~CVDMPlayer()
{
	if (m_bIsPlaying)
	{
		CloseFile();
	}
}

bool CVDMPlayer::Initialize(TiXmlElement* pConfig)
{
	return true;
}

bool CVDMPlayer::OpenFile(const CFileItem& file, const CPlayerOptions &options)
{
	//now xbmc's audiotrack play ui sound , plcore's audioengine player other sound.
        //In order to avoid conflict of audio device. close xbmc's audiotrack when play file.
        //change audiodevice to close xbmc's audiotrack and open a xbmc's NULL device(for volume control) 
        //Reference CloseFile by xiaolei.zhang

#if defined(TARGET_ANDROID) 
	CSettings::Get().SetString("audiooutput.audiodevice", "NULL:NULL");
#endif

	if (IsRunning())
	{
		StopThread(false);
	}

	if (m_bIsPlaying)
	{
		CloseFile();
		DeInitPlayer();
	}

	std::string strPath = file.GetPath();

	if (URIUtils::IsSmb(strPath) ||
		StringUtils::EqualsNoCase(StringUtils::Left(strPath, 9).c_str(), "bluray://"))
	{
		CURL url(strPath);
		if (url.GetUserName().empty() || url.GetPassWord().empty())
		{
			CPasswordManager::GetInstance().AuthenticateURL(url);
			strPath = url.Get();
		}
	}

	m_itemPlay.SetPath(strPath.c_str());
	m_itemPlay.SetMimeType(file.GetMimeType().c_str());

	m_optionsPlay.SetStartTime(options.starttime);
	m_optionsPlay.SetStartPercent(options.startpercent);
	m_optionsPlay.SetState(options.state.c_str());
	m_optionsPlay.SetFullscreen(options.fullscreen);
	m_optionsPlay.SetVideoOnly(options.video_only);

	m_pPlayToolConfig = new CVDMPlayToolConfig();
	m_pPlcorePlayerCallback = new CVDMPlayCallback();
	m_pPlcorePlaytoolCallback = new CVDMPlaytoolCallback();

	if (!m_pPlayToolConfig || !m_pPlcorePlayerCallback || !m_pPlcorePlaytoolCallback)
	{
		DeInitPlayer();

		return false;
	}

	m_pPlayToolConfig->SetRenderVendor(g_Windowing.GetRenderVendor().c_str());
	m_pPlayToolConfig->SetRenderRenderer(g_Windowing.GetRenderRenderer().c_str());

	unsigned int major, minor;
	g_Windowing.GetRenderVersion(major, minor);
	m_pPlayToolConfig->SetRenderVersion(major, minor);
	m_pPlayToolConfig->SetRenderCaps(g_Windowing.GetRenderCaps());
	m_pPlayToolConfig->SetMaxTextureSize(g_Windowing.GetMaxTextureSize());
	m_pPlayToolConfig->SetScreen(g_Windowing.GetCurrentScreen());


#if defined(HAS_DX)
	m_pPlayToolConfig->SetD3d9(g_Windowing.GetD3D());
	m_pPlayToolConfig->SetD3d9Device(g_Windowing.Get3DDevice());
	m_pPlayToolConfig->SetAIdentifier(g_Windowing.GetAIdentifier());
	m_pPlayToolConfig->SetDefaultD3DUsage(g_Windowing.DefaultD3DUsage());
	m_pPlayToolConfig->SetDefaultD3DPool(g_Windowing.DefaultD3DPool());
	m_pPlayToolConfig->SetD3DPP(g_Windowing.GetD3DPP());
#endif

#if defined(HAS_GL) || defined(HAS_GLES)
	//m_pPlayToolConfig->SetRenderExtensions(g_Windowing.GetRenderExtensions());
	m_pPlayToolConfig->SetRenderExtensions(" ");
#endif

#if defined(HAS_EGL)
	m_pPlayToolConfig->SetEGLDisplay(g_Windowing.GetEGLDisplay());
	m_pPlayToolConfig->SetEGLContext(g_Windowing.GetEGLContext());
#endif

#if defined(TARGET_ANDROID)
	if (1)
	{
		AndroidRuntime::Get().m_pNativewindow = (ANativeWindow *)(*(CXBMCApp::GetNativeWindow(30000)));

		AndroidRuntime::Get().m_sConfig.handle = (void**)&(AndroidRuntime::Get().m_pNativewindow);
		AndroidRuntime::Get().m_sConfig.nWidth = AndroidRuntime::Get().m_pNativewindow ? ANativeWindow_getWidth(AndroidRuntime::Get().m_pNativewindow) : 0;
		AndroidRuntime::Get().m_sConfig.nHeight = AndroidRuntime::Get().m_pNativewindow ? ANativeWindow_getHeight(AndroidRuntime::Get().m_pNativewindow) : 0;

		//	vidonconfig.pNativeActivity = CXBMCApp::GetCurrentActivity();
		//	vidonconfig.pNativeWindows = (ANativeWindow *)(*(CXBMCApp::GetNativeWindow (30000)));
	}
#endif
	m_pPlayTool = m_pPlcore->CreatePlayTool(VD_PLAYER_PLCORE, m_pPlayToolConfig, m_pPlcorePlaytoolCallback);
	if (!m_pPlayTool)
	{
		DeInitPlayer();

		return false;
	}

#ifdef HAS_VIDEO_PLAYBACK
	g_renderManager.PreInit();
#endif

	if (!m_pPlayTool->Init())
	{
		DeInitPlayer();

		return false;
	}
	m_event.Reset();

	Create();

	CGUIDialogBusy* dialog = (CGUIDialogBusy*)g_windowManager.GetWindow(WINDOW_DIALOG_BUSY);
	if (dialog)
	{
		dialog->Show();
		while (!m_event.WaitMSec(1))
			g_windowManager.ProcessRenderLoop(true);
		dialog->Close();
	}

	if (!m_bIsPlaying)
	{
		return false;
	}

	if (!StringUtils::EqualsNoCase(CSettings::Get().GetString("locale.audiolanguage"), "original") &&
		CMediaSettings::Get().GetCurrentVideoSettings().m_AudioStream <= 0)
	{
		int nAudioStreamCount = GetAudioStreamCount();

		for (size_t i = 0; i < nAudioStreamCount; i++)
		{
			SPlayerAudioStreamInfo info;
			GetAudioStreamInfo(i, info);

			std::string audio_language = g_langInfo.GetAudioLanguage();

			if (StringUtils::EqualsNoCase(audio_language, info.language))
			{
				SetAudioStream(i);
				break;
			}
		}
	}
	else
	{
		SetAudioStream(CMediaSettings::Get().GetCurrentVideoSettings().m_AudioStream);
	}

	if (!StringUtils::EqualsNoCase(CSettings::Get().GetString("locale.subtitlelanguage"), "original") &&
		CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleStream <= 0)
	{
		int nSubtitleCount = GetSubtitleCount();

		for (size_t i = 0; i < nSubtitleCount; i++)
		{
			std::string subtitle_language = g_langInfo.GetSubtitleLanguage();

			SPlayerSubtitleStreamInfo info;
			GetSubtitleStreamInfo(i, info);

			if (StringUtils::EqualsNoCase(subtitle_language, info.language))
			{
				SetSubtitle(i);
				break;
			}
		}
	}
	else
	{
		SetSubtitle(CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleStream);
	}

	SetSubTitleDelay(CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleDelay);
	SetSubtitleVisible(CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleOn);
	SetSubttileFeatures();
	
	g_graphicsContext.SetFullScreenVideo(true);

	g_dataCacheCore.SignalVideoInfoChange();
	g_dataCacheCore.SignalAudioInfoChange();
	
	m_bStreamInfoChanged = false;

	if (abs(m_optionsPlay.GetStartTime() - m_pCorePlayer->GetTime()) > 10)
	{
		m_pCorePlayer->SeekTime(m_optionsPlay.GetStartTime());
	}

	return true;
}

bool CVDMPlayer::QueueNextFile(const CFileItem &file)
{
	return true;
}

void CVDMPlayer::OnNothingToQueueNotify()
{
}

bool CVDMPlayer::CloseFile(bool reopen)
{
	//now xbmc's audiotrack play ui sound , plcore's audioengine player other sound.
        //In order to avoid conflict of audio device. close xbmc's audiotrack when play file.
        //change audiodevice to close xbmc's NULL device and open  xbmc's auidotrack 
        //Reference  OpenFile by xiaolei.zhang

#if defined(TARGET_ANDROID) 
   CSettings::Get().SetString("audiooutput.audiodevice", "AUDIOTRACK:android,audiotrack");
#endif

	if (!m_bIsPlaying)
	{
		return true;
	}

	if (!m_pCorePlayer)
	{
		return false;
	}

	m_bIsPlaying = false;

	m_pCorePlayer->CloseFile();

	if (CSettings::Get().GetBool("videoplayer.quitstereomodeonstop") && CSettings::Get().GetInt("videoscreen.stereoscopicmode") != RENDER_STEREO_MODE_OFF)
	{
		SetStereoMode(RENDER_STEREO_MODE_OFF, RENDER_STEREO_VIEW_OFF);
		SetGraphicContextStereoMode(RENDER_STEREO_MODE_OFF);
	}

	DeInitPlayer();

	return true;
}

bool CVDMPlayer::IsPlaying() const
{
	if (!m_pCorePlayer || !m_bIsPlaying)
	{
		return false;
	}

	return m_bIsPlaying && m_pCorePlayer->IsPlaying();
}

bool CVDMPlayer::CanPause()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->CanPause();
}

void CVDMPlayer::Pause()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	m_pCorePlayer->Pause();
}

bool CVDMPlayer::IsPaused() const
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->IsPaused();
}

bool CVDMPlayer::HasVideo() const
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->HasVideo();
}

bool CVDMPlayer::HasAudio() const
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->HasAudio();
}

bool CVDMPlayer::IsPassthrough() const
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->IsPassthrough();
}

bool CVDMPlayer::CanSeek()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->CanSeek();
}

void CVDMPlayer::Seek(bool bPlus, bool bLargeStep, bool bChapterOverride)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	m_pCorePlayer->Seek(bPlus, bLargeStep, bChapterOverride);
}

bool CVDMPlayer::SeekScene(bool bPlus)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->SeekScene(bPlus);
}

void CVDMPlayer::SeekPercentage(float fPercent)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	return m_pCorePlayer->SeekPercentage(fPercent);
}

float CVDMPlayer::GetPercentage()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->GetPercentage();
}

float CVDMPlayer::GetCachePercentage()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->GetCachePercentage();
}

void CVDMPlayer::SetMute(bool bOnOff)
{
	if (!m_pPlayTool)
	{
		return;
	}

	return m_pPlayTool->SetMute(bOnOff);
}

void CVDMPlayer::SetVolume(float volume)
{
	if (!m_pPlayTool)
	{
		return;
	}

	return m_pPlayTool->SetVolume(volume);
}

bool CVDMPlayer::ControlsVolume()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return true;// m_pCorePlayer->ControlsVolume();
}

void CVDMPlayer::SetDynamicRangeCompression(long drc)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetVolumeAmplification(drc / 100);
}


void CVDMPlayer::GetAudioInfo(std::string& strAudioInfo)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	char* strInfo = m_pCorePlayer->GetAudioInfo();
	if (strInfo)
	{
		strAudioInfo = strInfo;
	}

	m_pCorePlayer->ReleaseAudioInfo(strInfo);
}

void CVDMPlayer::GetVideoInfo(std::string& strVideoInfo)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	char* strInfo = m_pCorePlayer->GetVideoInfo();

	if (strInfo)
	{
		strVideoInfo = strInfo;
	}

	m_pCorePlayer->ReleaseVideoInfo(strInfo);
}

void CVDMPlayer::GetGeneralInfo(std::string& strGeneralInfo)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	char* strInfo = m_pCorePlayer->GetGeneralInfo();
	if (strInfo)
	{
		strGeneralInfo = strInfo;
	}

	m_pCorePlayer->ReleaseGeneralInfo(strInfo);
}

bool CVDMPlayer::CanRecord()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->CanRecord();
}

bool CVDMPlayer::IsRecording()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->IsRecording();
}

bool CVDMPlayer::Record(bool bOnOff)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->Record(bOnOff);
}

void CVDMPlayer::SetAVDelay(float fValue)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetAudioDelay(fValue);

	g_dataCacheCore.SignalAudioInfoChange();
}

float CVDMPlayer::GetAVDelay()
{
	return GetAudioDelay();
}

void CVDMPlayer::SetSubTitleDelay(float fValue)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetSubTitleDelay(fValue);
}

float CVDMPlayer::GetSubTitleDelay()
{
	if (!m_pPlayTool)
	{
		return 0;
	}

	return m_pPlayTool->GetSubTitleDelay();
}

int CVDMPlayer::GetSubtitleCount()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->GetSubtitleCount();
}

int CVDMPlayer::GetSubtitle()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->GetSubtitle();
}

void CVDMPlayer::GetSubtitleStreamInfo(int index, SPlayerSubtitleStreamInfo &info)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	VD_PlayerSubtitleStreamInfo* pInfo = m_pCorePlayer->GetSubtitleStreamInfo(index);
	if (!pInfo)
	{
		return;
	}

	info.language = pInfo->language ? pInfo->language : "";
	info.name = pInfo->name ? pInfo->name : "";

	m_pCorePlayer->ReleaseSubtitleStreamInfo(pInfo);
}

void CVDMPlayer::SetSubtitle(int iStream)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	if (iStream < 0 || iStream >= m_pCorePlayer->GetSubtitleCount())
	{
		return;
	}

	m_pCorePlayer->SetSubtitle(iStream);

	SetSubttileFeatures();
}

bool CVDMPlayer::GetSubtitleVisible()
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->GetSubtitleOn();
}

void CVDMPlayer::SetSubtitleVisible(bool bVisible)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetSubtitleOn(bVisible);

	SetSubttileFeatures();
}

void CVDMPlayer::AddSubtitle(const std::string& strSubPath)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	int nSubtitleID = m_pCorePlayer->AddSubtitle(strSubPath.c_str());

	SetSubtitle(nSubtitleID);
	SetSubtitleVisible(true);
}

int CVDMPlayer::GetAudioStreamCount()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->GetAudioStreamCount();
}

int CVDMPlayer::GetAudioStream()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	int nAudioStream = m_pCorePlayer->GetAudioStream();

	if (m_nAudioStream != -1)
	{
		if (nAudioStream != m_nAudioStream)
		{
			nAudioStream = m_nAudioStream;
		}
		else
		{
			m_nAudioStream = -1;
		}
	}

	return nAudioStream;
}

void CVDMPlayer::SetAudioStream(int iStream)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	if (iStream < 0 || iStream >= m_pCorePlayer->GetAudioStreamCount())
	{
		return;
	}

	m_nAudioStream = iStream;
	m_pCorePlayer->SetAudioStream(iStream);

	g_dataCacheCore.SignalAudioInfoChange();
}

void CVDMPlayer::GetAudioStreamInfo(int index, SPlayerAudioStreamInfo &info)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	int nStream = index;
	if (nStream == CURRENT_STREAM)
	{
		nStream = GetAudioStream();
	}

	VD_PlayerAudioStreamInfo* pInfo = m_pCorePlayer->GetAudioStreamInfo(nStream);
	if (!pInfo)
	{
		return;
	}

	info.bitrate = pInfo->bitrate;
	info.channels = pInfo->channels;
	info.samplerate = pInfo->samplerate;
	info.bitspersample = pInfo->bitspersample;
	info.language = pInfo->language ? pInfo->language : "";
	info.name = pInfo->name ? pInfo->name : "";
	info.audioCodecName = pInfo->audioCodecName ? pInfo->audioCodecName : "";

	m_pCorePlayer->ReleaseAudioStreamInfo(pInfo);

	if (!m_bStreamInfoChanged)
	{
		int nTitleNum = 0;
		bool bMainTitle = false;
		m_pCorePlayer->GetTitleInfo(nTitleNum, bMainTitle);

		if (bMainTitle)
		{
			g_dataCacheCore.SignalVideoInfoChange();
			g_dataCacheCore.SignalAudioInfoChange();

			m_bStreamInfoChanged = true;
		}
	}
}

TextCacheStruct_t* CVDMPlayer::GetTeletextCache()
{
	return NULL;
}

void CVDMPlayer::LoadPage(int p, int sp, unsigned char* buffer)
{
}

int CVDMPlayer::GetChapterCount()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->GetChapterCount();
}

int CVDMPlayer::GetChapter()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->GetChapter();
}

void CVDMPlayer::GetChapterName(std::string& strChapterName, int chapterIdx)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	char* strName = m_pCorePlayer->GetChapterName();
	if (strName)
	{
		strChapterName = strName;
	}

	m_pCorePlayer->ReleaseChapterName(strName);
}

int64_t CVDMPlayer::GetChapterPos(int chapterIdx)
{
	return 0;
}

int CVDMPlayer::SeekChapter(int iChapter)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->SeekChapter(iChapter);
}

void CVDMPlayer::SeekTime(int64_t iTime)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	m_pCorePlayer->SeekTime(iTime / 1000);
}

bool CVDMPlayer::SeekTimeRelative(int64_t iTime)
{
	SeekTime(GetDisplayTime() + iTime);

	return true;
}

int64_t CVDMPlayer::GetTime()
{
	int64_t nTime = 0;
	if (m_pCorePlayer)
	{
		nTime = m_pCorePlayer->GetTime();
	}

	return nTime;
}

int64_t CVDMPlayer::GetDisplayTime()
{
	return GetTime() * 1000;
}

int64_t CVDMPlayer::GetTotalTime()
{
	int64_t nTime = 0;
	if (m_pCorePlayer)
	{
		nTime = m_pCorePlayer->GetTotalTime();
	}

	return nTime * 1000;
}

void CVDMPlayer::GetVideoStreamInfo(SPlayerVideoStreamInfo &info)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	VD_PlayerVideoStreamInfo* pInfo = m_pCorePlayer->GetVideoStreamInfo();
	if (!pInfo)
	{
		return;
	}

	info.bitrate = pInfo->bitrate;
	info.videoAspectRatio = pInfo->videoAspectRatio;
	info.height = pInfo->height;
	info.width = pInfo->width;
	info.SrcRect = CRect(pInfo->SrcRect.x1, pInfo->SrcRect.y1, pInfo->SrcRect.x2, pInfo->SrcRect.y2);
	info.DestRect = CRect(pInfo->DestRect.x1, pInfo->DestRect.y1, pInfo->DestRect.x2, pInfo->DestRect.y2);
	info.language = pInfo->language ? pInfo->language : "";
	info.name = pInfo->name ? pInfo->name : "";
	info.videoCodecName = pInfo->videoCodecName ? pInfo->videoCodecName : "";
	info.stereoMode = pInfo->stereoMode ? pInfo->stereoMode : "";

	m_pCorePlayer->ReleaseVideoStreamInfo(pInfo);
}

int CVDMPlayer::GetSourceBitrate()
{
	return 0;
}

bool CVDMPlayer::GetStreamDetails(CStreamDetails &details)
{
	return false;
}

void CVDMPlayer::ToFFRW(int iSpeed)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return;
	}

	m_pCorePlayer->ToFFRW(iSpeed);
}

bool CVDMPlayer::SkipNext()
{
	return false;
}

bool CVDMPlayer::IsCaching() const
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->IsCaching();
}

int CVDMPlayer::GetCacheLevel() const
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return 0;
	}

	return m_pCorePlayer->GetCacheLevel();
}

bool CVDMPlayer::IsInMenu() const
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->IsInMenu();
}

bool CVDMPlayer::HasMenu()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->HasMenu();
}

void CVDMPlayer::DoAudioWork()
{
}

bool CVDMPlayer::OnAction(const CAction &action)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	VD_Action actionID = VD_ACTION_NONE;

	switch (action.GetID())
	{
	case ACTION_NEXT_ITEM:
	{
		actionID = VD_ACTION_NEXT_ITEM;
		break;
	}
	case ACTION_MOVE_RIGHT:
	{
		actionID = VD_ACTION_MOVE_RIGHT;
		break;
	}
	case ACTION_MOVE_UP:
	{
		actionID = VD_ACTION_MOVE_UP;
		break;
	}
	case ACTION_SELECT_ITEM:
	{
		actionID = VD_ACTION_SELECT_ITEM;
		break;
	}
	case ACTION_PREV_ITEM:
	{
		actionID = VD_ACTION_PREV_ITEM;
		break;
	}
	case ACTION_SHOW_VIDEOMENU:
	{
		actionID = VD_ACTION_SHOW_VIDEOMENU;
		break;
	}
	case ACTION_PREVIOUS_MENU:
	{
		actionID = VD_ACTION_PREVIOUS_MENU;
		break;
	}
	case ACTION_NAV_BACK:
	{
		actionID = VD_ACTION_NAV_BACK;
		break;
	}
	case ACTION_MOVE_LEFT:
	{
		actionID = VD_ACTION_MOVE_LEFT;
		break;
	}
	case ACTION_MOVE_DOWN:
	{
		actionID = VD_ACTION_MOVE_DOWN;
		break;
	}
	case ACTION_MOUSE_MOVE:
	{
		actionID = VD_ACTION_MOUSE_MOVE;
		break;
	}
	case ACTION_MOUSE_LEFT_CLICK:
	{
		actionID = VD_ACTION_MOUSE_LEFT_CLICK;
		break;
	}
	case REMOTE_0:
	{
		actionID = VD_ACTION_REMOTE_0;
		break;
	}
	case REMOTE_1:
	{
		actionID = VD_ACTION_REMOTE_1;
		break;
	}
	case REMOTE_2:
	{
		actionID = VD_ACTION_REMOTE_2;
		break;
	}
	case REMOTE_3:
	{
		actionID = VD_ACTION_REMOTE_3;
		break;
	}
	case REMOTE_4:
	{
		actionID = VD_ACTION_REMOTE_4;
		break;
	}
	case REMOTE_5:
	{
		actionID = VD_ACTION_REMOTE_5;
		break;
	}
	case REMOTE_6:
	{
		actionID = VD_ACTION_REMOTE_6;
		break;
	}
	case REMOTE_7:
	{
		actionID = VD_ACTION_REMOTE_7;
		break;
	}
	case REMOTE_8:
	{
		actionID = VD_ACTION_REMOTE_8;
		break;
	}
	case REMOTE_9:
	{
		actionID = VD_ACTION_REMOTE_9;
		break;
	}
	case ACTION_CHANNEL_UP:
	{
		actionID = VD_ACTION_CHANNEL_UP;
		break;
	}
	case ACTION_CHANNEL_DOWN:
	{
		actionID = VD_ACTION_CHANNEL_DOWN;
		break;
	}
	case ACTION_CHANNEL_SWITCH:
	{
		actionID = VD_ACTION_CHANNEL_SWITCH;
		break;
	}
	default:
		break;
	}

	if (actionID != VD_ACTION_NONE)
	{
		return m_pCorePlayer->OnAction(actionID, action.GetAmount(0), action.GetAmount(1));
	}

	return false;
}

std::string CVDMPlayer::GetPlayerState()
{
	std::string strPlayerState;

	if (!m_pCorePlayer || !IsPlaying())
	{
		return "";
	}

	char* strState = m_pCorePlayer->GetPlayerState();
	if (strState)
	{
		strPlayerState = strState;
	}

	m_pCorePlayer->ReleasePlayerState(strState);

	return strPlayerState;
}

bool CVDMPlayer::SetPlayerState(const std::string& state)
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return false;
	}

	return m_pCorePlayer->SetPlayerState(state.c_str());
}

std::string CVDMPlayer::GetPlayingTitle()
{
	if (!m_pCorePlayer || !IsPlaying())
	{
		return "";
	}

	char* strTitle = m_pCorePlayer->GetPlayingTitle();
	if (!strTitle)
	{
		return "";
	}

	std::string strPlayerTitle = strTitle;

	m_pCorePlayer->ReleasePlayingTitle(strTitle);

	return strPlayerTitle;
}

bool CVDMPlayer::SwitchChannel(const PVR::CPVRChannelPtr &channel)
{
	return false;
}

void CVDMPlayer::GetAudioCapabilities(std::vector<int> &audioCaps)
{
	audioCaps.assign(1, IPC_AUD_ALL);
}

void CVDMPlayer::GetSubtitleCapabilities(std::vector<int> &subCaps)
{
	subCaps.assign(1, IPC_SUBS_ALL);
}

bool CVDMPlayer::IsSelfPresent()
{
	return true;
}

void CVDMPlayer::Present(void)
{
	if (!m_pPlayTool || !m_bIsPlaying)
	{
		return;
	}

	m_pPlayTool->BindRenderThreadID();
	m_pPlayTool->RenderManagerRender(true);
}

void CVDMPlayer::SetPlayMode(DIMENSIONMODE mode)
{
	if (!m_pPlayTool)
	{
		return;
	}

	VD_DisplayMode vd_mode = VD_DISPLAY_MODE_2D_ORIGINAL;
	switch (mode)
	{
	case VS_2D_DISPLAY_THE_ORIGINAL:
		vd_mode = VD_DISPLAY_MODE_2D_ORIGINAL;
		break;
	case VS_2D_DISPLAY_THE_LEFT_HALF:
		vd_mode = VD_DISPLAY_MODE_2D_LEFT_HALF;
		break;
	case VS_2D_DISPLAY_THE_UPPER_HALF:
		vd_mode = VD_DISPLAY_MODE_2D_UPPER_HALF;
		break;
	case VS_3D_LEFT_RIGHT:
		vd_mode = VD_DISPLAY_MODE_3D_LEFT_RIGHT;
		break;
	case VS_3D_TOM_BOTTOM:
		vd_mode = VD_DISPLAY_MODE_3D_TOM_BOTTOM;
		break;
	case VS_3D_INTERLACING:
		vd_mode = VD_DISPLAY_MODE_3D_INTERLACING;
		break;
	case VS_3D_LINE_INTERLACING:
		vd_mode = VD_DISPLAY_MODE_3D_LINE_INTERLACING;
		break;
	case VS_3D_COLUMN_INTERLACING:
		vd_mode = VD_DISPLAY_MODE_3D_COLUMN_INTERLACING;
		break;
	default:
		vd_mode = VD_DISPLAY_MODE_2D_ORIGINAL;
		break;
	}
	m_pPlayTool->SetDisplayMode(vd_mode);
}

void CVDMPlayer::UpdateWindowSize(void)
{
	if (!m_pPlayTool || !m_bIsPlaying)
	{
		return;
	}

	RESOLUTION res = g_graphicsContext.GetVideoResolution();
	int nx = CDisplaySettings::Get().GetResolutionInfo(res).iWidth;
	int ny = CDisplaySettings::Get().GetResolutionInfo(res).iHeight;

	m_pPlayTool->RenderWndResized(nx, ny);
}

void CVDMPlayer::NotifyAudioOutputSettingsChanged()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->NotifyAudioOutputSettingsChanged();
}

void CVDMPlayer::SetDeinterlaceMode(EDEINTERLACEMODE mode)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetDeinterlaceMode(ChangeDeinterlaceMode(mode));
}

void CVDMPlayer::SetInterlaceMethod(EINTERLACEMETHOD mode)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetInterlaceMethod(ChangeInterlaceMode(mode));
}

void CVDMPlayer::SetScalingMethod(ESCALINGMETHOD mode)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetScalingMethod(ChangeScalingMethod(mode));
}

void CVDMPlayer::SetCustomZoomAmount(float Value)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetCustomZoomAmount(Value);
}

void CVDMPlayer::SetCustomPixelRatio(float Value)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetCustomPixelRatio(Value);
}

void CVDMPlayer::SetCustomVerticalShift(float Value)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetCustomVerticalShift(Value);
}

void CVDMPlayer::SetCustomNonLinStretch(bool Value)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetCustomNonLinStretch(Value);
}

void CVDMPlayer::SetViewMode(ViewMode mode)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetViewMode(ChangeViewMod(mode));
}

void CVDMPlayer::NotifyViewModeChanged()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->NotifyViewModeChanged();
}

void CVDMPlayer::SetWhetherSupportAC3(bool bSupport)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetWhetherSupportAC3(bSupport);
}

float CVDMPlayer::GetAudioDelay()
{
	if (!m_pPlayTool)
	{
		return 0;
	}

	return m_pPlayTool->GetAudioDelay();
}

bool CVDMPlayer::GetSubtitleOn()
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->GetSubtitleOn();
}

void CVDMPlayer::SetSubColor(unsigned int color)
{
	if (!m_pPlayTool)
	{
		return;
	}

	unsigned int colormap[8] = { 0xFFFFFF00, 0xFFFFFFFF, 0xFF0099FF, 0xFF00FF00, 0xFFCCFF00, 0xFF00FFFF, 0xFFE5E5E5, 0xFFC0C0C0 };
	unsigned int nColor = colormap[color];

	m_pPlayTool->SetSubColor(nColor);
}

bool CVDMPlayer::SetSubtitleSize(int size)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return 	m_pPlayTool->SetSubtitleSize(size);
}

void CVDMPlayer::SetSubtitlePos(SubtitleAlign align, float yPos)
{
	if (!m_pPlayTool)
	{
		return;
	}

	float fYPos = 0.0;
	if (align == SUBTITLE_ALIGN_TOP_OUTSIDE)
	{
		fYPos = 0.95;
	}

	if (align == SUBTITLE_ALIGN_TOP_INSIDE)
	{
		fYPos = 0.85;
	}

	if (align == SUBTITLE_ALIGN_BOTTOM_INSIDE)
	{
		fYPos = 0.15;
	}

	m_pPlayTool->SetSubtitlePos(ChangeSubtitleAlign(align), fYPos);
}

void CVDMPlayer::SetSubtitleStyle(int nStyle)
{
	if (!m_pPlayTool)
	{
		return;
	}

	return 	m_pPlayTool->SetSubtitleStyle(nStyle);
}

void CVDMPlayer::SetSubtitleBold(bool bBold)
{
	if (!m_pPlayTool)
	{
		return;
	}

	return 	m_pPlayTool->SetSubtitleBold(bBold);
}

void CVDMPlayer::SetSubtitleItalic(bool bItalic)
{
	if (!m_pPlayTool)
	{
		return;
	}

	return 	m_pPlayTool->SetSubtitleItalic(bItalic);
}

void CVDMPlayer::SetBrightness(float fBrightness)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetBrightness(fBrightness);
}

void CVDMPlayer::SetContrast(float fContrast)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetContrast(fContrast);
}

void CVDMPlayer::SetHue(float fHue)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetHue(fHue);
}

void CVDMPlayer::SetSaturation(float fSaturation)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetSaturation(fSaturation);
}

void CVDMPlayer::SetVdpauNoiseRedution(float f)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetVdpauNoiseRedution(f);
}

void CVDMPlayer::SetPostProcessOn(bool on)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetPostProcessOn(on);
}

void CVDMPlayer::SetCropOn(bool on)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetCropOn(on);
}

void CVDMPlayer::SetStereoInvert(bool on)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetStereoInvert(on);
}

void CVDMPlayer::BindRenderThreadID()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->BindRenderThreadID();
}

void CVDMPlayer::RenderManagerRender(bool clear, unsigned int flags, unsigned int alpha)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerRender(clear, flags, alpha);
}

void CVDMPlayer::RenderManagerFlush()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerFlush();
}

void CVDMPlayer::RenderManagerFrameMove()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerFrameMove();
}

bool CVDMPlayer::RenderManagerIsStarted()
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->RenderManagerIsStarted();
}

void CVDMPlayer::RenderManagerFrameFinish()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerFrameFinish();
}

void CVDMPlayer::RenderManagerUpdate()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerUpdate();
}

RESOLUTION CVDMPlayer::RenderManagerGetResolution()
{
	if (!m_pPlayTool)
	{
		return RES_INVALID;
	}

	return ChangeResolutionEx(m_pPlayTool->RenderManagerGetResolution());
}

void CVDMPlayer::RenderManagerSetupScreenshot()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerSetupScreenshot();
}

float CVDMPlayer::RenderManagerGetAspectRatio()
{
	if (!m_pPlayTool)
	{
		return 0;
	}

	return m_pPlayTool->RenderManagerGetAspectRatio();
}

bool CVDMPlayer::RenderManagerSupports(EDEINTERLACEMODE mode)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->RenderManagerSupports(ChangeDeinterlaceMode(mode));
}

bool CVDMPlayer::RenderManagerSupports(EINTERLACEMETHOD mode)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->RenderManagerSupports(ChangeInterlaceMode(mode));
}

bool CVDMPlayer::RenderManagerSupports(ERENDERFEATURE mode)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->RenderManagerSupports(ChangeRenderFeature(mode));
}

bool CVDMPlayer::RenderManagerSupports(ESCALINGMETHOD mode)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->RenderManagerSupports(ChangeScalingMethod(mode));
}

void CVDMPlayer::RenderManagerSetViewMode(ViewMode mode)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerSetViewMode(ChangeViewMod(mode));

	g_dataCacheCore.SignalVideoInfoChange();
}

std::string CVDMPlayer::RenderManagerGetVSyncState()
{
	std::string strState;
	char* pstrState;

	if (m_pPlayTool)
	{
		pstrState = m_pPlayTool->RenderManagerGetVSyncState();

		if (pstrState)
		{
			strState = pstrState;
		}

		m_pPlayTool->RenderManagerReleaseVSyncState(pstrState);
	}

	return strState;
}

void CVDMPlayer::RenderManagerFrameWait(int ms)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerFrameWait(ms);
}

void CVDMPlayer::RenderManagerUpdateResolution()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerUpdateResolution();
}

void CVDMPlayer::RenderManagerManageCaptures()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->RenderManagerManageCaptures();
}

bool CVDMPlayer::SetCSettings(const char* id, const char* value)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->SetCSettings(id, value);
}

bool CVDMPlayer::SetCSettings(const char* id, double value)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->SetCSettings(id, value);
}

bool CVDMPlayer::SetCSettings(const char* id, int value)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->SetCSettings(id, value);
}

bool CVDMPlayer::SetCSettings(const char* id, bool value)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->SetCSettings(id, value);
}

void CVDMPlayer::SetScreen(int screen)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetScreen(screen);
}

#if defined(HAS_DX)
void CVDMPlayer::SetAdapter(unsigned int adapter)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetAdapter(adapter);
}

void CVDMPlayer::SetD3DPP(D3DPRESENT_PARAMETERS pp)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetD3DPP(pp);
}
#endif

void CVDMPlayer::SetStereoMode(RENDER_STEREO_MODE mode)
{
	if (!m_pPlayTool)
	{
		return;
	}

	if (mode == RENDER_STEREO_MODE_SPLIT_VERTICAL)
	{
		m_pPlayTool->SetStereoMode(VD_STEREOSCOPIC_MODE_SIDE_BY_SIDE);
	}

	if (mode == RENDER_STEREO_MODE_SPLIT_HORIZONTAL)
	{
		m_pPlayTool->SetStereoMode(VD_STEREOSCOPIC_MODE_OVER_UNDER);
	}

	if (mode == RENDER_STEREO_MODE_OFF)
	{
		m_pPlayTool->SetStereoMode(VD_STEREOSCOPIC_MODE_AUTO);
	}
}

void CVDMPlayer::SetStereoMode(RENDER_STEREO_MODE mode, RENDER_STEREO_VIEW view)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetStereoMode(ChangeStereoMode(mode), ChangeStereoView(view));

	if (mode == RENDER_STEREO_MODE_SPLIT_VERTICAL)
	{
		m_pPlayTool->SetStereoMode(VD_STEREOSCOPIC_MODE_SIDE_BY_SIDE);
	}

	if (mode == RENDER_STEREO_MODE_SPLIT_HORIZONTAL)
	{
		m_pPlayTool->SetStereoMode(VD_STEREOSCOPIC_MODE_OVER_UNDER);
	}

	if (mode == RENDER_STEREO_MODE_OFF)
	{
		m_pPlayTool->SetStereoMode(VD_STEREOSCOPIC_MODE_AUTO);
	}
}

void CVDMPlayer::SetWindowResolution(int width, int height)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetWindowResolution(width, height);

	g_dataCacheCore.SignalVideoInfoChange();
}

void CVDMPlayer::SetGraphicContextStereoMode(RENDER_STEREO_MODE mode)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextStereoMode(ChangeStereoMode(mode));
}

void CVDMPlayer::SetGraphicContextStereoView(RENDER_STEREO_VIEW view)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextStereoView(ChangeStereoView(view));
}

void CVDMPlayer::SetGraphicContextFullScreenRoot(bool on)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextFullScreenRoot(on);
}

void CVDMPlayer::SetGraphicContextFullScreenVideo(bool on)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextFullScreenVideo(on);
}

void CVDMPlayer::SetGraphicContextCalibrating(bool on)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextCalibrating(on);
}

void CVDMPlayer::SetGraphicContextVideoResolution(RESOLUTION res, bool bForce)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextVideoResolution(ChangeResolution(res), bForce);
}

void CVDMPlayer::SetGraphicContextVideoRect(float x1, float y1, float x2, float y2)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextVideoRect(x1, y1, x2, y2);
}

void CVDMPlayer::SetGraphicContextScreenWidth(int n)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextScreenWidth(n);
}

void CVDMPlayer::SetGraphicContextScreenHeight(int n)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextScreenHeight(n);
}

void CVDMPlayer::SetGraphicContextScissors(float x1, float y1, float x2, float y2)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetGraphicContextScissors(x1, y1, x2, y2);
}

void CVDMPlayer::SetRenderViewPort(float x1, float y1, float x2, float y2)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetRenderViewPort(x1, y1, x2, y2);
}

void CVDMPlayer::SetMaxTextureSize(unsigned int size)
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->SetMaxTextureSize(size);
}

void CVDMPlayer::AEDeviceChange()
{
	if (!m_pPlayTool)
	{
		return;
	}

	m_pPlayTool->AEDeviceChange();
}

bool CVDMPlayer::CaptureRenderImage(const char* strSaveUrl, int nWidth)
{
	if (!m_pPlayTool)
	{
		return false;
	}

	return m_pPlayTool->CaptureRenderImage(strSaveUrl, nWidth);
}

bool CVDMPlayer::InitPlayCore(void)
{
	m_pPlcore = CreateVDPlayer();
	m_pConfig = new CVDMPlcoreConfig();
	s_pPlcoreCallback = new CVDMPlcoreCallback();

	if (!m_pPlcore || !m_pConfig || !s_pPlcoreCallback)
	{
		DeInitPlayCore();

		return false;
	}

	m_pConfig->SetFontName(CSettings::Get().GetString("subtitles.font").c_str());
	m_pConfig->SetFontPath(CSpecialProtocol::TranslatePath("special://xbmcbin/media/Fonts/").c_str());
	m_pConfig->SetLogPath(g_advancedSettings.m_logFolder.c_str());
	m_pConfig->SetPlcoreRuntimesPath(CSpecialProtocol::TranslatePath("special://xbmcbin/system/players/vdmplayer/").c_str());
	m_pConfig->SetTempFolderPath(CSpecialProtocol::TranslatePath("special://temp/").c_str());
	m_pConfig->SetProfileFolderPath(CSpecialProtocol::TranslatePath("special://xbmcbin/system/players/vdmplayer/").c_str());
	m_pConfig->SetBDJResourcePath(CSpecialProtocol::TranslatePath("special://xbmcbin/system/players/vdmplayer/").c_str());

	if (!m_pPlcore->Initialize(m_pConfig,s_pPlcoreCallback))
	{
		DeInitPlayCore();

		return false;
	}

#if defined(TARGET_ANDROID)
	if (1)
	{
		AndroidRuntime::Get().m_pNativewindow = (ANativeWindow *)(*(CXBMCApp::GetNativeWindow(30000)));

		AndroidRuntime::Get().m_sConfig.handle = (void**)&(AndroidRuntime::Get().m_pNativewindow);
		AndroidRuntime::Get().m_sConfig.nWidth = AndroidRuntime::Get().m_pNativewindow ? ANativeWindow_getWidth(AndroidRuntime::Get().m_pNativewindow) : 0;
		AndroidRuntime::Get().m_sConfig.nHeight = AndroidRuntime::Get().m_pNativewindow ? ANativeWindow_getHeight(AndroidRuntime::Get().m_pNativewindow) : 0;

		//	vidonconfig.pNativeActivity = CXBMCApp::GetCurrentActivity();
		//	vidonconfig.pNativeWindows = (ANativeWindow *)(*(CXBMCApp::GetNativeWindow (30000)));
	}
#endif

	return true;
}

void CVDMPlayer::DeInitPlayCore(void)
{
	if (m_pPlcore)
	{
		ReleaseVDPlayer(m_pPlcore);
		m_pPlcore = 0;
	}

	if (m_pConfig)
	{
		delete m_pConfig;
		m_pConfig = 0;
	}
	if (s_pPlcoreCallback)
	{
		delete s_pPlcoreCallback;
		s_pPlcoreCallback = 0;
	}
}

void CVDMPlayer::Process()
{
	if (!InitPlayer() || m_bStop)
	{
		m_event.Set();
		return;
	}

	if (!m_pCorePlayer->OpenFile(&m_itemPlay, &m_optionsPlay, NULL))
	{
		DeInitPlayer();
		m_event.Set();

		return;
	}

	while (!m_pCorePlayer->IsPlaying())
	{
		Sleep(1);
	}

	m_bIsPlaying = true;

	m_event.Set();
}

bool CVDMPlayer::InitPlayer(void)
{	
#if defined(HAS_EGL)
	m_pPlayToolConfig->SetEGLDisplay(g_Windowing.GetEGLDisplay());
	m_pPlayToolConfig->SetEGLContext(g_Windowing.GetEGLContext());
#endif
	SetCSettings("window.width", CSettings::Get().GetInt("window.width"));
	SetCSettings("window.height", CSettings::Get().GetInt("window.height"));

	// videoplayer
	SetCSettings("locale.audiolanguage", CSettings::Get().GetString("locale.audiolanguage").c_str());
	SetCSettings("videoplayer.preferdefaultflag", CSettings::Get().GetBool("videoplayer.preferdefaultflag"));
	SetCSettings("videoplayer.autoplaynextitem", CSettings::Get().GetBool("videoplayer.autoplaynextitem"));
	SetCSettings("videoplayer.seeksteps", CSettings::Get().GetInt("videoplayer.seeksteps"));
	SetCSettings("videoplayer.seekdelay", CSettings::Get().GetInt("videoplayer.seekdelay"));
	SetCSettings("videoplayer.adjustrefreshrate", CSettings::Get().GetInt("videoplayer.adjustrefreshrate"));
	SetCSettings("videoplayer.pauseafterrefreshchange", CSettings::Get().GetInt("videoplayer.pauseafterrefreshchange"));
	SetCSettings("videoplayer.usedisplayasclock", CSettings::Get().GetBool("videoplayer.usedisplayasclock"));
	SetCSettings("videoplayer.errorinaspect", CSettings::Get().GetInt("videoplayer.errorinaspect"));
	SetCSettings("videoplayer.stretch43", CSettings::Get().GetInt("videoplayer.stretch43"));
	SetCSettings("videoplayer.teletextenabled", CSettings::Get().GetBool("videoplayer.teletextenabled"));
	SetCSettings("videoplayer.teletextscale", CSettings::Get().GetBool("videoplayer.teletextscale"));
	SetCSettings("videoplayer.stereoscopicplaybackmode", CSettings::Get().GetInt("videoplayer.stereoscopicplaybackmode"));
	SetCSettings("videoplayer.quitstereomodeonstop", CSettings::Get().GetBool("videoplayer.quitstereomodeonstop"));

	// videoacceleration
	SetCSettings("videoplayer.rendermethod", CSettings::Get().GetInt("videoplayer.rendermethod"));
	SetCSettings("videoplayer.hqscalers", CSettings::Get().GetInt("videoplayer.hqscalers"));
	SetCSettings("videoplayer.useamcodec", CSettings::Get().GetBool("videoplayer.useamcodec"));
	SetCSettings("videoplayer.useawcodec", CSettings::Get().GetBool("videoplayer.useawcodec"));
	SetCSettings("videoplayer.userkcodec", CSettings::Get().GetBool("videoplayer.userkcodec"));
	SetCSettings("videoplayer.usea31codec", CSettings::Get().GetBool("videoplayer.usea31codec"));
	SetCSettings("videoplayer.usevdpau", CSettings::Get().GetBool("videoplayer.usevdpau"));
	SetCSettings("videoplayer.usevdpaumixer", CSettings::Get().GetBool("videoplayer.usevdpaumixer"));
	SetCSettings("videoplayer.usevdpaumpeg2", CSettings::Get().GetBool("videoplayer.usevdpaumpeg2"));
	SetCSettings("videoplayer.usevdpaumpeg4", CSettings::Get().GetBool("videoplayer.usevdpaumpeg4"));
	SetCSettings("videoplayer.usevdpauvc1", CSettings::Get().GetBool("videoplayer.usevdpauvc1"));
	SetCSettings("videoplayer.usevaapi", CSettings::Get().GetBool("videoplayer.usevaapi"));
	SetCSettings("videoplayer.usevaapimpeg2", CSettings::Get().GetBool("videoplayer.usevaapimpeg2"));
	SetCSettings("videoplayer.usevaapimpeg4", CSettings::Get().GetBool("videoplayer.usevaapimpeg4"));
	SetCSettings("videoplayer.usevaapivc1", CSettings::Get().GetBool("videoplayer.usevaapivc1"));
	SetCSettings("videoplayer.prefervaapirender", CSettings::Get().GetBool("videoplayer.prefervaapirender"));
	SetCSettings("videoplayer.usedxva2", CSettings::Get().GetBool("videoplayer.usedxva2"));
	SetCSettings("videoplayer.useomxplayer", CSettings::Get().GetBool("videoplayer.useomxplayer"));
	SetCSettings("videoplayer.useomx", CSettings::Get().GetBool("videoplayer.useomx"));
	SetCSettings("videoplayer.usevideotoolbox", CSettings::Get().GetBool("videoplayer.usevideotoolbox"));
	SetCSettings("videoplayer.usevda", CSettings::Get().GetBool("videoplayer.usevda"));
	SetCSettings("videoplayer.usemediacodec", CSettings::Get().GetBool("videoplayer.usemediacodec"));
	SetCSettings("videoplayer.usestagefright", CSettings::Get().GetBool("videoplayer.usestagefright"));

	// subtitles
	SetCSettings("locale.subtitlelanguage", CSettings::Get().GetString("locale.subtitlelanguage").c_str());
	SetCSettings("subtitles.parsecaptions", CSettings::Get().GetBool("subtitles.parsecaptions"));
	SetCSettings("subtitles.align", CSettings::Get().GetInt("subtitles.align"));
	SetCSettings("subtitles.stereoscopicdepth", CSettings::Get().GetInt("subtitles.stereoscopicdepth"));
	SetCSettings("subtitles.font", CSettings::Get().GetString("subtitles.font").c_str());
	SetCSettings("subtitles.height", CSettings::Get().GetInt("subtitles.height"));
	SetCSettings("subtitles.style", CSettings::Get().GetInt("subtitles.style"));
	SetCSettings("subtitles.color", CSettings::Get().GetInt("subtitles.color"));
	SetCSettings("subtitles.charset", CSettings::Get().GetString("subtitles.charset").c_str());
	SetCSettings("subtitles.overrideassfonts", CSettings::Get().GetBool("subtitles.overrideassfonts"));
	SetCSettings("subtitles.languages", CSettings::Get().GetString("subtitles.languages").c_str());
	SetCSettings("subtitles.storagemode", CSettings::Get().GetInt("subtitles.storagemode"));
	SetCSettings("subtitles.custompath", CSettings::Get().GetString("subtitles.custompath").c_str());
	SetCSettings("subtitles.pauseonsearch", CSettings::Get().GetBool("subtitles.pauseonsearch"));
	SetCSettings("subtitles.downloadfirst", CSettings::Get().GetBool("subtitles.downloadfirst"));

	// dvds
	SetCSettings("dvds.autorun", CSettings::Get().GetBool("dvds.autorun"));
	SetCSettings("dvds.playerregion", CSettings::Get().GetInt("dvds.playerregion"));
	SetCSettings("dvds.automenu", CSettings::Get().GetBool("dvds.automenu"));
	SetCSettings("disc.playback", CSettings::Get().GetInt("disc.playback"));

	// videoscreen
	SetCSettings("videoscreen.monitor", CSettings::Get().GetString("videoscreen.monitor").c_str());
	SetCSettings("videoscreen.screen", CSettings::Get().GetInt("videoscreen.screen"));
	SetCSettings("videoscreen.resolution", CSettings::Get().GetInt("videoscreen.resolution"));
	SetCSettings("videoscreen.screenmode", CSettings::Get().GetString("videoscreen.screenmode").c_str());
	SetCSettings("videoscreen.fakefullscreen", CSettings::Get().GetBool("videoscreen.fakefullscreen"));
	SetCSettings("videoscreen.blankdisplays", CSettings::Get().GetBool("videoscreen.blankdisplays"));
	SetCSettings("videoscreen.stereoscopicmode", CSettings::Get().GetInt("videoscreen.stereoscopicmode"));
	SetCSettings("videoscreen.preferedstereoscopicmode", CSettings::Get().GetInt("videoscreen.preferedstereoscopicmode"));
	SetCSettings("videoscreen.vsync", CSettings::Get().GetInt("videoscreen.vsync"));
	SetCSettings("videoscreen.limitedrange", CSettings::Get().GetBool("videoscreen.limitedrange"));

	// audio output
	SetCSettings("audiooutput.audiodevice", CSettings::Get().GetString("audiooutput.audiodevice").c_str());
	SetCSettings("audiooutput.channels", CSettings::Get().GetInt("audiooutput.channels"));
	SetCSettings("audiooutput.config", CSettings::Get().GetInt("audiooutput.config"));
	SetCSettings("audiooutput.samplerate", CSettings::Get().GetInt("audiooutput.samplerate"));
	SetCSettings("audiooutput.stereoupmix", CSettings::Get().GetBool("audiooutput.stereoupmix"));
	SetCSettings("audiooutput.maintainoriginalvolume", CSettings::Get().GetBool("audiooutput.maintainoriginalvolume"));
	SetCSettings("audiooutput.processquality", CSettings::Get().GetInt("audiooutput.processquality"));
	SetCSettings("audiooutput.streamsilence", CSettings::Get().GetInt("audiooutput.streamsilence"));
	SetCSettings("audiooutput.guisoundmode", CSettings::Get().GetInt("audiooutput.guisoundmode"));
	SetCSettings("audiooutput.passthrough", CSettings::Get().GetBool("audiooutput.passthrough"));
	SetCSettings("audiooutput.passthroughdevice", CSettings::Get().GetString("audiooutput.passthroughdevice").c_str());
	SetCSettings("audiooutput.ac3passthrough", CSettings::Get().GetBool("audiooutput.ac3passthrough"));
	SetCSettings("audiooutput.ac3transcode", CSettings::Get().GetBool("audiooutput.ac3transcode"));
	SetCSettings("audiooutput.eac3passthrough", CSettings::Get().GetBool("audiooutput.eac3passthrough"));
	SetCSettings("audiooutput.dtspassthrough", CSettings::Get().GetBool("audiooutput.dtspassthrough"));
	SetCSettings("audiooutput.truehdpassthrough", CSettings::Get().GetBool("audiooutput.truehdpassthrough"));
	SetCSettings("audiooutput.dtshdpassthrough", CSettings::Get().GetBool("audiooutput.dtshdpassthrough"));

	NotifyAudioOutputSettingsChanged();

	/*
	if (!m_pPlayTool->Init())
	{
		DeInitPlayer();

		return false;
	}
	*/

#if defined(HAS_DX)
	m_pPlayTool->SetD3DPP(g_Windowing.GetD3DPP());
	m_pPlayTool->SetAdapter(g_Windowing.GetAdapter());
#endif

	SetDeinterlaceMode(CMediaSettings::Get().GetCurrentVideoSettings().m_DeinterlaceMode);
	SetInterlaceMethod(CMediaSettings::Get().GetCurrentVideoSettings().m_InterlaceMethod);
	SetScalingMethod(CMediaSettings::Get().GetCurrentVideoSettings().m_ScalingMethod);
	SetViewMode((ViewMode)CMediaSettings::Get().GetCurrentVideoSettings().m_ViewMode);
	SetCustomZoomAmount(CMediaSettings::Get().GetCurrentVideoSettings().m_CustomZoomAmount);
	SetCustomPixelRatio(CMediaSettings::Get().GetCurrentVideoSettings().m_CustomPixelRatio);
	SetCustomVerticalShift(CMediaSettings::Get().GetCurrentVideoSettings().m_CustomVerticalShift);
	SetCustomNonLinStretch(CMediaSettings::Get().GetCurrentVideoSettings().m_CustomNonLinStretch);
	SetDynamicRangeCompression(CMediaSettings::Get().GetCurrentVideoSettings().m_VolumeAmplification * 100);
	SetBrightness(CMediaSettings::Get().GetCurrentVideoSettings().m_Brightness);
	SetContrast(CMediaSettings::Get().GetCurrentVideoSettings().m_Contrast);
	SetVdpauNoiseRedution(CMediaSettings::Get().GetCurrentVideoSettings().m_NoiseReduction);
	SetPostProcessOn(CMediaSettings::Get().GetCurrentVideoSettings().m_PostProcess);
	SetAVDelay(CMediaSettings::Get().GetCurrentVideoSettings().m_AudioDelay);
	SetStereoMode((RENDER_STEREO_MODE)CMediaSettings::Get().GetCurrentVideoSettings().m_StereoMode);
	SetStereoInvert(CMediaSettings::Get().GetCurrentVideoSettings().m_StereoInvert);

	SetPlayMode((DIMENSIONMODE)CSettings::Get().GetInt("d3.mode"));

	m_pCorePlayer = m_pPlayTool->GetPlayer(m_pPlcorePlayerCallback);

	if (!m_pCorePlayer)
	{
		DeInitPlayer();

		return false;
	}

	return true;
}

void CVDMPlayer::DeInitPlayer(void)
{
	if (m_bIsPlaying)
	{
		CloseFile();
	}

	if (m_pPlayTool && m_pCorePlayer)
	{
		m_pPlayTool->ReleasePlayer(m_pCorePlayer);
		m_pCorePlayer = 0;
	}

	if (m_pPlayTool)
	{
		m_pPlayTool->UnInit();
		m_pPlcore->ReleasePlayTool(m_pPlayTool);
		m_pPlayTool = 0;
	}

	if (m_pPlcorePlayerCallback)
	{
		delete m_pPlcorePlayerCallback;
		m_pPlcorePlayerCallback = 0;
	}

	if (m_pPlayToolConfig)
	{
		delete m_pPlayToolConfig;
		m_pPlayToolConfig = 0;
	}

	if (m_pPlcorePlaytoolCallback)
	{
		delete m_pPlcorePlaytoolCallback;
		m_pPlcorePlaytoolCallback = 0;
	}
}

void CVDMPlayer::SetSubttileFeatures(void)
{
	SetSubtitlePos((SubtitleAlign)CSettings::Get().GetInt("subtitles.align"), 0);
	SetSubColor(CSettings::Get().GetInt("subtitles.color"));
	SetSubtitleSize(CSettings::Get().GetInt("subtitles.height"));
	SetSubtitleStyle(CSettings::Get().GetInt("subtitles.style"));

	switch (CSettings::Get().GetInt("subtitles.style"))
	{
	case 0:
	{
		SetSubtitleBold(false);
		SetSubtitleItalic(false);
		break;
	}
	case 1:
	{
		SetSubtitleBold(true);
		SetSubtitleItalic(false);
		break;
	}
	case 2:
	{
		SetSubtitleBold(false);
		SetSubtitleItalic(true);
		break;
	}
	case 3:
	{
		SetSubtitleBold(true);
		SetSubtitleItalic(true);
		break;
	}
	default:
		break;
	}
}

VD_RenderStereoMode CVDMPlayer::ChangeStereoMode(RENDER_STEREO_MODE modeIn)
{
	VD_RenderStereoMode modeOut = VD_RENDER_STEREO_MODE_OFF;
	switch (modeIn)
	{
	case RENDER_STEREO_MODE_OFF:
	{
		modeOut = VD_RENDER_STEREO_MODE_OFF;
		break;
	}
	case RENDER_STEREO_MODE_SPLIT_HORIZONTAL:
	{
		modeOut = VD_RENDER_STEREO_MODE_SPLIT_HORIZONTAL;
		break;
	}
	case RENDER_STEREO_MODE_SPLIT_VERTICAL:
	{
		modeOut = VD_RENDER_STEREO_MODE_SPLIT_VERTICAL;
		break;
	}
	case RENDER_STEREO_MODE_ANAGLYPH_RED_CYAN:
	{
		modeOut = VD_RENDER_STEREO_MODE_ANAGLYPH_RED_CYAN;
		break;
	}
	case RENDER_STEREO_MODE_ANAGLYPH_GREEN_MAGENTA:
	{
		modeOut = VD_RENDER_STEREO_MODE_ANAGLYPH_GREEN_MAGENTA;
		break;
	}
	case RENDER_STEREO_MODE_INTERLACED:
	{
		modeOut = VD_RENDER_STEREO_MODE_INTERLACED;
		break;
	}
	case RENDER_STEREO_MODE_HARDWAREBASED:
	{
		modeOut = VD_RENDER_STEREO_MODE_HARDWAREBASED;
		break;
	}
	case RENDER_STEREO_MODE_MONO:
	{
		modeOut = VD_RENDER_STEREO_MODE_MONO;
		break;
	}
	case RENDER_STEREO_MODE_COUNT:
	{
		modeOut = VD_RENDER_STEREO_MODE_COUNT;
		break;
	}
	case RENDER_STEREO_MODE_AUTO:
	{
		modeOut = VD_RENDER_STEREO_MODE_AUTO;
		break;
	}
	default:
		break;
	}

	return modeOut;
}

VD_RenderStereoView CVDMPlayer::ChangeStereoView(RENDER_STEREO_VIEW viewIn)
{
	VD_RenderStereoView viewOut = VD_RENDER_STEREO_VIEW_OFF;

	switch (viewIn)
	{
	case RENDER_STEREO_VIEW_OFF:
	{
		viewOut = VD_RENDER_STEREO_VIEW_OFF;
		break;
	}
	case RENDER_STEREO_VIEW_LEFT:
	{
		viewOut = VD_RENDER_STEREO_VIEW_LEFT;
		break;
	}
	case RENDER_STEREO_VIEW_RIGHT:
	{
		viewOut = VD_RENDER_STEREO_VIEW_RIGHT;
		break;
	}
	default:
		break;
	}

	return viewOut;
}

VD_Resolution CVDMPlayer::ChangeResolution(RESOLUTION viewIn)
{
	VD_Resolution res = VD_RESOLUTION_INVALID;

	switch (viewIn)
	{
	case RES_INVALID:
	{
		res = VD_RESOLUTION_INVALID;
		break;
	}
	case RES_HDTV_1080i:
	{
		res = VD_RESOLUTION_HDTV_1080i;
		break;
	}
	case RES_HDTV_720pSBS:
	{
		res = VD_RESOLUTION_HDTV_720pSBS;
		break;
	}
	case RES_HDTV_720pTB:
	{
		res = VD_RESOLUTION_HDTV_720pTB;
		break;
	}
	case RES_HDTV_1080pSBS:
	{
		res = VD_RESOLUTION_HDTV_1080pSBS;
		break;
	}
	case RES_HDTV_1080pTB:
	{
		res = VD_RESOLUTION_HDTV_1080pTB;
		break;
	}
	case RES_HDTV_720p:
	{
		res = VD_RESOLUTION_HDTV_720p;
		break;
	}
	case RES_HDTV_480p_4x3:
	{
		res = VD_RESOLUTION_HDTV_480p_4x3;
		break;
	}
	case RES_HDTV_480p_16x9:
	{
		res = VD_RESOLUTION_HDTV_480p_16x9;
		break;
	}
	case RES_NTSC_4x3:
	{
		res = VD_RESOLUTION_NTSC_4x3;
		break;
	}
	case RES_NTSC_16x9:
	{
		res = VD_RESOLUTION_NTSC_16x9;
		break;
	}
	case RES_PAL_4x3:
	{
		res = VD_RESOLUTION_PAL_4x3;
		break;
	}
	case RES_PAL_16x9:
	{
		res = VD_RESOLUTION_PAL_16x9;
		break;
	}
	case RES_PAL60_4x3:
	{
		res = VD_RESOLUTION_PAL60_4x3;
		break;
	}
	case RES_PAL60_16x9:
	{
		res = VD_RESOLUTION_PAL60_16x9;
		break;
	}
	case RES_AUTORES:
	{
		res = VD_RESOLUTION_AUTORES;
		break;
	}
	case RES_WINDOW:
	{
		res = VD_RESOLUTION_WINDOW;
		break;
	}
	case RES_DESKTOP:
	{
		res = VD_RESOLUTION_DESKTOP;
		break;
	}
	case RES_CUSTOM:
	{
		res = VD_RESOLUTION_CUSTOM;
		break;
	}
	default:
		break;
	}

	return res;
}

RESOLUTION CVDMPlayer::ChangeResolutionEx(VD_Resolution viewIn)
{
	RESOLUTION res = RES_INVALID;

	switch (viewIn)
	{
	case VD_RESOLUTION_INVALID:
	{
		res = RES_INVALID;
		break;
	}
	case VD_RESOLUTION_HDTV_1080i:
	{
		res = RES_HDTV_1080i;
		break;
	}
	case VD_RESOLUTION_HDTV_720pSBS:
	{
		res = RES_HDTV_720pSBS;
		break;
	}
	case VD_RESOLUTION_HDTV_720pTB:
	{
		res = RES_HDTV_720pTB;
		break;
	}
	case VD_RESOLUTION_HDTV_1080pSBS:
	{
		res = RES_HDTV_1080pSBS;
		break;
	}
	case VD_RESOLUTION_HDTV_1080pTB:
	{
		res = RES_HDTV_1080pTB;
		break;
	}
	case VD_RESOLUTION_HDTV_720p:
	{
		res = RES_HDTV_720p;
		break;
	}
	case VD_RESOLUTION_HDTV_480p_4x3:
	{
		res = RES_HDTV_480p_4x3;
		break;
	}
	case VD_RESOLUTION_HDTV_480p_16x9:
	{
		res = RES_HDTV_480p_16x9;
		break;
	}
	case VD_RESOLUTION_NTSC_4x3:
	{
		res = RES_NTSC_4x3;
		break;
	}
	case VD_RESOLUTION_NTSC_16x9:
	{
		res = RES_NTSC_16x9;
		break;
	}
	case VD_RESOLUTION_PAL_4x3:
	{
		res = RES_PAL_4x3;
		break;
	}
	case VD_RESOLUTION_PAL_16x9:
	{
		res = RES_PAL_16x9;
		break;
	}
	case VD_RESOLUTION_PAL60_4x3:
	{
		res = RES_PAL60_4x3;
		break;
	}
	case VD_RESOLUTION_PAL60_16x9:
	{
		res = RES_PAL60_16x9;
		break;
	}
	case VD_RESOLUTION_AUTORES:
	{
		res = RES_AUTORES;
		break;
	}
	case VD_RESOLUTION_WINDOW:
	{
		res = RES_WINDOW;
		break;
	}
	case VD_RESOLUTION_DESKTOP:
	{
		res = RES_DESKTOP;
		break;
	}
	case VD_RESOLUTION_CUSTOM:
	{
		res = RES_CUSTOM;
		break;
	}
	default:
		break;
	}

	return res;
}

VD_DeinterlaceMode CVDMPlayer::ChangeDeinterlaceMode(EDEINTERLACEMODE modeIn)
{
	VD_DeinterlaceMode modeOut = VD_DEINTERLACE_MODE_OFF;

	switch (modeIn)
	{
	case VS_DEINTERLACEMODE_OFF:
	{
		modeOut = VD_DEINTERLACE_MODE_OFF;
		break;
	}
	case VS_DEINTERLACEMODE_AUTO:
	{
		modeOut = VD_DEINTERLACE_MODE_AUTO;
		break;
	}
	case VS_DEINTERLACEMODE_FORCE:
	{
		modeOut = VD_DEINTERLACE_MODE_FORCE;
		break;
	}
	default:
		break;
	}

	return modeOut;
}

VD_InterlaceMethod CVDMPlayer::ChangeInterlaceMode(EINTERLACEMETHOD modeIn)
{
	VD_InterlaceMethod modeOut = VD_INTERLACE_METHOD_NONE;

	switch (modeIn)
	{
	case VS_INTERLACEMETHOD_NONE:
	{
		modeOut = VD_INTERLACE_METHOD_NONE;
		break;
	}
	case VS_INTERLACEMETHOD_AUTO:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_AUTO;
		break;
	}
	case VS_INTERLACEMETHOD_RENDER_BLEND:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_RENDER_BLEND;
		break;
	}
	case VS_INTERLACEMETHOD_RENDER_WEAVE_INVERTED:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_RENDER_WEAVE_INVERTED;
		break;
	}
	case VS_INTERLACEMETHOD_RENDER_WEAVE:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_RENDER_WEAVE;
		break;
	}
	case VS_INTERLACEMETHOD_RENDER_BOB_INVERTED:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_RENDER_BOB_INVERTED;
		break;
	}
	case VS_INTERLACEMETHOD_RENDER_BOB:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_RENDER_BOB;
		break;
	}
	case VS_INTERLACEMETHOD_DEINTERLACE:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_DEINTERLACE;
		break;
	}
	case VS_INTERLACEMETHOD_VDPAU_BOB:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VDPAU_BOB;
		break;
	}
	case VS_INTERLACEMETHOD_INVERSE_TELECINE:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_INVERSE_TELECINE;
		break;
	}
	case VS_INTERLACEMETHOD_VDPAU_INVERSE_TELECINE:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VDPAU_INVERSE_TELECINE;
		break;
	}
	case VS_INTERLACEMETHOD_VDPAU_TEMPORAL:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VDPAU_TEMPORAL;
		break;
	}
	case VS_INTERLACEMETHOD_VDPAU_TEMPORAL_HALF:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VDPAU_TEMPORAL_HALF;
		break;
	}
	case VS_INTERLACEMETHOD_VDPAU_TEMPORAL_SPATIAL:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VDPAU_TEMPORAL_SPATIAL;
		break;
	}
	case VS_INTERLACEMETHOD_VDPAU_TEMPORAL_SPATIAL_HALF:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VDPAU_TEMPORAL_SPATIAL_HALF;
		break;
	}
	case VS_INTERLACEMETHOD_DEINTERLACE_HALF:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_DEINTERLACE_HALF;
		break;
	}
	case VS_INTERLACEMETHOD_DXVA_BOB:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_DXVA_BOB;
		break;
	}
	case VS_INTERLACEMETHOD_DXVA_BEST:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_DXVA_BEST;
		break;
	}
	case VS_INTERLACEMETHOD_SW_BLEND:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_SW_BLEND;
		break;
	}
	case VS_INTERLACEMETHOD_AUTO_ION:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_AUTO_ION;
		break;
	}
	case VS_INTERLACEMETHOD_VAAPI_BOB:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VAAPI_BOB;
		break;
	}
	case VS_INTERLACEMETHOD_VAAPI_MADI:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VAAPI_MADI;
		break;
	}
	case VS_INTERLACEMETHOD_VAAPI_MACI:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_VAAPI_MACI;
		break;
	}
	case VS_INTERLACEMETHOD_MMAL_ADVANCED:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_MMAL_ADVANCED;
		break;
	}
	case VS_INTERLACEMETHOD_MMAL_ADVANCED_HALF:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_MMAL_ADVANCED_HALF;
		break;
	}
	case VS_INTERLACEMETHOD_MMAL_BOB:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_MMAL_BOB;
		break;
	}
	case VS_INTERLACEMETHOD_MMAL_BOB_HALF:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_MMAL_BOB_HALF;
		break;
	}
	case VS_INTERLACEMETHOD_IMX_FASTMOTION:
	{
		break;
	}
	case VS_INTERLACEMETHOD_IMX_FASTMOTION_DOUBLE:
	{
		break;
	}
	case VS_INTERLACEMETHOD_MAX:
	{
		modeOut = VD_INTERLACE_METHOD_NONE_MAX;
		break;
	}
	default:
		break;
	}

	return modeOut;
}

VD_ScalingMethod CVDMPlayer::ChangeScalingMethod(ESCALINGMETHOD modeIn)
{
	VD_ScalingMethod modeOut = VD_SCALING_METHOD_NEAREST;

	switch (modeIn)
	{
	case VS_SCALINGMETHOD_NEAREST:
	{
		modeOut = VD_SCALING_METHOD_NEAREST;
		break;
	}
	case VS_SCALINGMETHOD_LINEAR:
	{
		modeOut = VD_SCALING_METHOD_LINEAR;
		break;
	}
	case VS_SCALINGMETHOD_CUBIC:
	{
		modeOut = VD_SCALING_METHOD_CUBIC;
		break;
	}
	case VS_SCALINGMETHOD_LANCZOS2:
	{
		modeOut = VD_SCALING_METHOD_LANCZOS2;
		break;
	}
	case VS_SCALINGMETHOD_LANCZOS3_FAST:
	{
		modeOut = VD_SCALING_METHOD_LANCZOS3_FAST;
		break;
	}
	case VS_SCALINGMETHOD_LANCZOS3:
	{
		modeOut = VD_SCALING_METHOD_LANCZOS3;
		break;
	}
	case VS_SCALINGMETHOD_SINC8:
	{
		modeOut = VD_SCALING_METHOD_SINC8;
		break;
	}
	case VS_SCALINGMETHOD_NEDI:
	{
		modeOut = VD_SCALING_METHOD_NEDI;
		break;
	}
	case VS_SCALINGMETHOD_BICUBIC_SOFTWARE:
	{
		modeOut = VD_SCALING_METHOD_BICUBIC_SOFTWARE;
		break;
	}
	case VS_SCALINGMETHOD_LANCZOS_SOFTWARE:
	{
		modeOut = VD_SCALING_METHOD_LANCZOS_SOFTWARE;
		break;
	}
	case VS_SCALINGMETHOD_SINC_SOFTWARE:
	{
		modeOut = VD_SCALING_METHOD_SINC_SOFTWARE;
		break;
	}
	case VS_SCALINGMETHOD_VDPAU_HARDWARE:
	{
		modeOut = VD_SCALING_METHOD_VDPAU_HARDWARE;
		break;
	}
	case VS_SCALINGMETHOD_DXVA_HARDWARE:
	{
		modeOut = VD_SCALING_METHOD_DXVA_HARDWARE;
		break;
	}
	case VS_SCALINGMETHOD_AUTO:
	{
		modeOut = VD_SCALING_METHOD_AUTO;
		break;
	}
	case VS_SCALINGMETHOD_SPLINE36_FAST:
	{
		modeOut = VD_SCALING_METHOD_SPLINE36_FAST;
		break;
	}
	case VS_SCALINGMETHOD_SPLINE36:
	{
		modeOut = VD_SCALING_METHOD_SPLINE36;
		break;
	}
	case VS_SCALINGMETHOD_MAX:
	{
		modeOut = VD_SCALING_METHOD_MAX;
		break;
	}
	default:
		break;
	}

	return modeOut;
}

VD_ViewMode CVDMPlayer::ChangeViewMod(ViewMode modeIn)
{
	VD_ViewMode modeOut = VD_VIEW_MODE_NORMAL;

	switch (modeIn)
	{
	case ViewModeNormal:
	{
		modeOut = VD_VIEW_MODE_NORMAL;
		break;
	}
	case ViewModeZoom:
	{
		modeOut = VD_VIEW_MODE_ZOOM;
		break;
	}
	case ViewModeStretch4x3:
	{
		modeOut = VD_VIEW_MODE_STRETCH4x3;
		break;
	}
	case ViewModeWideZoom:
	{
		modeOut = VD_VIEW_MODE_WIDEZOOM;
		break;
	}
	case ViewModeStretch16x9:
	{
		modeOut = VD_VIEW_MODE_STRETCH16x9;
		break;
	}
	case ViewModeOriginal:
	{
		modeOut = VD_VIEW_MODE_ORIGINAL;
		break;
	}
	case ViewModeCustom:
	{
		modeOut = VD_VIEW_MODE_CUSTOM;
		break;
	}
	default:
		break;
	}

	return modeOut;
}

VD_SubtitleAlign CVDMPlayer::ChangeSubtitleAlign(SubtitleAlign alignIn)
{
	VD_SubtitleAlign alignOut = VD_SUBTITLE_ALIGN_MANUAL;

	switch (alignIn)
	{
	case SUBTITLE_ALIGN_MANUAL:
	{
		alignOut = VD_SUBTITLE_ALIGN_MANUAL;
		break;
	}
	case SUBTITLE_ALIGN_BOTTOM_INSIDE:
	{
		alignOut = VD_SUBTITLE_ALIGN_BOTTOM_INSIDE;
		break;
	}
	case SUBTITLE_ALIGN_BOTTOM_OUTSIDE:
	{
		alignOut = VD_SUBTITLE_ALIGN_BOTTOM_OUTSIDE;
		break;
	}
	case SUBTITLE_ALIGN_TOP_INSIDE:
	{
		alignOut = VD_SUBTITLE_ALIGN_TOP_INSIDE;
		break;
	}
	case SUBTITLE_ALIGN_TOP_OUTSIDE:
	{
		alignOut = VD_SUBTITLE_ALIGN_TOP_OUTSIDE;
		break;
	}
	default:
		break;
	}

	return alignOut;
}

VD_RenderFeature CVDMPlayer::ChangeRenderFeature(ERENDERFEATURE featureIn)
{
	VD_RenderFeature featureOut = VD_RENDER_FEATURE_GAMMA;

	switch (featureIn)
	{
	case RENDERFEATURE_GAMMA:
	{
		featureOut = VD_RENDER_FEATURE_GAMMA;
		break;
	}
	case RENDERFEATURE_BRIGHTNESS:
	{
		featureOut = VD_RENDER_FEATURE_BRIGHTNESS;
		break;
	}
	case RENDERFEATURE_CONTRAST:
	{
		featureOut = VD_RENDER_FEATURE_CONTRAST;
		break;
	}
	case RENDERFEATURE_NOISE:
	{
		featureOut = VD_RENDER_FEATURE_NOISE;
		break;
	}
	case RENDERFEATURE_SHARPNESS:
	{
		featureOut = VD_RENDER_FEATURE_SHARPNESS;
		break;
	}
	case RENDERFEATURE_NONLINSTRETCH:
	{
		featureOut = VD_RENDER_FEATURE_NONLINSTRETCH;
		break;
	}
	case RENDERFEATURE_ROTATION:
	{
		featureOut = VD_RENDER_FEATURE_ROTATION;
		break;
	}
	case RENDERFEATURE_STRETCH:
	{
		featureOut = VD_RENDER_FEATURE_STRETCH;
		break;
	}
	case RENDERFEATURE_ZOOM:
	{
		featureOut = VD_RENDER_FEATURE_ZOOM;
		break;
	}
	case RENDERFEATURE_VERTICAL_SHIFT:
	{
		featureOut = VD_RENDER_FEATURE_VERTICAL_SHIFT;
		break;
	}
	case RENDERFEATURE_PIXEL_RATIO:
	{
		featureOut = VD_RENDER_FEATURE_PIXEL_RATIO;
		break;
	}
	case RENDERFEATURE_POSTPROCESS:
	{
		featureOut = VD_RENDER_FEATURE_POSTPROCESS;
		break;
	}
	default:
		break;
	}

	return featureOut;
}

#endif
