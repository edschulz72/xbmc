
#ifdef HAS_VIDONME

#pragma once

#include "SynchroMethod.h"

class Method_LogReportUpload: public MethodBase
{
public:
    virtual bool MethodCall();
    virtual bool Cancel(){ return false; };
    Method_LogReportUpload(const std::string& strUserID, const std::string& strUserEmail)
        : m_strUserID(strUserID), m_strUserEmail(strUserEmail) {}
private:
    std::string m_strUserID;
    std::string m_strUserEmail;
private:
    static bool Upload(const std::string& strUserID, const std::string& strUserEmail);
};

#endif