#pragma once

#include "utils/FileOperationJob.h"
#include "addons/Addon.h"
#include "utils/Stopwatch.h"
#include "threads/Event.h"
#include "utils/GlobalsHandling.h"
#include "SynchroMethod.h"

class CVDMVersionInfo
{
public:
    enum State
    {
        sUpdateNone,      // 无升级
        sUpdateAvail,       // 有升级;
        //sUpdateMust,        // 必须升级
    };
    State state;
    bool bIsBeta;
    int nVersionCode;
    std::string strVersionName;
    std::string strChangeLog;

    std::string strUrl;
    int64_t     nSize;
    std::string strHash;
};

class CVDMVersionCheck
{
public:
    static std::string GetCurrVersionCode();
    static std::string GetCurrVersionName();
    static bool VersionCheck(OUT CVDMVersionInfo& info);
    static bool ParseVersionInfo(const std::string strInfo, OUT CVDMVersionInfo& info);
    static std::string GetCurrVersionTime();
};

class CVDMVersionUpdateJob : public CFileOperationJob
{
public:

    CVDMVersionUpdateJob( const CVDMVersionInfo& info )
        : m_info(info)
    {

    }
    virtual bool DoWork();
    bool DownloadPackage(const CStdString& path, const CStdString& dest);
    bool Install(const CStdString& package);
private:
    void ReportValidateError(const CStdString& fileName);
    void ReportDownloadError(const CStdString& fileName);
    CVDMVersionInfo m_info;
};

class CVDMVersionUpdate : public IJobCallback
{
public:
    CCriticalSection m_mutex;
    unsigned int m_nJobID;
    unsigned int m_nProgress;

    unsigned int m_nJobIDCheckVersionInBackground;

    void OnJobComplete(unsigned int jobID, bool success, CJob* job);
    void OnJobProgress(unsigned int jobID, unsigned int progress, unsigned int total, const CJob *job);
public:
    bool IsCheckInBackground();
    bool CheckVersionInBackground();
    void CancekCheckVersionInBackground();

    void Start(const CVDMVersionInfo& info);
    bool Stop();
public:
    CVDMVersionUpdate();
    ~CVDMVersionUpdate();
    bool running() const { return m_nJobID != 0;}
    unsigned int progress() const { return m_nProgress;}
};

XBMC_GLOBAL_REF(CVDMVersionUpdate,g_vdmVersionUpdate);
#define g_vdmVersionUpdate XBMC_GLOBAL_USE(CVDMVersionUpdate)

