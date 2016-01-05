
#ifdef HAS_VIDONME

#pragma once

#include <vector>
#include <set>
#include "windows/GUIMediaWindow.h"
#include "utils/FileOperationJob.h"

struct AddonRepo
{
	std::string		strCountryCode;
	std::string		strPath;
};

typedef	std::vector<AddonRepo>	AddonRepoVec;

class CVDMWindowRecommend : public CGUIMediaWindow, public IJobCallback
{
public:
	CVDMWindowRecommend(void);
	virtual ~CVDMWindowRecommend(void);

	void GetRecommendedAddons(void);
	bool InstallAddons(std::string strCountryCode);

	static void SetCanActive(bool bCan);

	static bool CanActive(void);
	static bool AddonInstalled(void);


	virtual void OnInitWindow();

	void OnJobComplete(unsigned int jobID, bool success, CJob* job);

protected:
	virtual bool Update(const std::string &strDirectory, bool updateFilterPath = true);
	virtual bool OnClick(int iItem);
	virtual bool OnMessage(CGUIMessage& message);

private:
	bool LoadAddonInstallInfo(void);
	bool SaveAddonInstallInfo(void);

private:
	std::string		m_strCountryCode;
	std::string		m_strMapPath;
	AddonRepoVec	m_vecAddons;

	bool					m_bMapLoaded;

	unsigned int		m_nGetMapJobID;
	std::set<unsigned int>		m_setInstallJobID;
};

class CGetAddonMapJob : public CJob
{
public:
	CGetAddonMapJob(void);

	void GetAddonInfo(std::string& strCountryCode, std::string& strMapPath, AddonRepoVec& vecAddons);

	virtual bool DoWork(void);

private:
	bool GetAddonMap(void);

private:
	std::string		m_strCountryCode;
	std::string		m_strMapPath;
	AddonRepoVec	m_vecAddons;
};

#endif