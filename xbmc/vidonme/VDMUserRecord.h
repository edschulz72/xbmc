#if defined(HAS_VIDONME)

#pragma once

#include <vector>
#include <set>
#include "XBDateTime.h"
#include "utils/Job.h"

struct RecordInfo
{
	std::string		strFilePath;
	std::string		strUploaded;
};
typedef std::vector<RecordInfo>		RecordInfoVec;

struct AdvanceFeatureRecord
{
	std::string		strFeatureType;
	std::string		strUsingTime;
};
typedef std::vector<AdvanceFeatureRecord>	AdvanceFeatureRecordVec;

class CVDMUserRecord : public IJobCallback
{
public:
	static CVDMUserRecord& Instance();

	void SetVersion(const std::string& strVersion);

	void SetUserLogin(bool bLogin);
	void SetUserName(const std::string& strUserName);
	void SetUserEmail(const std::string& strUserEmail);
	void SetUserLanguage(const std::string& strLanguage);
	void SetUserIP(const std::string& strIP);
	void SetUserMacAdress(const std::string& strMacAdress);
	void SetBoxType(const std::string& strBoxType);
	void SetFirstRun(bool bFirstRun);

	void SetStartTime(const CDateTime& dtStartTime);
	void SetEndTime(const CDateTime& dtEndTime);

	void AddAdvanceFeatureUseRecord(const std::string& strType, const std::string& strTime);

	void SaveRecord(bool bUpload = false);
	void UploadRecord(std::string strUploadFile = "");
	void RemoveRecords(void);

	void OnJobComplete(unsigned int jobID, bool success, CJob* job);

private:
	CVDMUserRecord();
	~CVDMUserRecord();

	void LoadRecordInfo(void);
	void SaveReocrdInfo(void);


private:
	std::string			m_strVersion;

	bool						m_bRegisted;
	std::string			m_strName;
	std::string			m_strEmail;
	std::string			m_strLanguage;
	std::string			m_strIP;
	std::string			m_strMacAdress;
	std::string			m_strBoxType;
	bool						m_bFirstRun;

	CDateTime				m_dtStartTime;
	CDateTime				m_dtEndTime;

	AdvanceFeatureRecordVec m_vecFeatures;

	RecordInfoVec		m_vecRecordInfo;
	std::string			m_strAllRecordPath;
	std::string			m_strTimeRecordPath;

	std::set<unsigned int>		m_setJobID;
};

#endif