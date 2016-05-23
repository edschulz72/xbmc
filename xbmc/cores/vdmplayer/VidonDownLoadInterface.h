#ifndef __VIDONDOWNLOADINTERFACE_H__
#define __VIDONDOWNLOADINTERFACE_H__
#include "VidonCommon.h"

#if defined(_WIN32) && defined(_WINDLL)
#define	DLL_DOWNLOAD_API __declspec(dllexport)
#else
#define DLL_DOWNLOAD_API
#endif

namespace libvp
{


 enum DOWNLOAD_ERROR_CODE
  {
	EC_UNKNOWNERROR = -100,
    EC_CONNECTFAILED,
	EC_FILEOPFAILD,
	EC_FILEDISKNOTENOUGH,
	EC_CREATDOWNLOADFAILED,
	EC_OK = 0,
  };
  
  class IDownloadCallback
  {
  public:  
    virtual ~IDownloadCallback() {};
    virtual void OnDownloadStarted() = 0;
    virtual void OnDownloadStoped()  = 0;

    virtual void OnDownloadPaused()  = 0;
    virtual void OnDownloadResumed() = 0;

    virtual void OnDownloadFailed(DOWNLOAD_ERROR_CODE errodcode) = 0;
    virtual void OnDownloadFinished() = 0;
  };
  
  class IVidonDownloadInterface
  {
  public:
    virtual bool Init(vidonPlayerParam* pinputInfo) = 0;
    virtual bool Start() = 0;
    virtual void Stop() = 0;
    
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    
    virtual double GetSpeed() = 0;
    virtual double GetRemanenTime() = 0;
    
    virtual void GetFileSize(int64_t &nFilesize) = 0;
    virtual void GetProgress(int64_t &nDonesize) = 0;
	virtual void GetConvertpercent(int &nPercent) = 0;
	virtual void GetExtension(const char* pExtension) = 0;
  };
#ifdef __cplusplus
extern "C" {
#endif


DLL_DOWNLOAD_API IVidonDownloadInterface*  CreateDownload(IDownloadCallback* pDownloadCallback);

DLL_DOWNLOAD_API void      ReleaseDownload( IVidonDownloadInterface* pDownload );


#ifdef __cplusplus
}
#endif
}
#endif 