
#ifdef HAS_VIDONME

#include "VDMVersionUpdate.h"

#include "utils/URIUtils.h"
#include "utils/log.h"
#include "Util.h"
#include "filesystem/File.h"
#include "guilib/GUIMessage.h"
#include "GUIUserMessages.h"
#include "guilib/GUIWindowManager.h"
#include "utils/JobManager.h"
#include "utils/StringUtils.h"
#include "dialogs/GUIDialogYesNo.h"
#include "dialogs/GUIDialogOK.h"
#include "utils/AsyncFileCopy.h"
#include "filesystem/CurlFile.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/CPUInfo.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "vidonme/VDMRegionFeature.h"
#include "settings/Settings.h"
#include "DLLVidonUtils.h"

using namespace XFILE;

#if defined(TARGET_ANDROID)
#include "android/activity/XBMCApp.h"
#include "android/jni/PackageInfo.h"
#endif

#define VERSIONCHECK_CLIENTTYPE    "XBMC15"

enum VERSION_STATUS
{
  VERSION_STATUS_UNKNOWN,
  VERSION_STATUS_NORMAL, 
  VERSION_STATUS_LATEST,
  VERSION_STATUS_INVALID_PARAMS,
  VERSION_STATUS_INVALID_CHIPID,
  VERSION_STATUS_FREEVER_UPGRADE,
};

std::string CVDMVersionCheck::GetCurrVersionTime()
{
#if defined(VERSION_TIME)
  return VERSION_TIME;
#else
  return  __DATE__ " " __TIME__;
#endif
}

std::string CVDMVersionCheck::GetCurrVersionCode()
{
#if defined(TARGET_ANDROID)
  return StringUtils::Format("%d", CXBMCApp::GetPackageInfo().versionCode);
#else
  return "1";
#endif
}

std::string CVDMVersionCheck::GetCurrVersionName()
{
#if defined(TARGET_ANDROID)
  return CXBMCApp::GetPackageInfo().versionName;
#else
  return "0.0.0.1";
#endif
}

bool CVDMVersionCheck::VersionCheck(OUT CVDMVersionInfo& info)
{
  bool bRet = false;
  if( g_DllVidonUtils.IsLoaded() )
  {
    VidonCheckVersionHandle hVersinHandle = g_DllVidonUtils.Vidon_CreateCheckVersionHandle();
    bRet = g_DllVidonUtils.Vidon_CheckVersion( hVersinHandle, VERSIONCHECK_CLIENTTYPE, GetCurrVersionCode().c_str(), CSettings::Get().GetString("locale.language").c_str() );
    if( bRet )
    {
      info.bIsBeta = g_DllVidonUtils.Vidon_IsBetaVersion( hVersinHandle );
      info.nSize = g_DllVidonUtils.Vidon_GetFileSize( hVersinHandle );
      info.nVersionCode = g_DllVidonUtils.Vidon_GetVersionCode( hVersinHandle );
      info.state = g_DllVidonUtils.Vidon_HasNewVersion( hVersinHandle ) ? CVDMVersionInfo::sUpdateAvail : CVDMVersionInfo::sUpdateNone;
      info.strChangeLog = g_DllVidonUtils.Vidon_GetChangeLog( hVersinHandle );
      info.strHash = g_DllVidonUtils.Vidon_GetFileHashValue( hVersinHandle );
      info.strUrl = g_DllVidonUtils.Vidon_GetDownloadUrl( hVersinHandle );
      info.strVersionName = g_DllVidonUtils.Vidon_GetVersionName( hVersinHandle );
    }
    g_DllVidonUtils.Vidon_DestroyCheckVersionHandle( hVersinHandle );
  }
  return bRet;
}

class CVDMVersionCheckInBackgroundJob : public CJob
{
public:
  virtual bool DoWork()
  {
    return CVDMVersionCheck::VersionCheck( m_info );
  }
  const CVDMVersionInfo& GetResult(){ return m_info; };
private:
  CVDMVersionInfo m_info;
};

bool CVDMVersionUpdateJob::DownloadPackage(const std::string& path, const std::string& dest)
{
  CFileItemList list;
  list.Add(CFileItemPtr(new CFileItem(path,false)));
  list[0]->Select(true);
  SetFileOperation(CFileOperationJob::ActionReplace, list, dest);
  m_bKeepCache = true;
  return CFileOperationJob::DoWork();
}

bool CVDMVersionUpdateJob::Install(const std::string& package)
{
  CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info,
    g_localizeStrings.Get(70063),
    g_localizeStrings.Get(70072),
    TOAST_DISPLAY_TIME, false);

  bool bRet = false;

#if defined(TARGET_WINDOWS)

  bool bShow = true;
  HINSTANCE hInst = ShellExecuteA(NULL, "open", package.c_str(), NULL, NULL, bShow ? SW_SHOW : SW_HIDE);
  bRet = ((int)hInst > 32);

#elif defined(TARGET_ANDROID)

  std::string strAppPath;
  strAppPath = StringUtils::Format("file://%s", package.c_str()); 
  bRet = CXBMCApp::StartActivity("", "android.intent.action.VIEW", "application/vnd.android.package-archive", strAppPath.c_str());
#endif

  Sleep(TOAST_DISPLAY_TIME);
  return bRet;
}

void CVDMVersionUpdateJob::ReportDownloadError(const std::string& fileName)
{
  CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Error,
    g_localizeStrings.Get(70063),
    g_localizeStrings.Get(70073),
    TOAST_DISPLAY_TIME, false);
}

void CVDMVersionUpdateJob::ReportValidateError(const std::string& fileName)
{
  CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Error,
    g_localizeStrings.Get(70063),
    g_localizeStrings.Get(70074),
    TOAST_DISPLAY_TIME, false);
}


CVDMVersionUpdate::CVDMVersionUpdate()
  : m_nJobID(0)
  , m_nProgress(0)
  , m_nJobIDCheckVersionInBackground(0)
{

};

CVDMVersionUpdate::~CVDMVersionUpdate()
{
  //Stop();
}

void CVDMVersionUpdate::OnJobComplete(unsigned int jobID, bool success, CJob* job)
{
  CSingleLock lock(m_mutex);
  if (m_nJobID == jobID)
  { 
    m_nJobID = 0;
    m_nProgress = 0;
    lock.Leave();
    //success
    CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE_ITEM);
    //msg.SetStringParam(i->first);
    lock.Leave();
    g_windowManager.SendThreadMessage(msg);
  }
  else if( m_nJobIDCheckVersionInBackground == jobID )
  {
    m_nJobIDCheckVersionInBackground = 0;
    if( success )
    {
      CVDMVersionCheckInBackgroundJob* pCheckVersionJob = (CVDMVersionCheckInBackgroundJob*) job;

      if( NULL != pCheckVersionJob )
      {
        if( pCheckVersionJob->GetResult().state == CVDMVersionInfo::sUpdateAvail )
        {
          CGUIMessage msgVersionInfo(GUI_MSG_UPDATE_HASNEWVERSION, 0, WINDOW_HOME);
          CVDMVersionInfo* pInfo = new CVDMVersionInfo();
          *pInfo = pCheckVersionJob->GetResult();
          msgVersionInfo.SetPointer( pInfo );
          lock.Leave();
          g_windowManager.SendThreadMessage( msgVersionInfo, WINDOW_HOME );
        }
      }
    }
  }
}

void CVDMVersionUpdate::OnJobProgress(unsigned int jobID, unsigned int progress, unsigned int total, const CJob *job)
{
  CSingleLock lock(m_mutex);
  if (m_nJobID == jobID)
  {
    m_nProgress = progress;

    CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE_ITEM);
    //msg.SetStringParam(i->first);
    lock.Leave();
    g_windowManager.SendThreadMessage(msg);
  }
}

bool CVDMVersionUpdate::IsCheckInBackground()
{
  CSingleLock lock(m_mutex);
  return m_nJobIDCheckVersionInBackground != 0;
}

bool CVDMVersionUpdate::CheckVersionInBackground()
{
  CSingleLock lock(m_mutex);

  if( m_nJobIDCheckVersionInBackground != 0 ) return false;

  m_nJobIDCheckVersionInBackground = CJobManager::GetInstance().AddJob( new CVDMVersionCheckInBackgroundJob(), this );

  return true;
}

void CVDMVersionUpdate::CancelCheckVersionInBackground()
{
  CSingleLock lock(m_mutex);

  if( m_nJobIDCheckVersionInBackground != 0 )
  {
    CJobManager::GetInstance().CancelJob( m_nJobIDCheckVersionInBackground );
    m_nJobIDCheckVersionInBackground = 0;
  }

}

void CVDMVersionUpdate::Start(const CVDMVersionInfo& info)
{
  CSingleLock lock(m_mutex);
  if (m_nJobID) return;

  // don't run repo update jobs while on the login screen which runs under the master profile
  if((g_windowManager.GetActiveWindow() & WINDOW_ID_MASK) == WINDOW_LOGIN_SCREEN)
    return;


  m_nJobID = CJobManager::GetInstance().AddJob(new CVDMVersionUpdateJob(info), this);
}

bool CVDMVersionUpdate::Stop( void )
{
  CSingleLock lock(m_mutex);
  if (m_nJobID != 0)
  {
    CJobManager::GetInstance().CancelJob(m_nJobID);
    m_nJobID = 0;
    m_nProgress = 0;
    return true;
  }
  CancelCheckVersionInBackground();
  return false;
}


bool CVDMVersionUpdateJob::DoWork()
{
#if defined(TARGET_ANDROID)
  std::string dest;
  if (!CXBMCApp::GetExternalStorage(dest, "downloads"))
  {
    dest="special://userdata";
  }
#else
  std::string dest="special://temp/";
#endif
  std::string package = URIUtils::AddFileToFolder(dest,
    URIUtils::GetFileName(m_info.strUrl));
  CLog::Log(LOGINFO, "package: %s", package.c_str());

  if ( m_info.strHash.empty())
    CFile::Delete(package);

  if ( !DownloadPackage(m_info.strUrl, dest) ) 
  {
    ReportDownloadError(m_info.strUrl);
    return false;
  }

  if ( !m_info.strHash.empty() 
    && CUtil::GetFileMD5(package) != m_info.strHash)
  {
    CFile::Delete(package);
    CLog::Log(LOGERROR, "MD5 mismatch after download %s", package.c_str());

    ReportValidateError(CSpecialProtocol::TranslatePath(package));
    return false;
  }

  if (!Install(package))
    return false; // something went wrong

  return true;
}

#endif
