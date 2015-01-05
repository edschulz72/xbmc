#ifndef __LIBVIDONUTILS_H__

#define __LIBVIDONUTILS_H__

#ifdef DLL_LIBVIDONUTILS_EXPORTS
#define DLL_LIBVIDONUTILS_API __declspec(dllexport)
#else
#define DLL_LIBVIDONUTILS_API
#endif

#include <string>
#include <vector>
#include "CommonHeader.h"
#ifdef __cplusplus
extern "C" {
#endif

namespace libVidonUtils
{
  struct License;
  struct LibVidonUtilsConfig
  {
     char userdatapath[VIDON_UTILS_MAX_PATH];
     char bindatapath[VIDON_UTILS_MAX_PATH];

     LibVidonUtilsConfig()
     {
        memset( userdatapath, 0, sizeof(userdatapath)/sizeof(char) );
        memset( bindatapath, 0, sizeof(bindatapath)/sizeof(char) );

     }
  };

  struct CurrentLicense
  {
    char lid[VIDON_UTILS_MAX_PATH];
    char chipid[VIDON_UTILS_MAX_PATH];
    int64_t end;

    CurrentLicenseStatus status;
    CurrentLicenseType   type;
    CurrentLicense()
    {
      memset( lid, 0, sizeof(lid)/sizeof(char) );
      memset( chipid, 0, sizeof(chipid)/sizeof(char) );
      end = 0;
      status = CurrentLicenseStatus_sUnknown;
      type = CurrentLicenseType_sUnknown;
    }
  };

  struct OtherLicense
  {
    char lid[VIDON_UTILS_MAX_PATH];
    int64_t end;
    OtherLicenseStatus status;
    OtherLicense()
    {
      memset( lid, 0, sizeof(lid)/sizeof(char) );
      end = 0;
      status = OtherLicenseStatus_sUnbound;
    }
  };

  struct Code
  {
    char cid[VIDON_UTILS_MAX_PATH];
    char key[VIDON_UTILS_MAX_PATH];

    int month;  
    int day;    
    CodeStatus status;
    Code()
    {
      memset( cid, 0, sizeof(cid)/sizeof(char) );
      memset( key, 0, sizeof(key)/sizeof(char) );
      month = 0;
      day = 0;
      status = CodeStatus_sUnused;
    }
  };

  class LicenseManager
  {
  public:
    LicenseManager();
    virtual ~LicenseManager();

    virtual bool ConnectToServer( const char* username, const char* password );
    virtual ErrorCode GetConnectErrorCode();
    virtual const char* GetUserEmail();
    virtual const char* GetUserPortrait();
    virtual const char* GetUserName();

    virtual int GetCurrentLicenseCount();
    virtual CurrentLicense GetCurrentLicense( int index );

    virtual bool IsFunctionAvailable( FunctionID funcid, const char* lid );

    virtual int GetOtherLicenseCount();
    virtual OtherLicense GetOtherLicense( int index );

    virtual int GetCodeCount();
    virtual Code GetCode( int index );

  private:

    License* m_pLicense;

  };
}
  DLL_LIBVIDONUTILS_API bool LibVidonUtilsInit( libVidonUtils::LibVidonUtilsConfig* pConfig );
  DLL_LIBVIDONUTILS_API void LibVidonUtilsDeInit();

  DLL_LIBVIDONUTILS_API libVidonUtils::LicenseManager* CreateLicenseManager();
  DLL_LIBVIDONUTILS_API void DestroyLicenseManager( libVidonUtils::LicenseManager* pLicenseManager );

  DLL_LIBVIDONUTILS_API bool SetByLid( const char* username, const char* password, const char* lid, /*OUT*/ int& nResult);
  DLL_LIBVIDONUTILS_API bool SetByCid( const char* username, const char* password, const char* cid, const char* lid/* optional*/, /*OUT*/ int& nResult);
  DLL_LIBVIDONUTILS_API bool SetByKey( const char* username,const char* password, const char* key, const char* lid/* optional*/, /*OUT*/ int& nResult);

  DLL_LIBVIDONUTILS_API bool SetCountryCode( int code );
  DLL_LIBVIDONUTILS_API int GetCountryCode();


  typedef void* VidonCheckVersionHandle;
  DLL_LIBVIDONUTILS_API VidonCheckVersionHandle Vidon_CreateCheckVersionHandle();
  DLL_LIBVIDONUTILS_API void Vidon_DestroyCheckVersionHandle( VidonCheckVersionHandle hVersion );
  DLL_LIBVIDONUTILS_API bool Vidon_CheckVersion( VidonCheckVersionHandle hVersion, const char* clienttype, const char* currentversion, const char* lang );
  DLL_LIBVIDONUTILS_API bool Vidon_HasNewVersion( VidonCheckVersionHandle hVersion );
  DLL_LIBVIDONUTILS_API bool Vidon_IsBetaVersion( VidonCheckVersionHandle hVersion );
  DLL_LIBVIDONUTILS_API int Vidon_GetVersionCode( VidonCheckVersionHandle hVersion );
  DLL_LIBVIDONUTILS_API const char* Vidon_GetVersionName( VidonCheckVersionHandle hVersion );
  DLL_LIBVIDONUTILS_API const char* Vidon_GetChangeLog( VidonCheckVersionHandle hVersion );
  DLL_LIBVIDONUTILS_API const char* Vidon_GetDownloadUrl( VidonCheckVersionHandle hVersion );
  DLL_LIBVIDONUTILS_API int64_t Vidon_GetFileSize( VidonCheckVersionHandle hVersion );
  DLL_LIBVIDONUTILS_API const char* Vidon_GetFileHashValue( VidonCheckVersionHandle hVersion );

  typedef void* VidonUploadReportHandle;
  DLL_LIBVIDONUTILS_API VidonUploadReportHandle Vidon_CreateUploadReportHandle();
  DLL_LIBVIDONUTILS_API void Vidon_DestroyUploadReportHandle( VidonUploadReportHandle hReport );
  DLL_LIBVIDONUTILS_API bool Vidon_SetUploadStringParam( VidonUploadReportHandle hReport, int nParam, const char* value );
  DLL_LIBVIDONUTILS_API bool Vidon_PushUploadFileParam( VidonUploadReportHandle hReport, const char* filepath );
  DLL_LIBVIDONUTILS_API void Vidon_ClearParams( VidonUploadReportHandle hReport );
  DLL_LIBVIDONUTILS_API bool Vidon_UploadReport( VidonUploadReportHandle hReport );


#ifdef __cplusplus
}
#endif


#endif //ifndef __LIBVIDONUTILS_H__