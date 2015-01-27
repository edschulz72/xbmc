#ifdef HAS_VIDONME

#include "utils/StringUtils.h"
#include "utils/log.h"
#include "FileItem.h"
#include "Application.h"
#ifdef HAS_VIDEO_PLAYBACK
#include "cores/VideoRenderers/RenderManager.h"
#endif
#include "windowing/WindowingFactory.h"
#include "cores/AudioEngine/AEFactory.h"
#include "settings/Settings.h"
#include "Util.h"

#if defined(TARGET_ANDROID)
#include "utils/URIUtils.h"
#include "android/activity/XBMCApp.h"
#include "utils/CPUInfo.h"
#else
#include "Utils/URIUtils.h"
#endif

#include "guilib/GraphicContext.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/GUIWindowManager.h"


#include "VDMPlayer.h"


#include "TextureCache.h"

#include "filesystem/File.h"
#include "filesystem/Directory.h"
#include "filesystem/FileFactory.h"
#include "filesystem/DirectoryFactory.h"
#include "URL.h"

#include "PasswordManager.h"

//disable in amlogic demo
//#include "vidonme/VDMUtils.h"
//#include "vidonme/useraccount/UserAccount.h"
//#include "PasswordManager.h"

#if defined(TARGET_ANDROID)
#include "guilib/MatrixGLES.h"
#endif

#include "settings/MediaSettings.h"
#include "settings/DisplaySettings.h"
#include "utils/StringUtils.h"
#include "guilib/Key.h"
#include "utils/StdString.h"
#include "utils/StringUtils.h"
#include "LangInfo.h"
#include "settings/AdvancedSettings.h"

#if defined(HAS_VIDONME) && defined(TARGET_ANDROID)
#include "windowing/egl/WinSystemEGL.h"
#include "windowing/egl/EGLWrapper.h"
#endif

#if defined(TARGET_ANDROID) && defined(HAS_LIBAMCODEC)
#include "utils/AMLUtils.h"
#endif

#include "vidonme/VDMUserInfo.h"
#include "dialogs/GUIDialogBusy.h"
#include "vidonme/DLLVidonUtils.h"
#include "VidonPlayerManager.h"
//////////////////////////////////////////////////////////////////////////


#define AUDIO_ANALOG      0
#define AUDIO_IEC958      1
#define AUDIO_HDMI        2
#define AUDIO_IS_BITSTREAM(x) ((x) == AUDIO_IEC958 || (x) == AUDIO_HDMI)

static CCriticalSection s_renderlock;     
static bool             s_bManualClose = false;     //flag for sure it's closed by user, not auto playing finished            

bool                    CVDMPlayer::s_bPlayerCoreInited = false;
CCriticalSection        CVDMPlayer::s_sourcesMutex;

#if defined(TARGET_ANDROID)
extern CMatrixGLES g_matrices;
#endif

#if defined(TARGET_ANDROID)

GLint on_guishader_callback (int type, int value)
{
  switch (type)
  {
  case 0: //for enable guishader
    g_Windowing.EnableGUIShader ((ESHADERMETHOD)value);
    return 0;

  case 1: //for disable guishader
    g_Windowing.DisableGUIShader();
    return 0;

  case 2: //for posLoc
    return g_Windowing.GUIShaderGetPos();

  case 3: //for colLoc
    return g_Windowing.GUIShaderGetCol();

  case 4: //for tex0Loc
    return g_Windowing.GUIShaderGetCoord0();

  case 5:
    return g_Windowing.GUIShaderGetUniCol ();
  }

  return 0;
}

int on_guisetting_getint_callback(const char *strSetting)
{
  return  CSettings::Get().GetInt(strSetting);
}

#endif //TARGET_ANDROID

namespace VidOnMe
{

  void OnPlaybackEnd( void* owner )
  {
    if (!owner)
      return;

    if (!s_bManualClose)
    {
      CVDMPlayer * coreplayer = (CVDMPlayer *)owner;
      coreplayer->CloseFile(false);

      if (coreplayer->m_bOpenFiled)
        g_windowManager.PreviousWindow();
    }

    g_application.OnPlayBackEnded();
    //CVDMPlayer * coreplayer = (CVDMPlayer *)owner;
    //coreplayer->CloseFile();
  }

  void OnPlaybackStarted(int type, void* owner )
  {
    if (!owner)
      return;

    CVDMPlayer * coreplayer = (CVDMPlayer *)owner;
    coreplayer->m_actionrender = RENDER_TYPE_BEGIN;
    CAfterStartedProcess procsser(coreplayer);
    procsser.StopThread();

    CSingleLock lock(s_renderlock);

    coreplayer->PrepareStream();

    //coreplayer->OnStarted();

    g_application.OnPlayBackStarted();

    //CVDMPlayer * coreplayer = (CVDMPlayer *)owner;
    //coreplayer->m_bAbortRequest = true;
    //g_application.SwitchToFullScreen();
    CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleOn = coreplayer->GetSubtitleVisible();
    CMediaSettings::Get().GetCurrentVideoSettings().m_AudioStream = coreplayer->GetAudioStream();
    CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleStream = coreplayer->GetSubtitle();
    CLog::Log(LOGINFO, "********* callback  ******** m_SubtitleOn = %d *************************", CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleOn);
    CLog::Log(LOGINFO, "********* callback  ******** m_AudioStream = %d *************************", CMediaSettings::Get().GetCurrentVideoSettings().m_AudioStream);
    CLog::Log(LOGINFO, "********* callback  ******** m_SubtitleStream = %d *************************", CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleStream);
  }

  void OnNewVideoFrame( void* owner )
  {
    //g_application.NewFrame();
  }

  void OnPlaybackFailed( void* owner )
  {
    if (!owner)
      return;

    if (!s_bManualClose)
    {
      CVDMPlayer * coreplayer = (CVDMPlayer *)owner;
      coreplayer->CloseFile(false);

      if (coreplayer->m_bOpenFiled)
        g_windowManager.PreviousWindow();
    }

    g_application.OnPlayBackStopped();

    //CVDMPlayer * coreplayer = (CVDMPlayer *)owner;
    //coreplayer->CloseFile();

  }

  void OnQueueNextItem( void* owner )
  {
    g_application.OnQueueNextItem();
  }

  void OnPlayBackSpeedChanged( int iSpeed, void* owner )
  {
    g_application.OnPlayBackSpeedChanged(iSpeed);
  }

  void OnPlayBackResumed( void* owner )
  {
    g_application.OnPlayBackResumed();
  }

  void OnPlayBackPaused( void* owner )
  {
    g_application.OnPlayBackPaused();
  }

  void OnPlayBackSeek( int iTime, int seekOffset, void* owner )
  {
    g_application.OnPlayBackSeek(iTime, seekOffset);
  }


} //VidOnMe


static void OnAudioCallbackInitialize(void* owner, int iChannels, int iSamplesPerSec, int iBitsPerSample)
{
  IAudioCallback* iacallback = (IAudioCallback*)owner;

  if( iacallback )
    iacallback->OnInitialize(iChannels, iSamplesPerSec, iBitsPerSample);
}

static void OnAudioCallbackAudioData(void* owner, const float* pAudioData, int iAudioDataLength)
{
  IAudioCallback* iacallback = (IAudioCallback*)owner;

  if( iacallback )
    iacallback->OnAudioData(pAudioData, iAudioDataLength);
}

static void VDMPlayerLogCallback( int loglevel, const char* pstr, int console_id )
{
  CStdString strLog = StringUtils::Format(" PLAYERCORE:%s", pstr);
  CLog::Log(loglevel, strLog.c_str());
}

CAfterStartedProcess::CAfterStartedProcess(CVDMPlayer * pplayer)
  :CThread("CAfterStartedProcess")
{
  m_pPlayer = pplayer;
  Create();
}

void CAfterStartedProcess::Process()
{
  CLog::Log(LOGINFO, "-------------------------------------CAfterStartedProcess::Process()-------------------Start");
  if (m_pPlayer)
    m_pPlayer->OnStarted();
  CLog::Log(LOGINFO, "-------------------------------------CAfterStartedProcess::Process()-------------------End");
}

using namespace VidOnMe;
//////////////////////////////////////////////////////////////////////////
//class CVDMPlayer 

CVDMPlayer::CVDMPlayer(IPlayerCallback& callback, EPLAYERCORES specifyPlayer)
  : IPlayer(callback),
  CThread("CVDMPlayer"),
  m_ready(true),
  m_specifyPlayer(specifyPlayer)
{
  m_bIsPlaying = false;
  m_paused = false;
  m_playbackStartTime = 0;
  m_speed = 1;
  m_totalTime = 1;
  m_time = 0;

  m_pPlayer = NULL;
  m_playercallback = NULL;
  m_actiontype = ACTION_TYPE_NONE;
  m_actionrender = ACTION_TYPE_NONE;
  m_bOpenFiled = false;
  m_nSpeed = 1;

  //g_DllVidonUtils.Unload();
  m_ICorePlayerAudioCallback.cbowner = NULL;
}

CVDMPlayer::~CVDMPlayer()
{
  if (m_bIsPlaying)
    CloseFile(true);
  else
    StopThread();

  if( m_playercallback )
    delete m_playercallback;
}

bool CVDMPlayer::Initialize(TiXmlElement* pConfig)
{
  return true;
}

void CVDMPlayer::RegisterAudioCallback(IAudioCallback* pCallback)
{
  if( m_pPlayer )
  {
    m_ICorePlayerAudioCallback.cbowner = pCallback;
    m_ICorePlayerAudioCallback.pfn_initialize = OnAudioCallbackInitialize;
    m_ICorePlayerAudioCallback.pfn_audiodata = OnAudioCallbackAudioData;

    m_pPlayer->RegisterAudioCallback(&m_ICorePlayerAudioCallback);
  }
}

void CVDMPlayer::UnRegisterAudioCallback()
{
  if( m_pPlayer )
    m_pPlayer->UnRegisterAudioCallback();
}

bool CVDMPlayer::OpenFile(const CFileItem& file, const CPlayerOptions &options)
{
  if (m_bIsPlaying)
  {
    CloseFile(true);
    StopThread();
  }

  m_ready.Reset();
  m_item = file;
  m_bIsPlaying = true;

  m_mimetype = file.GetMimeType();

  //translate properties to string

  m_strProperties = file.GetPropertiesAsString();

  m_PlayerOptions = options;


#if defined(HAS_VIDEO_PLAYBACK)
  //g_renderManager.PreInit();
#endif

  bool bOpen = false;

  do 
  {
    // Suspend AE temporarily so exclusive or hog-mode sinks 
    // don't block external player's access to audio device  
    CAEFactory::Suspend();

    //wait for AE has completed suspended
    XbmcThreads::EndTime timer(500);
    while (!timer.IsTimePast() && !CAEFactory::IsSuspended())
    {
      Sleep(50);
    }
    if (timer.IsTimePast())
    {
      CLog::Log(LOGERROR,"vdmplayer: audioengine did not suspend before launching vdmplayer");
    }

    if (!s_bPlayerCoreInited)
    {
      InitializePlayerCore();

      if (!s_bPlayerCoreInited)
      {
        CLog::Log(LOGERROR,"vdmplayer: failed to init vdmplayer");
        break;
      }
    }

    CLog::Log(LOGNOTICE, "%s: %s : %lf", __FUNCTION__, m_item.GetPath().c_str(), options.starttime);

    //reset manual close flag
    s_bManualClose = false;

#if defined(TARGET_WINDOWS)

    //only windows need set this

    if (CSettings::Get().GetBool("videoplayer.usedxva2"))
      g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_RefreshHardwareAccelSetting(ENABLE_HARDWARE_ACCELERATE_DXVA);
    else
      g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_RefreshHardwareAccelSetting(DISABLE_HARDWARE_ACCELERATE);

#endif

    Create();

    if(!m_ready.WaitMSec(100))
		{
			CGUIDialogBusy* dialog = (CGUIDialogBusy*)g_windowManager.GetWindow(WINDOW_DIALOG_BUSY);
			if(dialog)
			{
				dialog->Show();
				while(!m_ready.WaitMSec(1))
					g_windowManager.ProcessRenderLoop(false);
				dialog->Close();
			}
    }

    bOpen = true;

  } while (false);

  return bOpen;

}

bool CVDMPlayer::QueueNextFile(const CFileItem& file)
{
  if( m_pPlayer )
  {
    SourceFileItem item;
    memset(&item, 0, sizeof(SourceFileItem));

    item.pszFile = file.GetPath();
    item.pszMimeType = file.GetMimeType();

    SetSourceItem(file, item);

    return m_pPlayer->QueueNextFile(&item);
  }

  return false;
}

void CVDMPlayer::OnNothingToQueueNotify()
{
  if( m_pPlayer )
    m_pPlayer->OnNothingToQueueNotify();
}

void CVDMPlayer::OnStarted()
{
  CLog::Log(LOGINFO, "-------------------------------------CVDMPlayer::OnStarted()-------------------Start");
  if (!m_pPlayer)
    return;

  //if (m_PlayerOptions.starttime > 0)
  //  SeekTime(m_PlayerOptions.starttime * 1000);

  CLog::Log(LOGINFO, "-------------------------------------CVDMPlayer::OnStarted()-------------------End");
}

bool CVDMPlayer::CloseFile()
{
  return CloseFile(true);
}

bool CVDMPlayer::CloseFile(bool bmanual) 
{
  if (!m_bIsPlaying)
    return true;

  if (m_actiontype == ACTION_TYPE_STOP)
    return true;

  {
    CSingleLock lock(s_renderlock);
    s_bManualClose = true;
    m_actiontype = ACTION_TYPE_STOP;
    m_actionrender = RENDER_TYPE_END;
  }

  m_ready.Reset();

  if (bmanual)
    StopThread();

  m_bIsPlaying = false;

  return true;
}

bool CVDMPlayer::IsPlaying() const
{
  CSingleLock lock(s_renderlock);

  if (!m_pPlayer)
    return false;

  return m_pPlayer->IsPlaying();
}

void CVDMPlayer::Pause()
{
  CSingleLock lock(s_renderlock);

  if (!m_pPlayer)
    return;

  if (m_actiontype != ACTION_TYPE_PAUSE && !IsCaching())
    m_actiontype = ACTION_TYPE_PAUSE;
  else
    m_actiontype = ACTION_TYPE_RUNING;

  m_pPlayer->Pause();
}

bool CVDMPlayer::IsPaused() const
{
  CSingleLock lock(s_renderlock);

  if (!m_pPlayer)
    return true;

  return m_pPlayer->IsPaused();
}

bool CVDMPlayer::HasVideo() const
{
  if( m_pPlayer )
    return m_pPlayer->HasVideo();

  return false;
}

bool CVDMPlayer::HasAudio() const
{
  if( m_pPlayer )
    return m_pPlayer->HasAudio();

  return false;
}

void CVDMPlayer::SwitchToNextLanguage()
{

}

void CVDMPlayer::ToggleSubtitles()
{

}

bool CVDMPlayer::CanSeek()
{
  return true;
}

void CVDMPlayer::Seek(bool bPlus, bool bLargeStep, bool bChapterOverride)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return;

  m_pPlayer->Seek(bPlus,bLargeStep);
}

void CVDMPlayer::SeekPercentage(float iPercent)
{
  int64_t iTotalTime = GetTotalTime();

  if (iTotalTime != 0)
  {
    float fpercent = iPercent / (float)100;
    int64_t iTime = (int64_t)((float)iTotalTime * fpercent);
    //CLog::Log(LOGDEBUG, "SeekPercentage is %f", iTime);
    //return iTime * 100 / (float)iTotalTime;
    SeekTime(iTime);
  }
}

float CVDMPlayer::GetPercentage()
{

  int64_t iTime = GetTime();
  int64_t iTotalTime = GetTotalTime();

  if (iTotalTime != 0)
  {
    CLog::Log(LOGDEBUG, "Percentage is %f", (iTime * 100 / (float)iTotalTime));
    return iTime * 100 / (float)iTotalTime;
  }

  return 0.0f;
}

void CVDMPlayer::GetAudioInfo(CStdString& strAudioInfo)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return;

  int nlength = 0;
  m_pPlayer->GetAudioInfo(NULL,nlength);
  if (!nlength)
    return;

  char* sztemp = strAudioInfo.GetBufferSetLength(nlength);
  m_pPlayer->GetAudioInfo(sztemp,nlength);
}

void CVDMPlayer::GetVideoInfo(CStdString& strVideoInfo)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return;

  int nlength = 0;
  m_pPlayer->GetVideoInfo(NULL,nlength);
  if (!nlength)
    return;
  char* sztemp = strVideoInfo.GetBufferSetLength(nlength);
  m_pPlayer->GetVideoInfo(sztemp,nlength);
}

void CVDMPlayer::GetGeneralInfo( CStdString& strVideoInfo)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return;

  int nlength = 0;
  m_pPlayer->GetGeneralInfo(NULL,nlength);
  if (!nlength)
    return;
  char* sztemp = strVideoInfo.GetBufferSetLength(nlength);
  m_pPlayer->GetGeneralInfo(sztemp,nlength);
}

void CVDMPlayer::SwitchToNextAudioLanguage()
{

}

void CVDMPlayer::SetAVDelay(float fValue)
{
  if (!m_pPlayer)
    return ;

  m_pPlayer->SetAVDelay(fValue);	
}

float CVDMPlayer::GetAVDelay()
{
  if (!m_pPlayer)
    return 0;

  return m_pPlayer->GetAVDelay();	
}

void CVDMPlayer::SetSubTitleDelay(float fValue)
{
  if (!m_pPlayer)
    return ;

  m_pPlayer->SetSubTitleDelay(fValue);	
}

float CVDMPlayer::GetSubTitleDelay()
{
  if (!m_pPlayer)
    return 0;

  return m_pPlayer->GetSubTitleDelay();	
}


void CVDMPlayer::SeekTime(int64_t iTime)
{
  CLog::Log(LOGINFO, "-------------------------------------CVDMPlayer::SeekTime(int64_t iTime)-------------------Start");
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return ;

  CLog::Log(LOGNOTICE, "This is Test for SeekTime : %s: %lld", __FUNCTION__, iTime);

  m_pPlayer->SeektoTime(iTime / 1000);
  CLog::Log(LOGINFO, "-------------------------------------CVDMPlayer::SeekTime(int64_t iTime)-------------------End");
}

int64_t CVDMPlayer::GetTime()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return 0;

  return (int64_t)(m_pPlayer->GetCurrentTime() * 1000);
}

int64_t CVDMPlayer::GetTotalTime()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return 0;

  return (int64_t)(m_pPlayer->GetDuration() * 1000);
}

void CVDMPlayer::ToFFRW(int iSpeed)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return;

  m_pPlayer->SetPlaySpeed(iSpeed);
}

void CVDMPlayer::ShowOSD(bool bOnoff)
{

}

CStdString CVDMPlayer::GetPlayerState()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return "";

  int nlength = 0;
  m_pPlayer->GetPlayerState(NULL,nlength);
  if (!nlength)
    return "";

  CStdString sresult;
  char * sztemp = sresult.GetBufferSetLength(nlength);
  m_pPlayer->GetPlayerState(sztemp,nlength);

  return sztemp;
}

bool CVDMPlayer::SetPlayerState(CStdString state)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return false;

  return m_pPlayer->SetPlayerState((char *)state.c_str());
}

void CVDMPlayer::Present()
{
  CSingleLock lock(s_renderlock);

  if (!m_pPlayer)
    return;

  //if (m_actionrender == RENDER_TYPE_BEGIN)
  {
    m_pPlayer->RenderVideoStream();
  }
}
std::string CVDMPlayer::GetISOlanguage(const std::string& strLanguage )
{
  CStdString srcLanguage;
  if(strLanguage == "Chinese (Simple)")
  {
    srcLanguage = "chi";
  }
  else if(strLanguage == "Chinese (Traditional)")
  {
    srcLanguage = "chi";
  }
  else if(strLanguage == "English")
  {
    srcLanguage = "eng";
  }
  else if(strLanguage == "French")
  {
    srcLanguage = "fre";
  }
  else if(strLanguage == "German")
  {
    srcLanguage = "ger";
  }
  else if(strLanguage == "Japanese")
  {
    srcLanguage = "jpn";
  }
  else if(strLanguage == "Korean")
  {
    srcLanguage = "kor";
  }
  else if(strLanguage == "Portuguese (Brazil)")
  {
    srcLanguage = "por";
  }
  else if(strLanguage == "Spanish")
  {
    srcLanguage = "spa";
  }
  else
  {
    srcLanguage = strLanguage;
  }
  return srcLanguage;
}


void CVDMPlayer::Process()
{
  m_playercallback = new ICorePlayerCallback;

  m_playercallback->cbowner = (void *)this;
  m_playercallback->pfn_OnPlayBackEnded = OnPlaybackEnd;
  m_playercallback->pfn_OnPlayBackStopped = OnPlaybackFailed;
  m_playercallback->pfn_OnPlayBackStarted = OnPlaybackStarted;
  m_playercallback->pfn_OnNewVideoFrame = OnNewVideoFrame;
  m_playercallback->pfn_OnQueueNextItem = OnQueueNextItem;
  m_playercallback->pfn_OnPlayBackResumed = OnPlayBackResumed;
  m_playercallback->pfn_OnPlayBackSpeedChanged = OnPlayBackSpeedChanged;
  m_playercallback->pfn_OnPlayBackPaused = OnPlayBackPaused;
  m_playercallback->pfn_OnPlayBackSeek = OnPlayBackSeek;

  m_pPlayer = g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_GetCorePlayer(m_playercallback);

  if (m_pPlayer)
  {
    RESOLUTION res = g_graphicsContext.GetVideoResolution();
    int nx =CDisplaySettings::Get().GetResolutionInfo(res).iWidth;
    int ny =CDisplaySettings::Get().GetResolutionInfo(res).iHeight;

    m_pPlayer->ReSize(nx,ny);
  }

  /*
  {
  CSingleLock lock(s_renderlock);
  m_bOpenSuccessfully = true;
  }
  return;
  */

  SourceFileItem sourceItem;
  memset(&sourceItem, 0, sizeof(SourceFileItem));
  SetSourceItem(m_item, sourceItem);

  m_bOpenFiled = false;

  bool bAudioOnly = false;
  CStdString strFile = m_item.GetPath();

  CLog::Log(LOGDEBUG, "%s: receive playing source path = %s", __FUNCTION__, strFile.c_str());

  IPlayerOptions options;
  sourceItem.pOptions = &options;
  sourceItem.sourceType = PLAYER_SOURCE_TYPE_FILE;

  options.spropertys = m_strProperties.c_str();

#if defined(TARGET_ANDROID)
  //options.displayMode = CMediaSettings::Get().GetCurrentVideoSettings().m_DimensionMode;
  //if(VidOnMe::VDMUtils::GetCPUType() != VidOnMe::CT_ALLWINNER_A20)
  //{
  //  options.displayMode = CSettings::Get().GetInt("vidonme.playmode");
  //}
  options.displayMode = CSettings::Get().GetInt("d3.mode");
  CLog::Log(LOGDEBUG, "%s: options.displayMode = %d", __FUNCTION__, options.displayMode);
#endif

  //no-smb file bluray need do the url decode working. 
  if(StringUtils::EqualsNoCase(StringUtils::Left(strFile, 9).c_str(), "bluray://"))
  {
    //bluray need do the path decode and get no-encode path
    GetSMBUsernamePassword(strFile);
  }
  else
  {
    if (URIUtils::IsSmb(strFile))
      GetSMBUsernamePassword(strFile);
  }

  //network setting
  SetNetworkSetting();

  //playercore config
  PlayercoreConfig config;
  memset(&config, 0, sizeof(PlayercoreConfig));

  if( EPC_PAPLAYER == m_specifyPlayer )
  {
    CLog::Log(LOGDEBUG, "%s: specify using EPC_PAPLAYER", __FUNCTION__);
    sourceItem.playerType = PLAYER_TYPE_AUDIO;
    bAudioOnly = true;
  }
  else
  {
    sourceItem.playerType = PLAYER_TYPE_VIDEO;
    SetVideoPlayingOptions(strFile, sourceItem.sourceType, options, config);
  }

  //decode path add by yong.li 
  if( IsVirtualFile(strFile) )
    strFile = DecodePathFromVirtualToReal(strFile);

  //set playing source path, this path has been processed
  sourceItem.pszFile = strFile.c_str();

  sourceItem.pszMimeType = m_mimetype.c_str();

  //set start time, and pass to player, let player doing the seek
  options.starttime = m_PlayerOptions.starttime;

  SetAudioOutputOptions();

  //sourceItem.bMainTitle = m_PlayerOptions.bPlayMainTitle;
  CLog::Log(LOGDEBUG, "vdmplayer :sourceItem.bMainTitle %d ", sourceItem.bMainTitle);

  m_bOpenFiled = m_pPlayer->OpenFile(&sourceItem);

  if (m_bOpenFiled)
  {
    if(m_ICorePlayerAudioCallback.cbowner != NULL && PLAYER_TYPE_AUDIO == sourceItem.playerType)
        m_pPlayer->RegisterAudioCallback(&m_ICorePlayerAudioCallback);

    float fbrightness = CMediaSettings::Get().GetCurrentVideoSettings().m_Brightness;
    float fcontrast = CMediaSettings::Get().GetCurrentVideoSettings().m_Contrast;

    m_ready.Set();

    g_application.SwitchToFullScreen();

    //g_Windowing.SetEventCallback(this);

    if (m_actiontype == ACTION_TYPE_NONE)
    {
      m_actiontype = ACTION_TYPE_RUNING;
    }

    for(;;)
    {
      {
        CSingleLock lock(s_renderlock);
        if( m_actiontype == ACTION_TYPE_STOP )
        {
          m_ready.Set();
          break;
        }
      }

      if (fbrightness != CMediaSettings::Get().GetCurrentVideoSettings().m_Brightness)
      {
        m_pPlayer->SetDisplayBrightness(CMediaSettings::Get().GetCurrentVideoSettings().m_Brightness);
        fbrightness = CMediaSettings::Get().GetCurrentVideoSettings().m_Brightness;
      }

      if (fcontrast != CMediaSettings::Get().GetCurrentVideoSettings().m_Contrast)
      {
        m_pPlayer->SetDisplayContrast(CMediaSettings::Get().GetCurrentVideoSettings().m_Contrast);
        fcontrast = CMediaSettings::Get().GetCurrentVideoSettings().m_Contrast;
      }

      Sleep(1);
    }
  }
  else
  {
    m_ready.Set();
    m_actiontype = ACTION_TYPE_OPENFAILED;
  }

  if (m_pPlayer)
    m_pPlayer->CloseFile();

  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_ReleaseCorePlayer(m_pPlayer);

  //need force to release playsource, and relese the resource
  RemovePlaySource(strFile);

  {
    CSingleLock lock(s_renderlock);
    m_pPlayer = NULL;
  }

  /* Resume AE processing of XBMC native audio */
  if (!CAEFactory::Resume())
  {
    CLog::Log(LOGFATAL, "vdmplayer: failed to restart audioengine after return from vdmplayer");
  }

  m_actiontype = ACTION_TYPE_OVER;

  //g_Windowing.SetEventCallback(NULL);
}

void CVDMPlayer::SetVideoPlayingOptions(CStdString& strFile, int& type, IPlayerOptions& options, PlayercoreConfig& config)
{
  //video playing setting

  CStdString audiolanguage = CSettings::Get().GetString("locale.audiolanguage");
  CStdString subtitlelanguage = CSettings::Get().GetString("locale.subtitlelanguage");
  if(audiolanguage.Equals("default"))
  {
    audiolanguage = CSettings::Get().GetString("locale.language");
  }
  if(subtitlelanguage.Equals("default"))
  {
    subtitlelanguage = CSettings::Get().GetString("locale.language");
  }

  CLog::Log(LOGINFO, "***************** audiolanguage = %s *************************", audiolanguage.c_str());
  CLog::Log(LOGINFO, "***************** subtitlelanguage = %s *************************", subtitlelanguage.c_str());

  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingString( CS_DEFAULT_AUDIO_LANGUAGE, GetISOlanguage(audiolanguage).c_str() );
  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingString( CS_DEFAULT_SUBTITLE_LANGUAGE, GetISOlanguage(subtitlelanguage).c_str() );

  CStdString custompath = CSettings::Get().GetString("subtitles.custompath");
  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingString( CS_SUBTITLE_SEARCH_PATH, CSettings::Get().GetString("subtitles.custompath").c_str());

  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingBool(BA_DVD_AUTOMENU, CSettings::Get().GetBool("dvds.automenu"));

  CLog::Log(LOGINFO, "***************** custompath = %s *************************", custompath.c_str());
  CLog::Log(LOGINFO, "***************** dvds.automenu = %d *************************", CSettings::Get().GetBool("dvds.automenu"));

  CLog::Log(LOGDEBUG, "%s: receive playing source path = %s", __FUNCTION__, strFile.c_str());

  //TODO, need test connect remote server
  //input path style example
  //VDM-mode:
  // 
  // bluray/folder/menu:
  // bluray/folder/playlist:
  //  
  // bluray/iso/menu:
  // bluray/iso/playlist:
  // 
  //XBMC-mode:
  // bluray/folder/menu:      bluray://Y%3a%5csever_source%5cbluray%5cMission%20Impossible%20-%20Ghost%20Protocol%202011%201080p%20Blu-ray%20AVC%20TrueHD%207.1-DIY-HDChina%5c/BDMV/MovieObject.bdmv
  // bluray/folder/playlist:  Y:\sever_source\bluray\Mission Impossible - Ghost Protocol 2011 1080p Blu-ray AVC TrueHD 7.1-DIY-HDChina\/BDMV/PLAYLIST/00800.mpls
  //  
  // bluray/iso/menu:      bluray://udf%3a%2f%2fY%253a%255csever_source%255cbluray%255c2012.iso%2f/BDMV/MovieObject.bdmv
  // bluray/iso/playlist:  Y:\sever_source\bluray\2012.iso/BDMV/PLAYLIST/00001.mpls
  //  
  // 


  //in xbmc-mode, need extract the bluray:// udf:// first 

  if(StringUtils::EqualsNoCase(StringUtils::Left(strFile, 9).c_str(), "bluray://"))
  {
    //bluray, not need mimetype
    m_mimetype = "";

    strFile = strFile.substr(9,strFile.length() - 9);

    if(StringUtils::EqualsNoCase(StringUtils::Left(strFile, 6).c_str(), "udf://"))
    {
      strFile = strFile.substr(6,strFile.length() - 6);
    }
    else
    {
      //not udf wrap, must be folder
      type = PLAYER_SOURCE_TYPE_FOLDER;
    }
  }

  //parser the path, make sure it's playlist / menu mode 
  //parser the source path, make sure it's iso or fake-folder iso

  //first check it's title mode

  CStdString sfileext = URIUtils::GetExtension(strFile);

  if( sfileext.Equals(".title") )
  {
    CURL::Decode(strFile);

    CStdString strFileName, strPath;

    //directly to get title.name 
    strFileName = StringUtils::Right(strFile, 11);						//00000.title
    strPath = StringUtils::Left(strFile, strFile.size() - 12);	// /00000.title = 12

    StringUtils::ToLower(strFileName);

    int playlist = -1;
    sscanf(strFileName.c_str(), "%05d.title", &playlist);

    options.playlist = playlist;

    URIUtils::RemoveSlashAtEnd(strPath);
    strFile = strPath;
  }
  else 
  {
    //old working logic 

    if( sfileext != ".iso" )
    {
      CStdString strExtension, strFileName, strPath;

      strExtension = URIUtils::GetExtension(strFile);
      StringUtils::ToLower(strExtension);

      //extract the playlist 
      strFileName = strFile;
      URIUtils::Split(strFile, strPath, strFileName);

      if( ".mpls" == strExtension )
      {
        //bluray, not need mimetype
        m_mimetype = "";

        StringUtils::ToLower(strFileName);

        int playlist = -1;
        sscanf(strFileName.c_str(), "%07d.mpls", &playlist);

        options.playlist = playlist;

        URIUtils::GetParentPath(strPath, strFile);		//remove PLAYLIST
        URIUtils::GetParentPath(strFile, strPath);		//remove BDMV
        URIUtils::RemoveSlashAtEnd(strPath);
        strFile = strPath;

        //the BDMV/PLAYLIST/BDMV has been removed, I need check it's file or folder
        strExtension = URIUtils::GetExtension(strFile);

        if( strExtension.Equals(".iso") )
          type = PLAYER_SOURCE_TYPE_FILE;
        else
          type = PLAYER_SOURCE_TYPE_FOLDER;
      }

      if( strFileName.Equals("MovieObject.bdmv") || strFileName.Equals("index.bdmv") )
      {
        //no specify the playlist, menu mode or longest 
        URIUtils::GetParentPath(strPath, strFile);		//remove BDMV

        while(URIUtils::HasSlashAtEnd(strFile))
            URIUtils::RemoveSlashAtEnd(strFile);

        //the BDMV/PLAYLIST/BDMV has been removed, I need check it's file or folder
        strExtension = URIUtils::GetExtension(strFile);

        if( strExtension.Equals(".iso") )
          type = PLAYER_SOURCE_TYPE_FILE;
        else
          type = PLAYER_SOURCE_TYPE_FOLDER;

        m_mimetype = "";
      }
    }
  }

  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetBDRegion(1);
  CStdString strCharset = g_langInfo.GetSubtitleCharSet();
  memcpy(config.charset, strCharset, strCharset.size());

  unsigned int colormap[8] = { 0xFFFFFF00, 0xFFFFFFFF, 0xFF0099FF, 0xFF00FF00, 0xFFCCFF00, 0xFF00FFFF, 0xFFE5E5E5, 0xFFC0C0C0 };
  config.fontcolor    = colormap[ CSettings::Get().GetInt("subtitles.color")];
  config.fontstyle    = CSettings::Get().GetInt("subtitles.style");
  config.fontsize     = CSettings::Get().GetInt("subtitles.height");
  config.bOverrideASS = CSettings::Get().GetBool("subtitles.overrideassfonts");
  config.bfontborder  = true;
  config.BorderColor  = 0xFF000000;
  config.fontposition = CSettings::Get().GetInt("subtitles.align");
  
  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_RefreshFontSetting(&config);

  //yong.li 20140829
  //Update the subtitle by user setting
  int fontsize = CSettings::Get().GetInt("subtitles.height")/2 - 8;
  int fontstyle = CSettings::Get().GetInt("subtitles.style");
  int fontcolor = CSettings::Get().GetInt("subtitles.color");
  int fontalign = CSettings::Get().GetInt("subtitles.align");
  CLog::Log(LOGINFO, "Subtitle Setting fontsize :%d, fontstyle :%d, fontcolor :%d, fontalign :%d", fontsize, fontstyle, fontcolor, fontalign);
  
  //xbmc neednot ,vmc need
  //UpdateSubtitleSetting(fontsize, fontstyle, fontcolor, fontalign);

  options.restoreAudioId = CMediaSettings::Get().GetCurrentVideoSettings().m_AudioStream;
  options.restoreSubtitleId = CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleStream;
#if 0
  if(CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitlePath != "")
  {
    strcpy(options.subtitlepath, CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitlePath);
  }
  else
#endif 
  {
    memset(options.subtitlepath,NULL,sizeof(options.subtitlepath));
  }

  CLog::Log(LOGINFO, "*****************options.restoreAudioId = %d *************************", options.restoreAudioId );
  CLog::Log(LOGINFO, "*****************options.restoreSubtitleId = %d *************************", options.restoreSubtitleId );
 //CLog::Log(LOGINFO, "*****************CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitlePath = %s *************************", CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitlePath.c_str() );
  CLog::Log(LOGINFO, "*****************options.subtitlepath = %s *************************", options.subtitlepath );
}

void CVDMPlayer::SetAudioOutputOptions()
{
  //m_pPlayer->SetDisplayRatio(VIEW_MODE_STRETCH_4x3);
  CStdString currentPassthroughDevice =  CSettings::Get().GetString("audiooutput.passthroughdevice");
  CStdString currentDevice = CSettings::Get().GetString("audiooutput.audiodevice");
  int aomode =  CSettings::Get().GetInt("audiooutput.mode");
  int nchannel = 1;
  AudioSetting * paudiosettings = g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_GetAudioSettings();
#if defined(TARGET_ANDROID)
  bool passthrough = CSettings::Get().GetBool("audiooutput.passthrough");
  if (!passthrough)
      aomode = AUDIO_ANALOG;
  else
  {
      if(strcmp(currentPassthroughDevice.c_str(),"AUDIOTRACK:HDMI") == 0)
	aomode = AUDIO_HDMI;
      else if(strcmp(currentPassthroughDevice.c_str(),"AUDIOTRACK:SPDIF") == 0) 
	aomode = AUDIO_IEC958;
      else
	CLog::Log(LOGERROR, "wrong passthrough device");
  }
#endif
  if (aomode == AUDIO_ANALOG)
  {
#if defined(TARGET_ANDROID)
    g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioDefaultOutputDevice("audiotrack",DEVICE_TYPE_COMMON);
#else
    //g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioDefaultOutputDevice(currentDevice.c_str(),DEVICE_TYPE_COMMON);
    int ncount = paudiosettings->deviceCounts;
    bool bfind =false;
    for (int i=0; i<ncount; i++)
    {
      if (!strcmp(paudiosettings->pDevices[i]->pszDeviceName, currentDevice.c_str()))
      {

        if (paudiosettings->pDevices[i]->outType == AUDIO_OUT_TYPE_ANALOG)
        {
          g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioOutputDevice(i);
          bfind = true;
          break;
        }
      }
    }

    if (!bfind)
    {
      g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioDefaultOutputDevice(currentDevice.c_str(),DEVICE_TYPE_COMMON);
    }
#endif
  }
  else 
  {
    int ncount = paudiosettings->deviceCounts;
    bool bfind =false;
    int type = 0;

    if (aomode == AUDIO_IEC958)
      type = AUDIO_OUT_TYPE_DIGITAL;
    else if (aomode == AUDIO_HDMI)
      type = AUDIO_OUT_TYPE_DISPLAYPORT;

#if defined(TARGET_VIDONME_BOX)
    if (aomode == AUDIO_IEC958)
      g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioDefaultOutputDevice("aw-spdif",DEVICE_TYPE_PASSTHROUGH);
    else if (aomode == AUDIO_HDMI)
      g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioDefaultOutputDevice("aw-hdmi",DEVICE_TYPE_PASSTHROUGH);

    g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioHdmiPassthroughType(0);
#else
    for (int i=0; i<ncount; i++)
    {
      if (!strcmp(paudiosettings->pDevices[i]->pszDeviceName, currentPassthroughDevice.c_str()))
      {

        if (paudiosettings->pDevices[i]->outType == type)
        {
          g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioOutputDevice(i);
          bfind = true;
          break;
        }
      }
    }

    if (!bfind)
    {
      g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioDefaultOutputDevice(currentPassthroughDevice.c_str(),DEVICE_TYPE_PASSTHROUGH);
    }
#endif
    int supportCodec = 0;
    if (CSettings::Get().GetBool("audiooutput.ac3passthrough"))
      supportCodec |= PASSTHROUGH_CODEC_AC3;
    if (CSettings::Get().GetBool("audiooutput.dtspassthrough"))
      supportCodec |= PASSTHROUGH_CODEC_DTS;
    if (CSettings::Get().GetBool("audiooutput.passthroughaac"))
      supportCodec |= PASSTHROUGH_CODEC_AAC;
    if (CSettings::Get().GetBool("audiooutput.multichannellpcm"))
      supportCodec |= PASSTHROUGH_CODEC_LPCM;
    if (CSettings::Get().GetBool("audiooutput.truehdpassthrough"))
      supportCodec |= PASSTHROUGH_CODEC_TRUEHD;
    if (CSettings::Get().GetBool("audiooutput.dtshdpassthrough"))
      supportCodec |= PASSTHROUGH_CODEC_DTSHD;

    g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioPassthroughCodec(supportCodec);
  }

  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetAudioOutputSpeakers(nchannel);
}

void CVDMPlayer::SetSourceItem(const CFileItem& fileItem, SourceFileItem& sourceItem)
{
  //set file item other attribute to source item, to make sure not losing other information

  sourceItem.startOffset = fileItem.m_lStartOffset;
  sourceItem.endOffset = fileItem.m_lEndOffset;
  sourceItem.startPartNumber = fileItem.m_lStartPartNumber;
}

void CVDMPlayer::OnReSize(int newWidth, int newHeight, int newLeft, int newTop)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return;

  m_pPlayer->ReSize(newWidth, newHeight);
}

bool CVDMPlayer::OnAction(const CAction &action)
{
  if (!m_pPlayer)
    return false;

  bool bret = false;

  switch (action.GetID())
  {
  case ACTION_STEP_FORWARD:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_MOVE_RIGHT);
    }
    break;
  case ACTION_STEP_BACK:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_MOVE_LEFT);
    }
    break;
  case ACTION_BIG_STEP_FORWARD:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_MOVE_UP);
    }
    break;
  case ACTION_BIG_STEP_BACK:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_MOVE_DOWN);
    }
    break;
  case ACTION_SHOW_OSD:
    {
      if (action.GetButtonCode() == 0xF00D)
        bret =m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_ENTER);
    }
    break;
  case ACTION_MOUSE_MOVE:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_MOUSE_MOVE,action.GetAmount(),action.GetAmount(1));
    }
    break;
  case ACTION_MOUSE_LEFT_CLICK:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_MOUSE_CLICK,action.GetAmount(),action.GetAmount(1));
    }
    break;
  case ACTION_NEXT_ITEM:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_NEXT_ITEM);
    }
    break;
  case ACTION_PREV_ITEM:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_PREV_ITEM);
    }
    break;
  case ACTION_SHOW_VIDEOMENU:
    {
      bret = m_pPlayer->OnAction(CCorePlayer::PLAYERCORE_ACTION_SHOWVIDEOMENU,action.GetAmount());
    }
    break;
  default:
    break;
  }
  return bret;
}

int  CVDMPlayer::GetChapterCount()
{
  //CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return -1;

  return m_pPlayer->GetChapterCount();
}

int  CVDMPlayer::GetChapter()
{
  if (!m_pPlayer)
    return -1;

  return m_pPlayer->GetChapter();
}

void CVDMPlayer::GetChapterName(CStdString& strChapterName)
{
  if (!m_pPlayer)
    return;

  char szchapter[255] = {'\0'};
  m_pPlayer->GetChapterName(szchapter);

  strChapterName = szchapter;
}

int  CVDMPlayer::SeekChapter(int iChapter)
{
  if (!m_pPlayer)
    return -1;

  return m_pPlayer->SeekChapter(iChapter);
}

int CVDMPlayer::GetSubtitleCount()
{
  if (!m_pPlayer)
    return -1;

  return m_pPlayer->GetSubtitleCount();
}

int CVDMPlayer::GetSubtitle()
{
  if (!m_pPlayer)
    return -1;

  return m_pPlayer->GetSubtitleCurrentStream();
}

void CVDMPlayer::GetSubtitleName(int iStream, CStdString &strStreamName)
{
  if (!m_pPlayer)
    return ;

  char szsubname[255] = {'\0'};

  m_pPlayer->GetSubtitleName(iStream, szsubname);
  strStreamName = szsubname;
}

void CVDMPlayer::GetSubtitleLanguage(int iStream, CStdString &strStreamLang)
{

}

void CVDMPlayer::SetSubtitle(int iStream)
{
  if (!m_pPlayer)
    return ;

  CLog::Log(LOGINFO, "*****************SetSubtitle's iStream = %d *************************", iStream );
  m_pPlayer->SelectSubtitle(iStream);
}

bool CVDMPlayer::GetSubtitleVisible()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return false;

  return m_pPlayer->GetSubtitleVisible();
}

void CVDMPlayer::SetSubtitleVisible(bool bVisible)
{
  if (!m_pPlayer)
    return ;

  m_pPlayer->SetSubtitleVisible(bVisible);
}

void CVDMPlayer::SetDisplayBrightness( float fBrightness )
{
  if (!m_pPlayer)
    return ;

  m_pPlayer->SetDisplayBrightness(fBrightness);
}

void CVDMPlayer::SetDisplayRatio( int nstyle )
{
  if (!m_pPlayer)
    return ;

  m_pPlayer->SetDisplayRatio(nstyle);
}

int CVDMPlayer::GetAudioStreamCount()
{
  if (!m_pPlayer)
    return 0;

  return m_pPlayer->GetAudioStreamCount();
}

int CVDMPlayer::GetAudioStream()
{
  if (!m_pPlayer)
    return -1;

  return m_pPlayer->GetAudioCurrentStream();
}

void CVDMPlayer::GetAudioStreamName(int iStream, CStdString &strStreamName)
{
  if (!m_pPlayer)
    return ;

  char szAudioName[255] = {'\0'};
  m_pPlayer->GetAudioStreamName(iStream, szAudioName);
  strStreamName = szAudioName; 
}

void CVDMPlayer::SetAudioStream(int iStream)
{
  if (!m_pPlayer)
    return ;

  m_pPlayer->SetAudioStream(iStream);
}

void CVDMPlayer::GetAudioStreamLanguage(int iStream, CStdString &strLanguage)
{
  if (!m_pPlayer)
    return ;

  /*
  //TODO
  char szAudioLanguage[255] = {'\0'};
  m_pPlayer->GetAudioStreamLanguage(iStream, szAudioLanguage);
  strLanguage = szAudioLanguage; 
  */
}

void CVDMPlayer::SetVolume(float nVolume)
{
  if (!m_pPlayer)
    return ;

  m_pPlayer->SetAudioVolume(nVolume,false);
}

void CVDMPlayer::GetVideoStreamInfo(SPlayerVideoStreamInfo &info)
{
  if (!m_pPlayer)
    return ;

  info.language.reserve(255);
  info.name.reserve(255);
  info.stereoMode.reserve(255);
  info.videoCodecName.reserve(255);
  m_pPlayer->GetVideoStreamInfo(info,255);
}
void CVDMPlayer::GetAudioStreamInfo(int index, SPlayerAudioStreamInfo &info)
{
  if (!m_pPlayer)
    return ;

  info.audioCodecName.reserve(255);
  info.language.reserve(255);
  info.name.reserve(255);
  m_pPlayer->GetAudioStreamInfo(index, info,255);
}
void CVDMPlayer::GetSubtitleStreamInfo(int index, SPlayerSubtitleStreamInfo &info)
{
  if (!m_pPlayer)
    return ;

  info.language.reserve(255);
  info.name.reserve(255);
  m_pPlayer->GetSubtitleStreamInfo(index,info,255);
}

void CVDMPlayer::SetMute(bool bOnOff)
{
  if (!m_pPlayer)
    return ;

  m_pPlayer->SetMute(bOnOff);
}

bool CVDMPlayer::HasMenu()
{
  if (!m_pPlayer)
    return false;

  return m_pPlayer->IsInMenu();
}

bool CVDMPlayer::IsInMenu() const
{
  if (!m_pPlayer)
    return false;

  return m_pPlayer->IsInMenu();
}

void CVDMPlayer::SetDynamicRangeCompression(long drc)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return;

  m_pPlayer->SetVolumeAmplification(drc);
}

bool CVDMPlayer::SetInputDimensionType (int type)
{
  if (!m_pPlayer)
    return false;

  return m_pPlayer->SetInputDimensionType (type);
}

bool CVDMPlayer::SetOutputDimensionType (int type)
{
  if (!m_pPlayer)
    return false;

  return m_pPlayer->SetOutputDimensionType (type);
}

void on_gui_prompt_callback(auth_status status)
{

}

void CVDMPlayer::InitializePlayerCore()
{
  if( s_bPlayerCoreInited )
    return;

  VidonPlayerConfig vidonconfig;

#if defined(TARGET_ANDROID)
  if(strncmp(g_cpuInfo.getCPUHardware().c_str(),"Amlogic",7) == 0)
  {
    vidonconfig.nDeviceType = VIDONPLAYER_DEVICE_AMLOGIC;
  }
  else if(strncmp(g_cpuInfo.getCPUHardware().c_str(),"sun6i",5) == 0)
  {
    vidonconfig.nDeviceType = VIDONPLAYER_DEVICE_A31;
  }

  vidonconfig.pNativeActivity = CXBMCApp::GetCurrentActivity();
  vidonconfig.pNativeWindows = (ANativeWindow *)(*(CXBMCApp::GetNativeWindow (30000)));
  vidonconfig.pRenderManager = &g_renderManager;
  vidonconfig.pMatrices = &g_matrices;
#endif

  //basic init, just for can using playercore api

  PlayercoreConfig config;
  vidonconfig.pPlayercoreconfig = &config;
  memset(&config, 0, sizeof(PlayercoreConfig));

  config.working_mode = PLAYERCORE_MODE_PLUGIN_PLAYER;

#if defined(HAS_VIDONME) && defined(TARGET_ANDROID)
  config.pfn_guishader_callback = on_guishader_callback;
  //CXBMCApp::GetScreenDimension (config.width, config.height);
  //CJG, 20141020,need to fix this
  if(g_Windowing.GetEglHandle())
  {
    RESOLUTION_INFO res;
    g_Windowing.GetEglHandle()->GetNativeResolution(&res);
    CLog::Log (LOGDEBUG, "### res.iScreen :%d ##", res.iScreen); 
    CLog::Log (LOGDEBUG, "### res.iWidth :%d ##", res.iWidth); 
    CLog::Log (LOGDEBUG, "### res.iHeight :%d ##", res.iHeight); 
    CLog::Log (LOGDEBUG, "### res.iScreenWidth :%d ##", res.iScreenWidth); 
    CLog::Log (LOGDEBUG, "### res.iScreenHeight :%d ##", res.iScreenHeight); 
    config.width = res.iScreenWidth;
    config.height = res.iScreenHeight;
  }
  else
  { 
    config.width = 1280;//1280;//1920;
    config.height = 720;
  }
  CLog::Log (LOGDEBUG, "### screen width:%d / %d ##", config.width, config.height);
  CLog::Log(LOGDEBUG, "regidit pfn_guisetting_getint_callback");
  config.pfn_guisetting_getint_callback = on_guisetting_getint_callback;
#endif
  config.pfn_gui_prompt_callback = on_gui_prompt_callback;

  //playercore will be put into system\player\dvdplayer\ 
  //get xbmc-bin correct folder

  CStdString strExePath = CUtil::ResolveExecutablePath();
  CStdString strHomePath;

  CStdString basePath = CSpecialProtocol::TranslatePath("special://xbmcbin/");
  if (!basePath.empty())
    strHomePath = basePath;

  //URIUtils::GetParentPath(strExePath, strHomePath);

  strcpy(config.pszHomePath, strHomePath.c_str());

  //user folder path is the log output folder

#if defined(TARGET_WINDOWS)
  strcpy(config.pszUserFolderPath, strHomePath.c_str());
#else
  //TARGET_ANDROID
  strcpy(config.pszUserFolderPath, CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder).c_str());
#endif 

  strcpy(config.pszTempFolderPath, CSpecialProtocol::TranslatePath("special://temp/").c_str());

  CLog::Log(LOGDEBUG, "##TEMP: %s ###", strHomePath.c_str());

#if defined(TARGET_WINDOWS)
  config.outerWindowHandle = g_Windowing.GetHwnd();

  RECT rc;
  ::GetClientRect(g_Windowing.GetHwnd() , &rc);
  config.width = rc.right - rc.left;
  config.height = rc.bottom - rc.top;

  config.d3d9 = g_Windowing.GetD3D();
  config.d3d9device = g_Windowing.Get3DDevice();
#endif

  //log settings

  //set log callback function, current log all using xbmc log system to output
#if defined(TARGET_WINDOWS) && defined(_DEBUG)
  //let playercore output log itself, in windows, output to console
  config.pfn_log_callback = NULL;
  config.bEnableConsoleLogger = true;
#endif

#if defined(_DEBUG)
  config.log_app = PLAYERCORE_LOG_APP_NOLIMITED;
  config.log_level = PLAYERCORE_LOG_LEVEL_DEBUG;
  config.bEnableConsoleLogger = true;
#else
  //specify the which app to call playercore
  config.log_app = PLAYERCORE_LOG_APP_VIDONME;
  config.log_level = PLAYERCORE_LOG_LEVEL_RELEASE;
  config.bEnableConsoleLogger = false;
#endif 

  config.log_mode = PLAYERCORE_LOG_MODE_INTERNAL_LIMITED_SIZE;
  config.log_filesize_mb = 1;
  strcpy(config.pszLogFilename, "vdmplayer");

  //external filesystem support, support playecore to playing smb / upnp file
  LoadExternalFileSystem(&config);

  strcpy(config.pszAppName, GetAppName());
  strcpy(config.pszAppVersion, GetAppVersion());
  strcpy(config.pszPackageName, GetAppPackageName());

  
  CStdString strUserName;
  CStdString strPassword;
  CStdString strEmail = CSettings::Get().GetString( "usercenter.email");
  if( CVDMUserInfo::Instance().GetUsernameAndPassword( strUserName, strPassword ) )
  {
    //TODO, set using international accout or other
    config.bInternationalAccount = CVDMUserInfo::Instance().GetCountryCode() != libVidonUtils::CountryCode_China;
    strcpy(config.pszUserName, strUserName.c_str());
    strcpy(config.pszUserPassword, strPassword.c_str());
    strcpy(config.pszUserEmail, strEmail.c_str());
  }
  else
  {
    strcpy(config.pszUserName, "");
    strcpy(config.pszUserPassword, "");
    strcpy(config.pszUserEmail, "");
  }

  s_bPlayerCoreInited = g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_Init(&vidonconfig);

}

void CVDMPlayer::UninitializePlayerCore()
{
  if (s_bPlayerCoreInited)
  {
    CleanPlaySource();
    g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_DeInit();
    s_bPlayerCoreInited = false;
  }

  if (g_DllVidonUtils.IsLoaded())
  {
    // Do not Unload is for UnableExitError while playing.
    //g_DllVidonUtils.Unload();
  }
}

void CVDMPlayer::SetSMBPassword(const std::string& strPassword)
{
  if( !s_bPlayerCoreInited )
    return ;
  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingString( CS_SAMBA_PASSWORD, strPassword.c_str() );
}
void CVDMPlayer::SetSMBUserName(const std::string& strUserName)
{
  if( !s_bPlayerCoreInited )
    return ;
  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingString( CS_SAMBA_USERNAME, strUserName.c_str() );
}

bool CVDMPlayer::AddPlaySource(const std::string& strFilePath)
{
  CSingleLock lock(s_sourcesMutex);

  if( !s_bPlayerCoreInited )
    return false;

  //default set as file

  int type = PLAYER_SOURCE_TYPE_FILE;

  if (XFILE::CDirectory::Exists(strFilePath))
  {
    type = PLAYER_SOURCE_TYPE_FOLDER;
  }

  return g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_AddPlaySource(strFilePath.c_str(), type);
}

void CVDMPlayer::RemovePlaySource(const std::string& strFilePath)
{
  CSingleLock lock(s_sourcesMutex);

  if( !s_bPlayerCoreInited )
    return;

  int type = PLAYER_SOURCE_TYPE_FILE;

  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_RemovePlaySource(strFilePath.c_str(), type);
}

void CVDMPlayer::CleanPlaySource()
{
  CSingleLock lock(s_sourcesMutex);

  if( !s_bPlayerCoreInited )
    return;

  g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_RemovePlaySourceAll();
}

int CVDMPlayer::GetPlaySourcePlaylistCount(const std::string& strFilePath)
{
  CSingleLock lock(s_sourcesMutex);

  if( !s_bPlayerCoreInited )
    return 0;

  return g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_GetPlaySourcePlaylistCount(strFilePath.c_str());
}

CVDMPlayer::Playlists CVDMPlayer::GetPlaySourcePlaylists(const std::string& strFilePath)
{
  CSingleLock lock(s_sourcesMutex);

  CVDMPlayer::Playlists playlists;

  if( !s_bPlayerCoreInited )
    return playlists;

  CStdString filePath = strFilePath;
  int nPlaylistCount = GetPlaySourcePlaylistCount(strFilePath);

  if( nPlaylistCount > 0 )
  {
    bool bMainMovieSet = false;
    int nMaxDurationIndex = 0;

    for (int i = 0; i < nPlaylistCount; i++)
    {
      CVDMPlayer::PlaylistPtr playlist = GetPlaySourcePlaylistByIndex(strFilePath, i);
      if (playlist)
      {
        playlists.push_back(playlist);
        if (playlist->bMainMovie)
        {
          bMainMovieSet = true;
        }
        else
        {
          if (playlist->nDurationMs > playlists[nMaxDurationIndex]->nDurationMs)
          {
            nMaxDurationIndex = playlists.size() - 1;
          }
        }
      }
    }

    if ((!bMainMovieSet) && (playlists.size() > 0))
    {
      playlists[nMaxDurationIndex]->bMainMovie = true;
    }
  }

  return playlists;
}

static CVDMPlayer::PlaylistPtr ConvertPlaylistObject( const char* pszFilePath, XSource::CPlaylist* pRawPlaylist )
{
  CVDMPlayer::PlaylistPtr playlist = CVDMPlayer::PlaylistPtr(new CVDMPlayer::Playlist());

  playlist->nDurationMs = pRawPlaylist->m_durationMs;
  playlist->nPlaylist = pRawPlaylist->m_Playlist;
  playlist->nIndex = pRawPlaylist->m_index;
  playlist->nAngles = pRawPlaylist->m_angles;

  playlist->b3D = pRawPlaylist->m_b3D;
  playlist->bMainMovie = pRawPlaylist->m_bMainMovie;
  playlist->bMenu = pRawPlaylist->m_bMenu;

  playlist->nChapterCount = pRawPlaylist->m_chapterCount;

  CStdString strPlayPath = pszFilePath;

  if (!URIUtils::HasSlashAtEnd(strPlayPath))
  {
    URIUtils::AddSlashAtEnd(strPlayPath);
  }

  strPlayPath = StringUtils::Format("%s%05d.title", strPlayPath.c_str(), playlist->nPlaylist);

  playlist->strPlayPath = strPlayPath;

  return playlist;
}

CVDMPlayer::PlaylistPtr CVDMPlayer::GetPlaySourcePlaylistByIndex(const std::string& strFilePath, int index)
{
  CSingleLock lock(s_sourcesMutex);

  CVDMPlayer::PlaylistPtr playlist;

  if( !s_bPlayerCoreInited )
    return playlist;

  XSource::CPlaylist* pRawPlaylist = g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_GetPlaySourcePlaylistByIndex(strFilePath.c_str(), index);

  if (pRawPlaylist && pRawPlaylist->m_durationMs >= 60 * 1000 )
    playlist = ConvertPlaylistObject(strFilePath.c_str(), pRawPlaylist);

  return playlist;
}

CVDMPlayer::PlaylistPtr CVDMPlayer::GetPlaySourceMainMoviePlaylist(const std::string& strFilePath)
{
  CSingleLock lock(s_sourcesMutex);

  CVDMPlayer::PlaylistPtr playlist;

  if( !s_bPlayerCoreInited )
    return playlist;

  CStdString filePath = strFilePath;
  XSource::CPlaylist* pRawPlaylist = g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_GetPlaySourceMainmoviePlaylist(strFilePath.c_str());

  if (pRawPlaylist)
    playlist = ConvertPlaylistObject(strFilePath.c_str(), pRawPlaylist);

  return playlist;
}

void CVDMPlayer::GetPlaySourceMetaKeyValue(const std::string& strFilePath, const std::string& strKey, std::string& strValue)
{
  if( !s_bPlayerCoreInited )
    return;

  const char* szValue = g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_GetPlaySourceMeta(strFilePath.c_str(), strKey.c_str());

  if (szValue)
    strValue = szValue;
}

void CVDMPlayer::GetPlaySourceMetaInfo(const std::string& strFilePath, PlaySourceMetaInfo& metaInfo)
{
  CSingleLock lock(s_sourcesMutex);

  if( !s_bPlayerCoreInited )
    return;

  //need call parser it first, if not any playlist, still have any meta information ?

  int playlist = GetPlaySourcePlaylistCount(strFilePath);

  GetPlaySourceMetaKeyValue(strFilePath.c_str(), "FileFormat", metaInfo.strFileFormat);
  GetPlaySourceMetaKeyValue(strFilePath.c_str(), "VideoCodec", metaInfo.strVideoCodec);
  GetPlaySourceMetaKeyValue(strFilePath.c_str(), "VideoResolution", metaInfo.strVideoResolution);
  GetPlaySourceMetaKeyValue(strFilePath.c_str(), "AudioChannel", metaInfo.strAudioChannel);
  GetPlaySourceMetaKeyValue(strFilePath.c_str(), "AudioCodec", metaInfo.strAudioCodec);
  GetPlaySourceMetaKeyValue(strFilePath.c_str(), "AudioLanguage", metaInfo.strAudioLanguage);
  GetPlaySourceMetaKeyValue(strFilePath.c_str(), "SubtitleCodec", metaInfo.strSubtitleCodec);
  GetPlaySourceMetaKeyValue(strFilePath.c_str(), "SubLanguage", metaInfo.strSubtitleLanguage);

  //menu mode support value: none, hdmv, bdj

  if( metaInfo.strFileFormat == "bluray" )
    GetPlaySourceMetaKeyValue(strFilePath.c_str(), "MenuMode", metaInfo.strMenuMode);
}

std::string CVDMPlayer::GetPlaySourceThumbnail(const std::string& strFilePath, int playlist, int width, int height, int time)
{
  CSingleLock lock(s_sourcesMutex);

  std::string strThumbnail;

  if( !s_bPlayerCoreInited )
    return strThumbnail;

#if defined(TARGET_ANDROID)
  //cache on sd card
  CStdString strDestPath = CSpecialProtocol::TranslatePath("special://home/temp/");
#else
  CStdString strDestPath = CSpecialProtocol::TranslatePath("special://temp/");
#endif

  CStdString strFileName = URIUtils::GetFileName(strFilePath.c_str());
  strFileName = StringUtils::Left(strFileName, strFileName.find_last_of('.'));
  strDestPath += strFileName;

  if (!URIUtils::HasSlashAtEnd(strDestPath))
  {
    URIUtils::AddSlashAtEnd(strDestPath);
  }

  CStdString strDestFolder = strDestPath;
  strDestPath = StringUtils::Format("%s%05d.jpg", strDestPath.c_str(), playlist);

  bool bNeedsRecaching = false;
  CStdString strLoadPath = CTextureCache::Get().CheckCachedImage(strDestPath, true, bNeedsRecaching);
  if (!bNeedsRecaching && !strLoadPath.empty())
  {
    return strLoadPath.c_str();
  }

  if (!XFILE::CDirectory::Exists(strDestFolder))
  {
    XFILE::CDirectory::Create(strDestFolder);
  }

  bool bRet = g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_GetPlaySourceThumb(strFilePath.c_str(), strDestPath.c_str(), width, height, time, playlist);

  if (bRet)
  {
    CLog::Log(LOGINFO, "Extract image from %s to %s successfully!", strFilePath.c_str(), strDestPath.c_str());

    CTextureDetails details;
    strThumbnail = CTextureCache::Get().CacheImage(strDestPath, NULL, &details).c_str();
    if (!strThumbnail.empty())
    {
      CLog::Log(LOGINFO, "Cache image from %s to %s successfully!", strDestPath.c_str(), strThumbnail.c_str());
    }
    else
    {
      CLog::Log(LOGINFO, "Cache image from %s to %s failed!", strDestPath.c_str(), strThumbnail.c_str());
    }

    XFILE::CFile::Delete(strDestPath);
  }
  else
  {
    CLog::Log(LOGINFO, "Extract image from %s to %s failed!", strFilePath.c_str(), strDestPath.c_str());
  }  

  return strThumbnail;
}

void CVDMPlayer::PrepareStream()
{
  if (!m_pPlayer)
    return ;
  m_pPlayer->GetAllStreamInfo();
}

CStdString CVDMPlayer::GetAudioCodecName()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return "";
  PrepareStream();

  MediaStreamInfo * pinfo = m_pPlayer->GetCurrentStreamInfo(1);
  if (!pinfo)
    return "";

  return pinfo->codec;
}

CStdString CVDMPlayer::GetVideoCodecName()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return "";
  PrepareStream();
  MediaStreamInfo * pinfo = m_pPlayer->GetCurrentStreamInfo(2);
  if (!pinfo)
    return "";

  return pinfo->codec;
}

int CVDMPlayer::GetChannels()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return 0;
  PrepareStream();
  MediaStreamInfo * pinfo = m_pPlayer->GetCurrentStreamInfo(1);
  if (!pinfo)
    return 0;

  return pinfo->channels;
}

int CVDMPlayer::GetPictureWidth()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return 0;
  PrepareStream();
  MediaStreamInfo * pinfo = m_pPlayer->GetCurrentStreamInfo(2);
  if (!pinfo)
    return 0;

  return pinfo->width;
}

int CVDMPlayer::GetPictureHeight()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return 0;
  PrepareStream();
  MediaStreamInfo * pinfo = m_pPlayer->GetCurrentStreamInfo(2);
  if (!pinfo)
    return 0;

  return pinfo->height;
}

void CVDMPlayer::GetVideoAspectRatio(float& fAR)
{
  fAR = 0.0f;
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return;

  fAR = m_pPlayer->GetVideoAspectRatio();
}

bool CVDMPlayer::GetCurrentSubtitle(CStdString& strSubtitle)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return false;

  char * stemp;
  stemp = (char *)m_pPlayer->GetCurrentSubtitle();
  if (strcmp(stemp,"") == 0)
    return false;

  strSubtitle = stemp;
  return true;
}

bool CVDMPlayer::IsCaching() const
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return false;

  return m_pPlayer->IsCaching();
}

int CVDMPlayer::GetCacheLevel() const
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return 0;

  return m_pPlayer->GetCacheLevel();
}

float CVDMPlayer::GetCachePercentage()
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return 0;

  return m_pPlayer->GetCachePercentage();
}

int CVDMPlayer::AddSubtitle(const CStdString& strSubPath)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return -1;

  CLog::Log(LOGINFO, "*****************AddSubtitle's strSubPath = %s *************************", strSubPath.c_str());
  return m_pPlayer->AddSubtitle(strSubPath.c_str());
}

const char*  CVDMPlayer::GetSubtitlePath(int iStream)
{
  CSingleLock lock(s_renderlock);
  if (!m_pPlayer)
    return NULL;

  return m_pPlayer->GetSubtitlePath(iStream);
}


bool CVDMPlayer::IsVirtualFile( CStdString& strPath )
{
  if( StringUtils::EqualsNoCase(StringUtils::Right(strPath, 11),"folder.bdmv")
    || StringUtils::EqualsNoCase(StringUtils::Right(strPath, 10),"folder.ifo")
    || StringUtils::EqualsNoCase(StringUtils::Right(strPath, 11),"folder.bdav")
    || StringUtils::EqualsNoCase(StringUtils::Right(strPath, 12),"folder.dvdvr")
    || StringUtils::EqualsNoCase(StringUtils::Right(strPath, 10),"folder.bdm") )
    return true;

  return false;
}

CStdString CVDMPlayer::DecodePathFromVirtualToReal( CStdString& strPath )
{
  CStdString strFile = strPath;
  if(StringUtils::EqualsNoCase(StringUtils::Right(strPath, 11),"folder.bdmv"))
  {        
    strFile = StringUtils::Left(strPath, strPath.length() - 11);
    strFile += "BDMV/index.bdmv";
  }
  else if(StringUtils::EqualsNoCase(StringUtils::Right(strPath, 10),"folder.ifo"))
  {
    strFile = StringUtils::Left(strPath, strPath.length() - 10);
    strFile += "VIDEO_TS/VIDEO_TS.IFO";
  }
  else if( StringUtils::EqualsNoCase(StringUtils::Right(strPath, 11),"folder.bdav"))
  {
    strFile = StringUtils::Left(strPath, strPath.length() - 11);
    strFile += "BDAV/info.bdav";
  }
  else if( StringUtils::EqualsNoCase(StringUtils::Right(strPath, 12),"folder.dvdvr"))
  {
    strFile = StringUtils::Left(strPath, strPath.length() - 12);
    strFile += "BDMV/index.bdmv";
  }
  else if( StringUtils::EqualsNoCase(StringUtils::Right(strPath, 10),"folder.bdm") )
  {
    strFile = StringUtils::Left(strPath, strPath.length() - 10);
    strFile += "BDMV/index.bdm";
  }

  CUtil::ForceForwardSlashes(strFile);

  return strFile;
}

void CVDMPlayer::GetSMBUsernamePassword(CStdString &strFilePath)
{
  strFilePath = CURL::Decode(CURL::Decode(strFilePath));
  CStdString::size_type pos = strFilePath.find("smb://");
  if (pos != CStdString::npos)//is SMB
  {
    CLog::Log(LOGDEBUG, "strProtocol : SMB");

    CStdString strSmbFilePath;

    if (pos != 0)
      strSmbFilePath = strFilePath.substr(pos,strFilePath.length() - pos);
    else
      strSmbFilePath = strFilePath;

    CLog::Log(LOGDEBUG, "vdmplayer: smb path = %s", strSmbFilePath.c_str());

    CURL url(URIUtils::SubstitutePath(strSmbFilePath));

    bool bXBMCUI;
    if (url.GetUserName() == "" && url.GetPassWord() == "")//From XBMC UI (Looks like bluray://udf%3a%2f%2fsmb%253a%252f%252f10.10.2.184%252fav%252fbd%252f2012.iso%2f/BDMV/PLAYLIST/00001.mpls)
      bXBMCUI = true;
    else//From VMC UI 
      bXBMCUI = false;

#if defined(HAS_FILESYSTEM_SMB)
    if (bXBMCUI)
      CPasswordManager::GetInstance().AuthenticateURL(url);
    CLog::Log(LOGDEBUG, "vdmplayer: set smb userdetails username = %s, pwd = %s", url.GetUserName().c_str(), url.GetPassWord().c_str());
    SetSMBUserName(url.GetUserName());
    SetSMBPassword(url.GetPassWord());
#endif 

    if (!bXBMCUI)
      strFilePath = url.GetWithoutUserDetails();
  }
}

void CVDMPlayer::SetNetworkSetting()
{
  if (CSettings::Get().GetBool("network.usehttpproxy") 
    && !CSettings::Get().GetString("network.httpproxyserver").empty() 
    && CSettings::Get().GetInt("network.httpproxyport") > 0)
  {
    g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingBool(NETWORK_USE_HTTP_PROXY, true);
    g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingString(NETWORK_HTTP_PROXY_SERVER, CSettings::Get().GetString("network.httpproxyserver").c_str());
    g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingInt(NETWORK_HTTP_PROXY_PORT, CSettings::Get().GetInt("network.httpproxyport"));

    if (CSettings::Get().GetString("network.httpproxyusername").length() > 0)
    {
      g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingString(NETWORK_HTTP_PROXY_USERNAME, CSettings::Get().GetString("network.httpproxyusername").c_str());
      g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_SetSettingString(NETWORK_HTTP_PROXY_PASSWORD, CSettings::Get().GetString("network.httpproxypassword").c_str());
    }
  }
}

//add by yong.li 2014.08.26 for external subtitle setting 
void CVDMPlayer::UpdateSubtitleSetting(int size, int style, int color, int align)
{
  if (!m_pPlayer)
    return ;

  //translation parameters
  //now only test
  SubAttribute subAttribute;
  subAttribute.fontsize = size;
  subAttribute.fontstyle = style;
  subAttribute.fontcolor = color;
  subAttribute.fontalign = align;

  m_pPlayer->UpdateSubtitleSetting((SubAttribute*)&subAttribute);

}

//////////////////////////////////////////////////////////////////////////
//local filesystem support 

using namespace XFILE;

struct ExternalFileHandle
{
  IFile* pFile;

  //TODO, other member

};

static ExternalFileHandle* Exfsys_Open(const char* path)
{
  ExternalFileHandle* exhandle;

  CURL url(path);

  IFile* file = XFILE::CFileFactory::CreateLoader(url);

  if( file )
  {
    if( file->Open(url) )
    {
      exhandle = new ExternalFileHandle;
      exhandle->pFile = file;
    }
    else
    {
      CLog::Log(LOGERROR, "Open file(%s) failed", path);
    }
  }

  return exhandle;
}

static void Exfsys_Close(ExternalFileHandle* h)
{
  if( h && h->pFile )
  {
    h->pFile->Close();
    delete h->pFile;
    delete h;
  }
}

static int64_t Exfsys_Seek(ExternalFileHandle* h, int64_t iFilePosition, int iWhence)
{
  if( h && h->pFile )
    return h->pFile->Seek(iFilePosition, iWhence);

  return 0;
}

static unsigned int Exfsys_Read(ExternalFileHandle* h, void* lpBuf, int64_t uiBufSize, int flags)
{
  if( h && h->pFile )
    return h->pFile->Read(lpBuf, uiBufSize);

  return 0;
}

static int Exfsys_Stat(ExternalFileHandle* h, struct __stat64* buffer)
{
  if( h && h->pFile )
    return h->pFile->Stat(buffer);

  return 0;
}

static int Exfsys_Truncate(ExternalFileHandle* h, int64_t size)
{
  if( h && h->pFile )
    return h->pFile->Truncate(size);

  return 0;
}

static int64_t Exfsys_GetLength(ExternalFileHandle* h)
{
  if( h && h->pFile )
    return h->pFile->GetLength();

  return 0;
}

static int64_t Exfsys_GetPosition(ExternalFileHandle* h)
{
  if( h && h->pFile )
    return h->pFile->GetPosition();

  return 0;
}

static int  Exfsys_GetChunkSize(ExternalFileHandle* h)
{
  if( h && h->pFile )
    return h->pFile->GetChunkSize();

  return 0;
}

static bool Exfsys_Exists(const char* path)
{
  CURL url(path);

  IFile* file = XFILE::CFileFactory::CreateLoader(url);

  if( file )
    return file->Exists(url);

  return false;
}

static int Exfsys_StatEx(const char* path, struct __stat64* buffer)
{
  CURL url(path);

  IFile* file = XFILE::CFileFactory::CreateLoader(url);

  if( file )
    return file->Stat(url, buffer);

  return 0;
}

//Directory support 

using namespace XFILE;

struct ExternalDirectoryHandle
{
  CFileItemList itemList;
  XFILE::IDirectory* pDirectory;
};

static ExternalDirectoryHandle* Exfsys_GetDirectory(const char* path)
{
  ExternalDirectoryHandle* handle = new ExternalDirectoryHandle;

  CStdString strPath = path;

  handle->pDirectory = CDirectoryFactory::Create(strPath);

  if( !handle->pDirectory )
  {
    delete handle;
    return NULL;
  }

  if( !handle->pDirectory->GetDirectory(strPath, handle->itemList) )
  {
    delete handle->pDirectory;
    delete handle;
    return NULL;
  }

  return handle;
}

static int Exfsys_GetDirectoryItemCount(ExternalDirectoryHandle* handle)
{
  if( handle )
    return handle->itemList.Size();

  return 0;
}

static const char* Exfsys_GetDirectoryItemLabel(ExternalDirectoryHandle* handle, int index)
{
  static CStdString strEmpty;

  if( handle )
  {
    CFileItemPtr item = handle->itemList.Get(index);

    if( item )
      return item->GetLabel().c_str();
  }

  return strEmpty.c_str();
}

static const char* Exfsys_GetDirectoryItemAttr(ExternalDirectoryHandle* handle, int index, bool* bIsFolder)
{
  static CStdString strEmpty;

  *bIsFolder = false;

  if( handle )
  {
    CFileItemPtr item = handle->itemList.Get(index);

    if( item )
    {
      *bIsFolder = item->m_bIsFolder;
      return item->GetPath().c_str();
    }
  }

  return strEmpty.c_str();
}

static void Exfsys_CloseGetDirectory(ExternalDirectoryHandle* handle)
{
  if( handle )
  {
    if( handle->pDirectory )
      delete handle->pDirectory;

    delete handle;
  }
}

static bool Exfsys_CreateDirectory(const char* path)
{
  CStdString strPath = path;
  CStdString realPath = URIUtils::SubstitutePath(strPath);

  std::auto_ptr<IDirectory> pDirectory(CDirectoryFactory::Create(realPath));

  if (pDirectory.get())
    return pDirectory->Create(realPath.c_str());

  return false;
}

static bool Exfsys_ExistsDirectory(const char* path)
{
  CStdString strPath = path;
  CStdString realPath = URIUtils::SubstitutePath(strPath);

  std::auto_ptr<IDirectory> pDirectory(CDirectoryFactory::Create(realPath));

  if (pDirectory.get())
    return pDirectory->Exists(realPath.c_str());

  return false;
}

static bool Exfsys_RemoveDirectory(const char* path)
{
  CStdString strPath = path;
  CStdString realPath = URIUtils::SubstitutePath(strPath);

  std::auto_ptr<IDirectory> pDirectory(CDirectoryFactory::Create(realPath));

  if (pDirectory.get())
    return pDirectory->Remove(realPath.c_str());

  return false;
}

static bool Exfsys_IsAllowedDirectory(const char* filePath)
{
  return true;
}

void CVDMPlayer::LoadExternalFileSystem( PlayercoreConfig* pConfig )
{
  static ExternalFilesystem exfsys;

  exfsys.Open = Exfsys_Open;
  exfsys.Close = Exfsys_Close;
  exfsys.Seek = Exfsys_Seek;
  exfsys.Read = Exfsys_Read;
  exfsys.Stat = Exfsys_Stat;
  exfsys.Truncate = Exfsys_Truncate;
  exfsys.GetLength = Exfsys_GetLength;
  exfsys.GetPosition = Exfsys_GetPosition;
  exfsys.GetChunkSize = Exfsys_GetChunkSize;
  exfsys.Exists = Exfsys_Exists;
  exfsys.StatEx = Exfsys_StatEx;

  exfsys.GetDirectory = Exfsys_GetDirectory;
  exfsys.GetDirectoryItemCount = Exfsys_GetDirectoryItemCount;
  exfsys.GetDirectoryItemLabel = Exfsys_GetDirectoryItemLabel;
  exfsys.GetDirectoryItemAttr = Exfsys_GetDirectoryItemAttr;
  exfsys.CloseGetDirectory = Exfsys_CloseGetDirectory;

  exfsys.CreateDirectory = Exfsys_CreateDirectory;
  exfsys.ExistsDirectory = Exfsys_ExistsDirectory;
  exfsys.RemoveDirectory = Exfsys_RemoveDirectory;
  exfsys.IsAllowedDirectory = Exfsys_IsAllowedDirectory;

  pConfig->externalFilesystem = &exfsys;
}

const char* CVDMPlayer::GetAppName()
{
  static const char* APP_NAME = "XBMC";
  return APP_NAME;
}

const char* CVDMPlayer::GetAppVersion()
{
  static const char* APP_VERSION = "13.2";
  return APP_VERSION;
}

const char* CVDMPlayer::GetAppPackageName()
{
  static const char* APP_PACKAGE_NAME = "org.vidonme.xbmc13";
  return APP_PACKAGE_NAME;
}

const char* CVDMPlayer::GetPlayerCoreVersion()
{
  InitializePlayerCore();
  if(!s_bPlayerCoreInited)
    return NULL;
  return g_DllVidonUtils.GetVidonPlayerManagerInstance()->Vidonplayer_GetVersionInfo();
}

const std::map<CStdString, CStdString>& CVDMPlayer::GetPlayerCoreVersion2()
{
    static bool s_bGetPlayerCoreVersion = false;
    static std::map<CStdString, CStdString> s_versions;
    if (s_bGetPlayerCoreVersion) return s_versions;

    const char* const szPlayerCoreVersion = GetPlayerCoreVersion();
	if (!szPlayerCoreVersion) return s_versions;

    s_bGetPlayerCoreVersion = true;
    CStdStringArray lines = StringUtils::SplitString(szPlayerCoreVersion, "\n");
    for (CStdStringArray::iterator line = lines.begin()
        ; line != lines.end(); ++line )
    {
        CStdStringArray keyVal = StringUtils::SplitString(*line, ":");
        if (keyVal.size() == 2)
        {
            StringUtils::Trim(keyVal[0]);
            StringUtils::Trim(keyVal[1]);
            s_versions.insert(std::make_pair(keyVal[0], keyVal[1]));
        }
    }

    return s_versions;
}



bool CVDMPlayer::HandlesType(const CStdString &type)
{
  
  //current in amlogic-box, we need check using vdmplayer or dvdplayer
  //default is not set using vdmplayer here, will decide by late logic
  bool bSupportType = true;
  bool passthrough = false;
#if defined(TARGET_ANDROID)
  if(CT_UNKNOWN == g_cpuInfo.GetCPUType())
  {
    bSupportType = false;
    CLog::Log(LOGWARNING, "vdmplayer unknown  cpu hardware= %s",g_cpuInfo.getCPUHardware().c_str());
    return bSupportType;
  }
#endif

  do 
  {
#if defined(TARGET_ANDROID)

    //check amlogic chip 
    bool bAmlogic = false;

#if defined(HAS_LIBAMCODEC)
    //in amlogic box need check it
    if(CT_AMLOGIC_M8 == g_cpuInfo.GetCPUType())
      bAmlogic = true;
#endif 

    //TODO, other chip all using vdmplayer
    if( !bAmlogic )
      break;

    //enable passthrough, need using vdmplayer
    passthrough = CSettings::Get().GetBool("audiooutput.passthrough");
    if( passthrough )
      break;

    //not using amlogic codec, using vmdplayer
    if( !CSettings::Get().GetBool("videoplayer.useamcodec") )
      break;

    //check it's common file or special file
    //not using amlogic, not need set it
    //dvd,bluray using vdmplayer
    if ( type.compare("iso") == 0 
        || type.compare("bdmv") == 0 
        || type.compare("mpls") == 0
        || type.compare("ifo") == 0 )
     {       
        break;
     }
    
#else

    //other platforam will using vdmplayer
    break;

#endif //ANDROID

    //using xbmc-dvdplayer
    bSupportType = false;

  } while (false);

  CLog::Log(LOGINFO, "vdmplayer type(%s) check = %d, passthrough = %d", type.c_str(), bSupportType, passthrough);

  return bSupportType;

}


void CVDMPlayer::SetPlayMode(DIMENSIONMODE mode)
{
#if defined(TARGET_VIDONME_BOX)
    switch (mode)
    {
    case VS_2D_DISPLAY_THE_ORIGINAL:
      m_pPlayer->SetAllWinnerDisplayMode (ALLWINNER_DISPLAYMODE_2D_FULL);
      break;

    case VS_2D_DISPLAY_THE_LEFT_HALF:
      m_pPlayer->SetAllWinnerDisplayMode (ALLWINNER_DISPLAYMODE_2D_LEFT);
      break;

    case VS_2D_DISPLAY_THE_UPPER_HALF:
      m_pPlayer->SetAllWinnerDisplayMode (ALLWINNER_DISPLAYMODE_2D_RIGHT);
      break;

    case VS_3D_LEFT_RIGHT:
      m_pPlayer->SetAllWinnerDisplayMode (ALLWINNER_DISPLAYMODE_3D_LEFT_RIGHT);
      break;

    case VS_3D_TOM_BOTTOM:
      m_pPlayer->SetAllWinnerDisplayMode (ALLWINNER_DISPLAYMODE_3D_TOP_BOTTOM);
      break;

    case VS_3D_INTERLACING:
      m_pPlayer->SetAllWinnerDisplayMode (ALLWINNER_DISPLAYMODE_3D_DUAL_STREAM);
      break;

    default:
      break; 
    }
#endif
}

#endif //HAS_VIDONME
