#if defined(HAS_VIDONME)

#ifndef __DLLVIDONUTILS_H__
#define __DLLVIDONUTILS_H__

#include "system.h"
#include "DynamicDll.h"
#include "libVidonUtils.h"


#if defined( TARGET_WINDOWS )
#define DLL_PATH_LIBVIDONUTILS "special://xbmcbin/system/libVidonUtils.dll"
#elif defined( TARGET_ANDROID )
#define DLL_PATH_LIBVIDONUTILS "libvidonutils.so"
#else
#error NO Support Other OS
#endif

class CVidonPlayerManager;
class DLLVidonUtilsInterface
{
public:

  virtual ~DLLVidonUtilsInterface(){};

  virtual bool LibVidonUtilsInit( libVidonUtils::LibVidonUtilsConfig* pConfig ) = 0;
  virtual void LibVidonUtilsDeInit()=0;

  virtual libVidonUtils::LicenseManager* CreateLicenseManager()=0;
  virtual void DestroyLicenseManager( libVidonUtils::LicenseManager* pLicenseManager )=0;

  virtual bool SetByLid( const char* username, const char* password, const char* lid, /*OUT*/ int& nResult)=0;
  virtual bool SetByCid( const char* username, const char* password, const char* cid, const char* lid/* optional*/, /*OUT*/ int& nResult)=0;
  virtual bool SetByKey( const char* username,const char* password, const char* key, const char* lid/* optional*/, /*OUT*/ int& nResult)=0;
  virtual bool SetCountryCode( int code )=0;
  virtual int GetCountryCode()=0;

  virtual VidonCheckVersionHandle Vidon_CreateCheckVersionHandle() = 0;
  virtual void Vidon_DestroyCheckVersionHandle( VidonCheckVersionHandle hVersion ) = 0;
  virtual bool Vidon_CheckVersion( VidonCheckVersionHandle hVersion, const char* clienttype, const char* currentversion, const char* lang ) = 0;
  virtual bool Vidon_HasNewVersion( VidonCheckVersionHandle hVersion ) = 0;
  virtual bool Vidon_IsBetaVersion( VidonCheckVersionHandle hVersion ) = 0;
  virtual int Vidon_GetVersionCode( VidonCheckVersionHandle hVersion ) = 0;
  virtual const char* Vidon_GetVersionName( VidonCheckVersionHandle hVersion ) = 0;
  virtual const char* Vidon_GetChangeLog( VidonCheckVersionHandle hVersion ) = 0;
  virtual const char* Vidon_GetDownloadUrl( VidonCheckVersionHandle hVersion ) = 0;
  virtual int64_t Vidon_GetFileSize( VidonCheckVersionHandle hVersion ) = 0;
  virtual const char* Vidon_GetFileHashValue( VidonCheckVersionHandle hVersion ) = 0;

  virtual VidonUploadReportHandle Vidon_CreateUploadReportHandle() = 0;
  virtual void Vidon_DestroyUploadReportHandle( VidonUploadReportHandle hReport ) = 0;
  virtual bool Vidon_SetUploadStringParam( VidonUploadReportHandle hReport, int nParam, const char* value ) = 0;
  virtual bool Vidon_PushUploadFileParam( VidonUploadReportHandle hReport, const char* filepath ) = 0;
  virtual void Vidon_ClearParams( VidonUploadReportHandle hReport ) = 0;
  virtual bool Vidon_UploadReport( VidonUploadReportHandle hReport ) = 0;


  virtual CVidonPlayerManager* GetVidonPlayerManagerInstance() = 0;
};

class DLLVidonUtils:public DllDynamic, DLLVidonUtilsInterface
{
  DECLARE_DLL_WRAPPER(DLLVidonUtils, DLL_PATH_LIBVIDONUTILS)

  DEFINE_METHOD1( bool, LibVidonUtilsInit, ( libVidonUtils::LibVidonUtilsConfig* p1 ) )
  DEFINE_METHOD0( void, LibVidonUtilsDeInit )

  DEFINE_METHOD0( libVidonUtils::LicenseManager*, CreateLicenseManager )
  DEFINE_METHOD1( void, DestroyLicenseManager, ( libVidonUtils::LicenseManager* p1 ) )

  DEFINE_METHOD4( bool, SetByLid, ( const char* p1, const char* p2, const char* p3, int& p4) )
  DEFINE_METHOD5( bool, SetByCid, ( const char* p1, const char* p2, const char* p3, const char* p4, int& p5) )
  DEFINE_METHOD5( bool, SetByKey, ( const char* p1,const char* p2, const char* p3, const char* p4, int& p5) )
  DEFINE_METHOD1( bool, SetCountryCode, ( int p1 ) )
  DEFINE_METHOD0( int, GetCountryCode )

  DEFINE_METHOD0( VidonCheckVersionHandle, Vidon_CreateCheckVersionHandle )
  DEFINE_METHOD1( void, Vidon_DestroyCheckVersionHandle, ( VidonCheckVersionHandle p1 ) )
  DEFINE_METHOD4( bool, Vidon_CheckVersion, ( VidonCheckVersionHandle p1, const char* p2, const char* p3, const char* p4 ) )
  DEFINE_METHOD1( bool, Vidon_HasNewVersion, ( VidonCheckVersionHandle p1 ) )
  DEFINE_METHOD1( bool, Vidon_IsBetaVersion, ( VidonCheckVersionHandle p1 ) )
  DEFINE_METHOD1( int, Vidon_GetVersionCode, ( VidonCheckVersionHandle p1 ) )
  DEFINE_METHOD1( const char*, Vidon_GetVersionName, ( VidonCheckVersionHandle p1 ) )
  DEFINE_METHOD1( const char*, Vidon_GetChangeLog, ( VidonCheckVersionHandle p1 ) )
  DEFINE_METHOD1( const char*, Vidon_GetDownloadUrl, ( VidonCheckVersionHandle p1 ) )
  DEFINE_METHOD1( int64_t, Vidon_GetFileSize, ( VidonCheckVersionHandle p1 ) )
  DEFINE_METHOD1( const char*, Vidon_GetFileHashValue, ( VidonCheckVersionHandle p1 ) )

  DEFINE_METHOD0( VidonUploadReportHandle, Vidon_CreateUploadReportHandle )
  DEFINE_METHOD1( void, Vidon_DestroyUploadReportHandle, ( VidonUploadReportHandle p1 ) )
  DEFINE_METHOD3( bool, Vidon_SetUploadStringParam, ( VidonUploadReportHandle p1, int p2, const char* p3 ) )
  DEFINE_METHOD2( bool, Vidon_PushUploadFileParam, ( VidonUploadReportHandle p1, const char* p2 ) )
  DEFINE_METHOD1( void, Vidon_ClearParams, ( VidonUploadReportHandle p1 ) )
  DEFINE_METHOD1( bool, Vidon_UploadReport, ( VidonUploadReportHandle p1 ) )


  DEFINE_METHOD0( CVidonPlayerManager*, GetVidonPlayerManagerInstance )

  BEGIN_METHOD_RESOLVE()
  RESOLVE_METHOD(LibVidonUtilsInit)
  RESOLVE_METHOD(LibVidonUtilsDeInit)
  RESOLVE_METHOD(CreateLicenseManager)
  RESOLVE_METHOD(DestroyLicenseManager)
  RESOLVE_METHOD(SetByLid)
  RESOLVE_METHOD(SetByCid)
  RESOLVE_METHOD(SetByKey)
  RESOLVE_METHOD(SetCountryCode)
  RESOLVE_METHOD(GetCountryCode)

  RESOLVE_METHOD(Vidon_CreateCheckVersionHandle)
  RESOLVE_METHOD(Vidon_DestroyCheckVersionHandle)
  RESOLVE_METHOD(Vidon_CheckVersion)
  RESOLVE_METHOD(Vidon_HasNewVersion)
  RESOLVE_METHOD(Vidon_IsBetaVersion)
  RESOLVE_METHOD(Vidon_GetVersionCode)
  RESOLVE_METHOD(Vidon_GetVersionName)
  RESOLVE_METHOD(Vidon_GetChangeLog)
  RESOLVE_METHOD(Vidon_GetDownloadUrl)
  RESOLVE_METHOD(Vidon_GetFileSize)
  RESOLVE_METHOD(Vidon_GetFileHashValue)
  RESOLVE_METHOD(Vidon_CreateUploadReportHandle)
  RESOLVE_METHOD(Vidon_DestroyUploadReportHandle)
  RESOLVE_METHOD(Vidon_SetUploadStringParam)
  RESOLVE_METHOD(Vidon_PushUploadFileParam)
  RESOLVE_METHOD(Vidon_ClearParams)
  RESOLVE_METHOD(Vidon_UploadReport)

  RESOLVE_METHOD(GetVidonPlayerManagerInstance)

  END_METHOD_RESOLVE()

//public:    
//  virtual void Unload() {};

};

extern DLLVidonUtils g_DllVidonUtils;


#endif // ifndef __DLLVIDONUTILS_H__

#endif // if defined(HAS_VIDONME)