#if defined(HAS_VIDONME)
#include "VDMUserInfo.h"
#include "DLLVidonUtils.h"
#include "utils/Job.h"
#include "utils/JobManager.h"
#include "utils/StringUtils.h"
#include "utils/XBMCTinyXML.h"
#include "filesystem/SpecialProtocol.h"
#include "utils/URIUtils.h"
#include "profiles/ProfilesManager.h"
#include "guilib/LocalizeStrings.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "utils/log.h"
#include "settings/Settings.h"
#include "VDMRegionFeature.h"
#include "GUIUserMessages.h"
#include "guilib/GUIWindowManager.h"
#include "SynchroMethod.h"

class CLoginJob : public CJob
{
public:
  CLoginJob( const CStdString& strUserName, const CStdString& strPassword )
  {
    m_strUserName = strUserName;
    m_strPassword = strPassword;
    if( g_DllVidonUtils.IsLoaded() )
    {
      m_pLicenseManager = g_DllVidonUtils.CreateLicenseManager();
    }
    else
    {
      m_pLicenseManager = NULL;
      //m_strErrorDesc = g_localizeStrings.Get(70005);
      m_strErrorDesc = "libVidonUtils.dll is not loaded!";
    }
  }

  virtual ~CLoginJob()
  {
    if( g_DllVidonUtils.IsLoaded() )
    {
      g_DllVidonUtils.DestroyLicenseManager( m_pLicenseManager );
      m_pLicenseManager = NULL;
    }
  }

  virtual bool DoWork()
  {
    if( NULL != m_pLicenseManager )
    {
      bool bRet = m_pLicenseManager->ConnectToServer( m_strUserName.c_str(), m_strPassword.c_str() );
      if( !bRet )
      {
        m_strErrorDesc = g_localizeStrings.Get(70005);
      }
      return bRet;
    }

    return false;
  }

  libVidonUtils::LicenseManager* GetResult() { return m_pLicenseManager; };
  CStdString GetErrorDescription() { return m_strErrorDesc; } ;
  CStdString GetUserName() { return m_strUserName; };
  CStdString GetPassword() { return m_strPassword; };

private:
  CStdString m_strUserName;
  CStdString m_strPassword;
  libVidonUtils::LicenseManager* m_pLicenseManager;
  CStdString m_strErrorDesc;
};

CVDMUserInfo& CVDMUserInfo::Instance()
{
  static CVDMUserInfo s_userinfo;

  return s_userinfo;
}

CVDMUserInfo::CVDMUserInfo()
:m_event(true, true)
{
  m_pCurrentLicenseInfo = NULL;
  m_bLogin = false;
  m_eContryCode = libVidonUtils::CountryCode_Unkown;
  m_nlocalLicenseType = libVidonUtils::CurrentLicenseType_sUnknown;
}

static bool LoadVMCCountryCodeSetting( const CStdString& filepath, int& nCode )
{
  CXBMCTinyXML xmlDoc;
  if( !xmlDoc.LoadFile(filepath) )
  {
    return false;
  }

  TiXmlElement* pRoot = xmlDoc.RootElement();

  if( NULL == pRoot )
  {
    return false;
  }

  TiXmlElement* pVidonmeNode = pRoot->FirstChildElement("vidonme");
  if( pVidonmeNode )
  {
    TiXmlElement* pElem = pVidonmeNode->FirstChildElement("regionischina");
    if (pElem && pElem->GetText())
    {
      std::string strIsInChina = pElem->GetText();
      if( strIsInChina == "true" || strIsInChina == "1" )
      {
        nCode = libVidonUtils::CountryCode_China;
      }
      else
      {
        nCode = libVidonUtils::CountryCode_Others;
      }
      return true;
    }
  }

  return false;
}

bool CVDMUserInfo::GetUsernameAndPassword( CStdString& strUserName, CStdString& strPassword )
{
  if( m_strUserName.empty() || m_strPassword.empty() )
  {
    CStdString strUserDataFile = CSpecialProtocol::TranslatePath(URIUtils::AddFileToFolder(CProfilesManager::Get().GetUserDataFolder(), "userdata.xml"));

    if( !LoadUserInfoFromFile( strUserDataFile ) )
    {
#if 0 /*defined(TARGET_ANDROID)*/
      strUserDataFile = "/sdcard/Android/data/org.vidonme.mediacenter/files/.vmc/userdata/userdata.xml";
      if( !LoadUserInfoFromFile( strUserDataFile ) )
      {
        return false;
      }
      int nCountryCode = libVidonUtils::CountryCode_Others;
      if( LoadVMCCountryCodeSetting( "/sdcard/Android/data/org.vidonme.mediacenter/files/.vmc/userdata/guisettings.xml", nCountryCode ) )
      {
        SetCountryCode( (libVidonUtils::CountryCode)nCountryCode );
      }
      CLog::Log(LOGINFO, "username: %s", m_strUserName.c_str());
#else
      return false;
#endif //if defined(TARGET_ANDROID)
    }
  }

  strUserName = m_strUserName;
  strPassword = m_strPassword;
  return true;
}

bool CVDMUserInfo::LoadUserInfoFromFile( const CStdString& filepath )
{
  CXBMCTinyXML xmlDoc;
  if (!xmlDoc.LoadFile(filepath))
  {
    return false;
  }

  TiXmlElement* pRoot = xmlDoc.RootElement();
  std::string strRoot = pRoot->Value() ? pRoot->Value() : "";
  if (strRoot != "useraccount")
  {
    return false;
  }

  TiXmlElement* pCurrentUserNode = pRoot->FirstChildElement("currentuser");
  if (pCurrentUserNode)
  {
    TiXmlElement* pElem = pCurrentUserNode->FirstChildElement("username");
    if (pElem && pElem->GetText())
    {
      m_strUserName = pElem->GetText();
    }

    pElem = pCurrentUserNode->FirstChildElement("password");
    if (pElem && pElem->GetText())
    {
      m_strPassword = pElem->GetText();
    }

    pElem = pCurrentUserNode->FirstChildElement("countrycode");
    if (pElem && pElem->GetText())
    {
      SetCountryCode( (libVidonUtils::CountryCode)atoi(pElem->GetText()) );
    }
    else
    {
      SetCountryCode( libVidonUtils::CountryCode_Others );
    }

    pElem = pCurrentUserNode->FirstChildElement("licensetype");
    if (pElem && pElem->GetText())
    {
      m_nlocalLicenseType = atoi(pElem->GetText());
    }
    else
    {
      m_nlocalLicenseType = libVidonUtils::CurrentLicenseType_sTrail;
    }
  }

  return true;
}

bool CVDMUserInfo::SaveUsernameAndPassword( const CStdString& strUserName, const CStdString& strPassword )
{
  if( strUserName.empty() || strPassword.empty() )
  {
    return false;
  }

  m_strUserName = strUserName;
  m_strPassword = strPassword;

  CStdString strUserDataFile = CSpecialProtocol::TranslatePath(URIUtils::AddFileToFolder(CProfilesManager::Get().GetUserDataFolder(), "userdata.xml"));

  CXBMCTinyXML xmlDoc;
  if (!xmlDoc.LoadFile(strUserDataFile))
  {
    xmlDoc.Clear();
    TiXmlElement* pRoot = new TiXmlElement("useraccount");
    xmlDoc.LinkEndChild(pRoot);
  }

  TiXmlElement* pRoot = xmlDoc.RootElement();
  std::string strRoot = pRoot->Value() ? pRoot->Value() : "";
  if (strRoot != "useraccount")
  {
    pRoot->SetValue("useraccount");
    pRoot->Clear();
  }

  TiXmlElement* pCurrentUserNode = pRoot->FirstChildElement("currentuser");
  if (!pCurrentUserNode)
  {
    pCurrentUserNode = new TiXmlElement("currentuser");
    pRoot->LinkEndChild(pCurrentUserNode);
  }

  pCurrentUserNode->Clear();

  TiXmlText* pText = new TiXmlText(m_strUserName);
  TiXmlElement* pElem = new TiXmlElement("username");
  pElem->LinkEndChild(pText);
  pCurrentUserNode->LinkEndChild(pElem);

  pText = new TiXmlText(m_strPassword);
  pElem = new TiXmlElement("password");
  pElem->LinkEndChild(pText);
  pCurrentUserNode->LinkEndChild(pElem);

  pText = new TiXmlText(StringUtils::Format("%d", m_eContryCode));
  pElem = new TiXmlElement("countrycode");
  pElem->LinkEndChild(pText);
  pCurrentUserNode->LinkEndChild(pElem);

  pText = new TiXmlText(StringUtils::Format("%d", m_nlocalLicenseType));
  pElem = new TiXmlElement("licensetype");
  pElem->LinkEndChild(pText);
  pCurrentUserNode->LinkEndChild(pElem);

  return xmlDoc.SaveFile(strUserDataFile);
}

libVidonUtils::CountryCode CVDMUserInfo::GetCountryCode()
{
  CSingleLock lock(m_secUserInfo);
  if( m_eContryCode == libVidonUtils::CountryCode_Unkown )
  {
    GetUsernameAndPassword( m_strUserName, m_strPassword );
  }

  return m_eContryCode;
}

void CVDMUserInfo::SetCountryCode( libVidonUtils::CountryCode code )
{
  CSingleLock lock(m_secUserInfo);
  m_eContryCode = code;
  if( g_DllVidonUtils.IsLoaded() )
  {
    g_DllVidonUtils.SetCountryCode( m_eContryCode );
  }
  if( code == libVidonUtils::CountryCode_China )
  {
    CVDMRegionFeature::Get().SetType( CVDMRegionFeature::tChina );
  }
  else
  {
    CVDMRegionFeature::Get().SetType(CVDMRegionFeature::tInternation);
  }
}

bool CVDMUserInfo::IsLogin()
{
  CSingleLock lock(m_secUserInfo);
  return m_bLogin;
}

bool CVDMUserInfo::MaybeProVersion()
{
  CSingleLock lock(m_secUserInfo);
  bool bMaybePro = false;
  if( IsLogin() )
  {
    if( !IsCurrentLicenseEmpty() )
    {
      bMaybePro = (m_pCurrentLicenseInfo->type == libVidonUtils::CurrentLicenseType_sNormal);
    }
    else
    {
      bMaybePro = false;
    }
  }
  else
  {
    if( m_nlocalLicenseType == libVidonUtils::CurrentLicenseType_sUnknown )
    {
      m_nlocalLicenseType = libVidonUtils::CurrentLicenseType_sTrail;
      GetUsernameAndPassword( m_strUserName, m_strPassword );//load localLicenseType, TODO:should only load localLicenseType.
    }
    bMaybePro = (m_nlocalLicenseType == libVidonUtils::CurrentLicenseType_sNormal);
  }

  return bMaybePro;
}

bool CVDMUserInfo::DoLogin( CStdString strUserName, CStdString strPassword, bool bWait /*true*/ )
{
  {
    CSingleLock lock(m_secUserInfo);
    m_strErrorDescription.clear();
  }

  bool bRet = true;
  if( strUserName.empty() && strPassword.empty() )
  {
    bRet = GetUsernameAndPassword( strUserName, strPassword );
  }

  if( bRet )
  {
    if( bWait )
    {
      CLoginJob loginJob( strUserName, strPassword );
      bRet = loginJob.DoWork();
      OnJobComplete( 0, bRet, &loginJob );
    }
    else
    {
      m_event.Reset();
      CLoginJob* loginJob = new CLoginJob( strUserName, strPassword );
      CJobManager::GetInstance().AddJob(loginJob, this);
    }

    bRet = m_bLogin;
  }

  return bRet;
}

class Method_WaitLoginInBackground: public MethodBase
{
public:
  Method_WaitLoginInBackground( CEvent& event )
  :m_event(event)
  {
  }
  virtual bool MethodCall();
  virtual bool Cancel(){ return false; };
private:
  CEvent& m_event;
};

bool Method_WaitLoginInBackground::MethodCall()
{
  m_event.WaitMSec( 30000 );
  return true;
}

void CVDMUserInfo::WaitLoginInBackground( bool bShowUI )
{
  if( IsLogin() )
  {
    return;
  }

  if( bShowUI )
  {
    Method_WaitLoginInBackground* pWaitLogin = new Method_WaitLoginInBackground(m_event);
    SynchroMethodCall(boost::shared_ptr<MethodPtr>(new MethodPtr(pWaitLogin)));
  }
  else
  {
    m_event.WaitMSec( 30000 );
  }
}

CStdString CVDMUserInfo::GetLoginErrorDescription()
{
  CSingleLock lock(m_secUserInfo);
  return m_strErrorDescription;
}


bool CVDMUserInfo::IsCurrentLicenseAvailable()
{
  CSingleLock lock(m_secUserInfo);

  if( !IsCurrentLicenseEmpty() )
  {
    return m_pCurrentLicenseInfo->status == libVidonUtils::CurrentLicenseStatus_sNormal;
  }

  return false;
}

bool CVDMUserInfo::IsCurrentLicenseTrial()
{
  CSingleLock lock(m_secUserInfo);

  if( !IsCurrentLicenseEmpty() )
  {
    return m_pCurrentLicenseInfo->type == libVidonUtils::CurrentLicenseType_sTrail;
  }

  return false;
}


CStdString CVDMUserInfo::GetCurrentLicenseID()
{
  CStdString strLicenseID;
  CSingleLock lock(m_secUserInfo);

  if( !IsCurrentLicenseEmpty() )
  {
    strLicenseID = m_pCurrentLicenseInfo->lid;
  }

  return strLicenseID;
}


bool CVDMUserInfo::IsCurrentLicenseExpired()
{
  CSingleLock lock(m_secUserInfo);
  if( !IsCurrentLicenseEmpty() )
  {
    return m_pCurrentLicenseInfo->status == libVidonUtils::CurrentLicenseStatus_sExpire;
  }
  return false;
}

bool CVDMUserInfo::IsCurrentLicenseEmpty()
{
  CSingleLock lock(m_secUserInfo);
  return NULL == m_pCurrentLicenseInfo;
}

bool CVDMUserInfo::IsFunctionAvailable( const CStdString& strFuncID )
{
  CSingleLock lock(m_secUserInfo);

  return IsCurrentLicenseAvailable();
}

bool CVDMUserInfo::GetAvailableCode( std::vector<libVidonUtils::Code>& vecCode )
{
  CSingleLock lock(m_secUserInfo);

  std::vector<libVidonUtils::Code>::iterator iter = m_vecCode.begin();

  for( ; iter != m_vecCode.end(); iter++ )
  {
    if( iter->status == libVidonUtils::CodeStatus_sUnused )
    {
      vecCode.push_back( *iter );
    }
  }
  return true;
}

bool CVDMUserInfo::ParseLicenseInfo( libVidonUtils::LicenseManager* pLicenseManager )
{
  CSingleLock lock(m_secUserInfo);
  bool bRet = false;
  if( NULL != pLicenseManager )
  {
    libVidonUtils::ErrorCode eConnStatus = pLicenseManager->GetConnectErrorCode();
    if( eConnStatus == libVidonUtils::ErrorCode_sNormal )
    {
      int nLicenseCount = pLicenseManager->GetCurrentLicenseCount();
      if( nLicenseCount > 0 )
      {
        delete m_pCurrentLicenseInfo;
        m_pCurrentLicenseInfo = new libVidonUtils::CurrentLicense;
        enum { Priority_Expired = 1, Priority_Trial = 10, Priority_Order = 100, Priority_Normal = 1000 };
        int nMaxPriority = 0;
        for( int i=0; i < nLicenseCount; i++ )
        {
          int nPriority = 0;
          libVidonUtils::CurrentLicense license = pLicenseManager->GetCurrentLicense(i);
          if( license.status == libVidonUtils::CurrentLicenseStatus_sNormal )
          {
            nPriority += Priority_Normal;
          }
          else
          {
            nPriority += Priority_Expired;
          }
          if( license.type == libVidonUtils::CurrentLicenseType_sNormal )
          {
            nPriority += Priority_Order;
          }
          else
          {
            nPriority += Priority_Trial;
          }

          if( nMaxPriority < nPriority )
          {
            *m_pCurrentLicenseInfo = license;
            nMaxPriority = nPriority;
          }
        }
        m_nlocalLicenseType = m_pCurrentLicenseInfo->type;
      }
      else
      {
        delete m_pCurrentLicenseInfo;
        m_pCurrentLicenseInfo = NULL;
        m_nlocalLicenseType = libVidonUtils::CurrentLicenseType_sTrail;
      }

      int nCodeCount = pLicenseManager->GetCodeCount();
      m_vecCode.clear();
      for( int i=0; i < nCodeCount; i++ )
      {
        libVidonUtils::Code code = pLicenseManager->GetCode(i);
        m_vecCode.push_back( code );
      }
      m_strEmail = pLicenseManager->GetUserEmail() != NULL ? pLicenseManager->GetUserEmail(): "";
      m_strPortrait = pLicenseManager->GetUserPortrait() != NULL ? pLicenseManager->GetUserPortrait(): "";
      m_strUserName = pLicenseManager->GetUsername() != NULL ? pLicenseManager->GetUsername(): "";
      m_bLogin = true;
      bRet = true;
    }
    else
    {
      m_strErrorDescription = g_localizeStrings.Get(70006);
    }
  }
  return bRet;
}

void CVDMUserInfo::OnJobComplete(unsigned int jobID, bool success, CJob* job)
{
  CSingleLock lock(m_secUserInfo);

  CLoginJob* pLoginJob = (CLoginJob*)job;
  if( success )
  {
    bool bRet = ParseLicenseInfo( pLoginJob->GetResult() );
    if( jobID > 0 )
    {
      NotifyUser();
    }
    if( bRet )
    {
      m_strPassword = pLoginJob->GetPassword();
    }
    SaveSettingsUserInfo();
    CGUIMessage updatelogo(GUI_MSG_UPDATE_VIDON_LOGO, 0, WINDOW_HOME);
    g_windowManager.SendThreadMessage( updatelogo, WINDOW_HOME );
  }
  else
  {
    m_strErrorDescription = pLoginJob->GetErrorDescription();
  }

  if( jobID > 0 )
  {
    m_event.Set();
  }
}

void CVDMUserInfo::SaveSettingsUserInfo()
{
  if( this->IsLogin() )
  {
    CSettings::Get().SetString( "usercenter.name", m_strUserName );
    CSettings::Get().SetString( "usercenter.email", m_strEmail );
    if( NULL != m_pCurrentLicenseInfo )
    {
      CStdString strLicenseID = m_pCurrentLicenseInfo->lid;
      CSettings::Get().SetString( "usercenter.licenseid", strLicenseID );
      CDateTime endtime;
      endtime.SetFromUTCDateTime( m_pCurrentLicenseInfo->end );
      CStdString strEndTime = endtime.GetAsLocalizedDate();
      CSettings::Get().SetString( "usercenter.licensetime", strEndTime );
    }
    else
    {
      CSettings::Get().SetString( "usercenter.licenseid", g_localizeStrings.Get(70092) );
      CSettings::Get().SetString( "usercenter.licensetime", g_localizeStrings.Get(70092) );
    }
    CVDMUserInfo::Instance().SaveUsernameAndPassword( m_strUserName, m_strPassword );
  }
}

CStdString CVDMUserInfo::GetCurrentLicenseEndTime()
{
  CSingleLock lock(m_secUserInfo);
  CStdString strEndTime;
  if( NULL != m_pCurrentLicenseInfo )
  {
    CDateTime endtime;
    endtime.SetFromUTCDateTime( m_pCurrentLicenseInfo->end );
    strEndTime = endtime.GetAsLocalizedDate();
  }

  return strEndTime;
}

void CVDMUserInfo::NotifyUser()
{
  if( this->IsLogin() && !this->IsCurrentLicenseEmpty() )
  {
    if( this->IsCurrentLicenseExpired() )
    {
      //CGUIDialogKaiToast::QueueNotification( g_localizeStrings.Get(70018), g_localizeStrings.Get(70018) );
    }
    else if( this->IsCurrentLicenseAvailable() )
    {
      CDateTime endtime;
      endtime.SetFromUTCDateTime( m_pCurrentLicenseInfo->end );
      const CDateTime now = CDateTime::GetUTCDateTime();
      CDateTimeSpan span = endtime - now;
      if( endtime > now && span.GetDays() < 7 )  //will be expired.
      {
        CStdString strTip = StringUtils::Format(g_localizeStrings.Get(70017).c_str(), endtime.GetAsLocalizedDate().c_str());
        CGUIDialogKaiToast::QueueNotification( CGUIDialogKaiToast::Info, g_localizeStrings.Get(70018), strTip );
      }
    }
    else
    {
      //do nothing
    }
  }
}


#endif //if defined(HAS_VIDONME)