#pragma once

#include "SynchroMethod.h"

class Method_LogReportUpload: public MethodBase
{
public:
    virtual bool MethodCall();
    virtual bool Cancel(){ return false; };
    Method_LogReportUpload(const CStdString& strUserID, const CStdString& strUserEmail)
        : m_strUserID(strUserID), m_strUserEmail(strUserEmail) {}
private:
    CStdString m_strUserID;
    CStdString m_strUserEmail;
private:
    static bool Upload(const CStdString& strUserID, const CStdString& strUserEmail);

};
