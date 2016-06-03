#ifdef HAS_VIDONME

#pragma once

#include "threads/SingleLock.h"
#include "threads/Event.h"
#include "utils/Job.h"
#include "libVidonUtils.h"
#include <vector>

class CVDMUserInfo:public IJobCallback
{
public:
  static CVDMUserInfo& Instance();

  bool GetUsernameAndPassword( std::string& strUserName, std::string& strPassword );
  bool SaveUsernameAndPassword( const std::string& strUserName, const std::string& strPassword );

  libVidonUtils::CountryCode GetCountryCode();
  void  SetCountryCode( libVidonUtils::CountryCode code );

  bool IsLogin();

  bool DoLogin( std::string strUserName = "", std::string strPassword = "", bool bWait = true );

  std::string GetLoginErrorDescription();

  bool IsCurrentLicenseExpired();

  bool IsCurrentLicenseEmpty();

  bool IsCurrentLicenseAvailable();

  bool IsCurrentLicenseTrial();

  std::string GetCurrentLicenseID();

  std::string GetCurrentLicenseEndTime();

  bool IsFunctionAvailable( const std::string& strFuncID );

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

  bool LoadUserInfoFromFile( const std::string& filepath );

  void SaveSettingsUserInfo();

  CCriticalSection                   m_secUserInfo;
  std::string                        m_strErrorDescription;
  libVidonUtils::CurrentLicense*     m_pCurrentLicenseInfo;
  std::vector<libVidonUtils::Code>   m_vecCode;
  std::string                        m_strEmail;
  std::string                        m_strPortrait;
  bool                               m_bLogin;
  std::string                        m_strUserName;
  std::string                        m_strPassword;
  libVidonUtils::CountryCode         m_eContryCode;
  int                                m_nlocalLicenseType;
  CEvent                             m_event;
};

#endif