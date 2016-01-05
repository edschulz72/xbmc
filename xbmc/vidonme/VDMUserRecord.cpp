#if defined(HAS_VIDONME)

#include "VDMUserRecord.h"
#include "utils/XBMCTinyXML.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "filesystem/File.h"
#include "filesystem/SpecialProtocol.h"
#include "settings/AdvancedSettings.h"
#include "vidonme/VDMLogReportUpload.h"

class CUploadRecordJob : public CJob
{
public:
	CUploadRecordJob(std::string strName, std::string strEmail, RecordInfoVec& vecRecordInfo, std::string strUploadFile)
	{
		m_strName = strName;
		m_strEmail = strEmail;
		m_strUploadFile = strUploadFile;
		m_vecRecordInfo = vecRecordInfo;
	}

	void GetRecordInfo(RecordInfoVec& vecRecordInfo)
	{
		vecRecordInfo = m_vecRecordInfo;
	}

	virtual bool DoWork()
	{
		for (size_t i = 0; i < m_vecRecordInfo.size(); i++)
		{
			std::string strFilePath = m_vecRecordInfo[i].strFilePath;

			if (!XFILE::CFile::Exists(strFilePath) || m_vecRecordInfo[i].strUploaded == "true")
			{
				continue;
			}

			if (m_strUploadFile.empty() || m_strUploadFile == strFilePath)
			{
				Method_UserRecordUpload* LogUpload = new Method_UserRecordUpload(m_strName, m_strEmail, strFilePath);
				bool bRet = SynchroMethodCall(std::shared_ptr<MethodPtr>(new MethodPtr(LogUpload)));
				m_vecRecordInfo[i].strUploaded = bRet ? "true" : "false";
			}
		}

		return true;
	}

private:
	std::string m_strName;
	std::string m_strEmail;
	std::string	m_strUploadFile;
	RecordInfoVec		m_vecRecordInfo;
};

CVDMUserRecord& CVDMUserRecord::Instance()
{
	static CVDMUserRecord s_userRecord;

	return s_userRecord;
}

CVDMUserRecord::CVDMUserRecord()
{
	m_setJobID.clear();
	LoadRecordInfo();
}

CVDMUserRecord::~CVDMUserRecord()
{
	SaveReocrdInfo();
}

void CVDMUserRecord::SetVersion(const std::string& strVersion)
{
	m_strVersion = strVersion;
}

void CVDMUserRecord::SetUserLogin(bool bLogin)
{
	m_bRegisted = bLogin;
}

void CVDMUserRecord::SetUserName(const std::string& strUserName)
{
	m_strName = strUserName;
}

void CVDMUserRecord::SetUserEmail(const std::string& strUserEmail)
{
	m_strEmail = strUserEmail;
}

void CVDMUserRecord::SetUserLanguage(const std::string& strLanguage)
{
	m_strLanguage = strLanguage;
}

void CVDMUserRecord::SetUserIP(const std::string& strIP)
{
	m_strIP = strIP;
}

void CVDMUserRecord::SetUserMacAdress(const std::string& strMacAdress)
{
	m_strMacAdress = strMacAdress;
}

void CVDMUserRecord::SetBoxType(const std::string& strBoxType)
{
	m_strBoxType = strBoxType;
}

void CVDMUserRecord::SetFirstRun(bool bFirstRun)
{
	m_bFirstRun = bFirstRun;
}

void CVDMUserRecord::SetStartTime(const CDateTime& dtStartTime)
{
	m_dtStartTime		= dtStartTime;

	RecordInfo record;
	m_strAllRecordPath = URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), StringUtils::CreateUUID());
	record.strFilePath = m_strAllRecordPath;
	record.strUploaded = "false";
	m_vecRecordInfo.push_back(record);


	m_strTimeRecordPath = URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), StringUtils::CreateUUID());
	record.strFilePath = m_strTimeRecordPath;
	record.strUploaded = "false";
	m_vecRecordInfo.push_back(record);
}

void CVDMUserRecord::SetEndTime(const CDateTime& dtEndTime)
{
	m_dtEndTime = dtEndTime;
	SaveReocrdInfo();
}

void CVDMUserRecord::AddAdvanceFeatureUseRecord(const std::string& strType, const std::string& strTime)
{
	AdvanceFeatureRecord stFeature;
	stFeature.strFeatureType	= strType;
	stFeature.strUsingTime		= strTime;

	m_vecFeatures.push_back(stFeature);
}

void CVDMUserRecord::SaveRecord(bool bUpload)
{
	CXBMCTinyXML xmlDoc;
	
	TiXmlElement elementXBMC("VidOnXBMC");
	elementXBMC.SetAttribute("Version", m_strVersion.c_str());

	TiXmlElement elementUser("UserInfo");
	if (m_bRegisted)
	{
		elementUser.SetAttribute("IsRegisted", "true");
	}
	else
	{
		elementUser.SetAttribute("IsRegisted", "false");
	}
	elementUser.SetAttribute("Name", m_strName.c_str());
	elementUser.SetAttribute("Email", m_strEmail.c_str());
	elementUser.SetAttribute("Language", m_strLanguage.c_str());
	elementUser.SetAttribute("IP", m_strIP.c_str());
	elementUser.SetAttribute("Mac", m_strMacAdress.c_str());
	elementUser.SetAttribute("BoxType", m_strBoxType.c_str());
	if (m_bFirstRun)
	{
		elementUser.SetAttribute("FirstRun", "true");
	}
	else
	{
		elementUser.SetAttribute("FirstRun", "false");
	}
	elementXBMC.InsertEndChild(elementUser);

	TiXmlElement elementWorking("WorkingRecord");
	elementWorking.SetAttribute("StartTime", m_dtStartTime.GetAsDBDateTime().c_str());
	elementWorking.SetAttribute("EndTime", "");
	elementWorking.SetAttribute("WorkTime", "");

	if (m_dtEndTime > m_dtStartTime)
	{
		std::string strEndTime;
		strEndTime = m_dtEndTime.GetAsDBDateTime();
		elementWorking.SetAttribute("EndTime", strEndTime.c_str());
		CDateTimeSpan dtsWork = m_dtEndTime - m_dtStartTime;
		elementWorking.SetAttribute("WorkTime", dtsWork.GetSecondsTotal());
	}

	elementXBMC.InsertEndChild(elementWorking);

	TiXmlElement elementFeatures("AdvanceFeature");
	for (size_t i = 0; i < m_vecFeatures.size(); i++)
	{
		TiXmlElement elementFeature("Feature");
		elementFeature.SetAttribute("Type", m_vecFeatures[i].strFeatureType.c_str());
		elementFeature.SetAttribute("StartTime", m_vecFeatures[i].strUsingTime.c_str());
		elementFeatures.InsertEndChild(elementFeature);
	}

	if (!bUpload)
	{
		elementXBMC.InsertEndChild(elementFeatures);
	}

	xmlDoc.InsertEndChild(elementXBMC);

	if (bUpload)
	{
		xmlDoc.SaveFile(m_strTimeRecordPath.c_str());
		UploadRecord(m_strTimeRecordPath);
	}
	else
	{
		xmlDoc.SaveFile(m_strAllRecordPath.c_str());
	}
}

void CVDMUserRecord::UploadRecord(std::string strUploadFile)
{
	unsigned int nJobID = CJobManager::GetInstance().AddJob(new CUploadRecordJob(m_strName, m_strEmail, m_vecRecordInfo, strUploadFile), this);

	m_setJobID.insert(nJobID);
}

void CVDMUserRecord::RemoveRecords(void)
{
	RecordInfoVec vecTemp;

	for (size_t i = 0; i < m_vecRecordInfo.size(); i++)
	{
		std::string strFilePath = m_vecRecordInfo[i].strFilePath;

		if (m_vecRecordInfo[i].strUploaded == "true" && XFILE::CFile::Exists(strFilePath))
		{
			if (XFILE::CFile::Delete(strFilePath))
			{
				continue;
			}
		}

		vecTemp.push_back(m_vecRecordInfo[i]);
	}

	m_vecRecordInfo.swap(vecTemp);
}

void CVDMUserRecord::LoadRecordInfo(void)
{
	m_vecRecordInfo.clear();

	std::string strPath;
	CXBMCTinyXML doc;
	
	strPath = URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "RecordInfo");

	if (!doc.LoadFile(strPath))
	{
		return;
	}

	TiXmlElement *root = doc.RootElement();
	if (!root || strcmp(root->Value(), "RecordInfo"))
	{
		return;
	}

	TiXmlElement *record = root->FirstChildElement("Record");
	while (record)
	{
		RecordInfo nodeRecord;

		nodeRecord.strFilePath = record->Attribute("FilePath");
		nodeRecord.strUploaded = record->Attribute("Uploaded");

		m_vecRecordInfo.push_back(nodeRecord);

		record = record->NextSiblingElement("Record");
	}
}

void CVDMUserRecord::SaveReocrdInfo(void)
{
	std::string strPath;
	CXBMCTinyXML doc;

	strPath = URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "RecordInfo");

	TiXmlElement root("RecordInfo");

	for (size_t i = 0; i < m_vecRecordInfo.size(); ++i)
	{
		TiXmlElement record("Record");

		record.SetAttribute("FilePath", m_vecRecordInfo[i].strFilePath.c_str());
		record.SetAttribute("Uploaded", m_vecRecordInfo[i].strUploaded.c_str());

		root.InsertEndChild(record);
	}

	doc.InsertEndChild(root);
	doc.SaveFile(strPath.c_str());
}

void CVDMUserRecord::OnJobComplete(unsigned int jobID, bool success, CJob* job)
{
	if (m_setJobID.find(jobID) != m_setJobID.end())
	{
		m_setJobID.erase(jobID);

		RecordInfoVec		vecRecordInfo;
		CUploadRecordJob* pJob = (CUploadRecordJob*)job;
		pJob->GetRecordInfo(vecRecordInfo);

		for (int i = 0; i < m_vecRecordInfo.size(); ++i)
		{
			if (i < vecRecordInfo.size() && m_vecRecordInfo[i].strFilePath == vecRecordInfo[i].strFilePath)
			{
				m_vecRecordInfo[i].strUploaded = vecRecordInfo[i].strUploaded;
			}
		}

		RemoveRecords();

		SaveReocrdInfo();
	}
}

#endif