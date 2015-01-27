#if defined(HAS_VIDONME)

#pragma once

#include "threads/SingleLock.h"
#include "threads/Event.h"
#include "utils/StdString.h"
#include "utils/Job.h"
#include "libVidonUtils.h"
#include <vector>

class CVDMUserInfo:public IJobCallback
{
public:
  static CVDMUserInfo& Instance();

  bool GetUsernameAndPassword( CStdString& strUserName, CStdString& strPassword );
  bool SaveUsernameAndPassword( const CStdString& strUserName, const CStdString& strPassword );

  libVidonUtils::CountryCode GetCountryCode();
  void  SetCountryCode( libVidonUtils::CountryCode code );

  bool IsLogin();

  bool DoLogin( CStdString strUserName = "", CStdString strPassword = "", bool bWait = true );

  CStdString GetLoginErrorDescription();

  bool IsCurrentLicenseExpired();

  bool IsCurrentLicenseEmpty();

  bool IsCurrentLicenseAvailable();

  bool IsCurrentLicenseTrial();

  CStdString GetCurrentLicenseID();

  CStdString GetCurrentLicenseEndTime();

  bool IsFunctionAvailable( const CStdString& strFuncID );

  bool GetAvailableCode( std::vector<libVidonUtils::Code>& vecCode );

  bool MaybeProVersion();

  void WaitLoginInBackground( bool bShowUI = true );

private:
  CVDMUserInfo();
  //CVDMUserInfo(const CVDMUserInfo& other){};
  //CVDMUserInfo& operator = (const CVDMUserInfo& other){};

  virtual void OnJobComplete(unsigned int jobID, bool success, CJob* job);

  bool ParseLicenseInfo( libVidonUtils::LicenseManager* pLicenseManager );

  void NotifyUser();

  bool LoadUserInfoFromFile( const CStdString& filepath );

  void SaveSettingsUserInfo();

  CCriticalSection                   m_secUserInfo;
  CStdString                         m_strErrorDescription;
  libVidonUtils::CurrentLicense*     m_pCurrentLicenseInfo;
  std::vector<libVidonUtils::Code>   m_vecCode;
  CStdString                         m_strEmail;
  CStdString                         m_strPortrait;
  bool                               m_bLogin;
  CStdString                         m_strUserName;
  CStdString                         m_strPassword;
  libVidonUtils::CountryCode         m_eContryCode;
  int                                m_nlocalLicenseType;
  CEvent                             m_event;
};

#endif //if defined(HAS_VIDONME)