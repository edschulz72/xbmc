
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

class Method_UserRecordUpload : public MethodBase
{
public:
	virtual bool MethodCall();
	virtual bool Cancel(){ return false; };
	Method_UserRecordUpload(const std::string& strUserID, const std::string& strUserEmail, const std::string& strFilePath)
		: m_strUserID(strUserID), m_strUserEmail(strUserEmail), m_strFilePath(strFilePath){}
private:
	std::string m_strUserID;
	std::string m_strUserEmail;
	std::string m_strFilePath;
private:
	static bool Upload(const std::string& strUserID, const std::string& strUserEmail, const std::string& strFilePath);

};

#endif