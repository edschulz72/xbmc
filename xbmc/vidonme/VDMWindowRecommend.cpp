
#ifdef HAS_VIDONME

#include "VDMWindowRecommend.h"
#include "URL.h"
#include "FileItem.h"
#include "Util.h"
#include "GUIUserMessages.h"
#include "utils/URIUtils.h"
#include "utils/Job.h"
#include "utils/log.h"
#include "utils/JobManager.h"
#include "utils/JSONVariantParser.h"
#include "guilib/GUIWindowManager.h"
#include "interfaces/Builtins.h"
#include "filesystem/ZipFile.h"
#include "filesystem/ZipManager.h"
#include "filesystem/ZipDirectory.h"
#include "filesystem/SpecialProtocol.h"
#include "vidonme/DLLVidonUtils.h"
#include "addons/IAddon.h"
#include "addons/AddonManager.h"
#include "settings/AdvancedSettings.h"
#include "dialogs/GUIDialogYesNo.h"
#include "dialogs/GUIDialogKaiToast.h"

std::vector<std::string>	m_vecInstalled;
bool m_bCanActive;

CVDMWindowRecommend::CVDMWindowRecommend(void)
	: CGUIMediaWindow(VDM_WINDOW_RECOMMEND, "special://xbmc/media/VDMResource/Recommend.xml")
{
	m_bMapLoaded = false;
	m_bCanActive = true;

	AddonRepo		repo;

	repo.strCountryCode = "Asian";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/Asian.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "AU";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/Australian.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "Br";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/Brazilian.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "CA";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/Canadian.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "DE";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/DE.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "FR";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/French.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "Es";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/Spanish.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "TH";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/Thai.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "UK";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/UK.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "US";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/US1.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "US";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/US2.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "US";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/US3.zip";
	m_vecAddons.push_back(repo);

	repo.strCountryCode = "VN";
	repo.strPath = "https://raw.githubusercontent.com/Crazyaddon/AddOnRepo/master/Vietnamese.zip";
	m_vecAddons.push_back(repo);

	LoadAddonInstallInfo();

	GetRecommendedAddons();
}


CVDMWindowRecommend::~CVDMWindowRecommend(void)
{
	if (m_nGetMapJobID)
	{
		CJobManager::GetInstance().CancelJob(m_nGetMapJobID);
	}
}

void CVDMWindowRecommend::GetRecommendedAddons(void)
{
	m_nGetMapJobID = CJobManager::GetInstance().AddJob(new CGetAddonMapJob(), this, CJob::PRIORITY_HIGH);
}

void CVDMWindowRecommend::SetCanActive(bool bCan)
{
	m_bCanActive = bCan;
}

bool CVDMWindowRecommend::CanActive(void)
{
	return m_bCanActive;
}

bool CVDMWindowRecommend::AddonInstalled(void)
{
	return !m_vecInstalled.empty();
}

void CVDMWindowRecommend::OnInitWindow()
{
	if (!m_bMapLoaded)
	{
		GetRecommendedAddons();
	}

	m_bCanActive = false;

	return CGUIMediaWindow::OnInitWindow();
}

bool CVDMWindowRecommend::InstallAddons(std::string strCountryCode)
{
	m_strCountryCode = strCountryCode;
	std::string	strDest = CSpecialProtocol::TranslatePath("special://home/addons/packages/");

	std::string	strSource;

	CFileItemList list;
	for (size_t i = 0; i < m_vecAddons.size(); i++)
	{
		if (m_strCountryCode == m_vecAddons[i].strCountryCode)
		{
			strSource = m_vecAddons[i].strPath;

			CFileItemPtr pItem = CFileItemPtr(new CFileItem(strSource, false));
			pItem->Select(true);
			list.Add(pItem);
		}
	}
	
	CFileOperationJob* pJob(new CFileOperationJob(CFileOperationJob::ActionReplace, list, strDest, true));
	unsigned int nID = CJobManager::GetInstance().AddJob(pJob, this, CJob::PRIORITY_HIGH);
	m_setInstallJobID.insert(nID);

	return true;
}


void CVDMWindowRecommend::OnJobComplete(unsigned int jobID, bool success, CJob* job)
{
	if (m_nGetMapJobID == jobID)
	{
		m_nGetMapJobID = 0;

		if (success)
		{
			CGetAddonMapJob* pJob = (CGetAddonMapJob*)job;
			pJob->GetAddonInfo(m_strCountryCode, m_strMapPath, m_vecAddons);

			CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE);
			g_windowManager.SendThreadMessage(msg);

			m_bMapLoaded = true;
		}
	}
	else if (m_setInstallJobID.find(jobID) != m_setInstallJobID.end())
	{
		m_setInstallJobID.erase(jobID);
		if (success)
		{
			CGUIMessage msg(GUI_MSG_DOWNLOAD_SUCCESS, 0, 0);
			g_windowManager.SendThreadMessage(msg, VDM_WINDOW_RECOMMEND);

			CFileOperationJob* pJob = (CFileOperationJob*)job;

			bool bRet = true;
			std::string	strSource;
			CFileItemList itemList;
			itemList.Copy(pJob->GetItems());
			
			for (int i = 0; i < itemList.Size(); i++)
			{
				strSource = itemList.Get(i)->GetPath();
				strSource = URIUtils::GetFileName(strSource);
				strSource = URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath("special://home/addons/packages/").c_str(), strSource);
				std::string strDest = CSpecialProtocol::TranslatePath("special://home/addons/");
				bRet = bRet && g_ZipManager.ExtractArchive(strSource, strDest, true, g_localizeStrings.Get(70072));
			}

			if (bRet)
			{
				CGUIDialogKaiToast::QueueNotification(g_localizeStrings.Get(70131), g_localizeStrings.Get(70135));

				ADDON::CAddonMgr::Get().FindAddons();
				m_vecInstalled.push_back(m_strCountryCode);

				CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE);
				g_windowManager.SendThreadMessage(msg);

				SaveAddonInstallInfo();
			}
			else
			{
				CGUIDialogKaiToast::QueueNotification(g_localizeStrings.Get(70131), g_localizeStrings.Get(70136));
				CLog::Log(LOGERROR, "Decompress addon repository failed.");
			}
		}
		else
		{
			CGUIDialogKaiToast::QueueNotification(g_localizeStrings.Get(70131), g_localizeStrings.Get(70134));
			CLog::Log(LOGERROR, "Download addon repository failed.");
		}
	}
}

bool CVDMWindowRecommend::Update(const std::string &strDirectory, bool updateFilterPath)
{
	ClearFileItems();

	CFileItemPtr pItem(new CFileItem());
	pItem->SetLabel("..");
	pItem->SetIconImage("DefaultFolderBack.png");
	pItem->SetLabelPreformated(true);
	pItem->m_bIsFolder = true;
	m_vecItems->Add(pItem);

	int nSelect = 0;
	std::string strCode;

	for (size_t i = 0; i < m_vecAddons.size(); i++)
	{
		if (m_vecAddons[i].strCountryCode == strCode)
		{
			continue;
		}

		strCode = m_vecAddons[i].strCountryCode;

		CFileItemPtr item(new CFileItem());
		item->SetLabel(m_vecAddons[i].strCountryCode);
		item->SetIconImage(URIUtils::GetParentPath(m_vecAddons[i].strPath) + m_vecAddons[i].strCountryCode.c_str() + ".png");
		item->SetLabelPreformated(true);
		item->m_bIsFolder = true;

		for (size_t j = 0; j < m_vecInstalled.size(); ++j)
		{
			if (m_vecAddons[i].strCountryCode == m_vecInstalled[j])
			{
				item->Select(true);
				break;
			}
		}
		
		if (m_vecAddons[i].strCountryCode == m_strCountryCode)
		{
			nSelect = m_vecItems->Size();
		}

		m_vecItems->Add(item);
	}

	OnPrepareFileItems(*m_vecItems);

	m_vecItems->FillInDefaultIcons();

	m_guiState.reset(CGUIViewState::GetViewState(GetID(), *m_vecItems));

	m_unfilteredItems->SetPath(m_vecItems->GetPath());
	m_unfilteredItems->Append(*m_vecItems);

	OnCacheFileItems(*m_vecItems);

	OnFilterItems(GetProperty("filter").asString());

	UpdateButtons();

	if (nSelect > 0)
	{
		m_viewControl.SetSelectedItem(nSelect);
	}

	return true;
}

bool CVDMWindowRecommend::OnClick(int iItem)
{
	if (iItem < 0 || iItem >= (int)m_vecItems->Size()) return true;
	CFileItemPtr pItem = m_vecItems->Get(iItem);
	
	CGUIMediaWindow::OnClick(iItem);
	
	if (iItem > 0)
	{
		std::string strCountryCode = pItem->GetLabel();

		for (size_t i = 0; i < m_vecInstalled.size(); i++)
		{
			if (strCountryCode == m_vecInstalled[i])
			{
				bool bCanceled = false;
				if (!CGUIDialogYesNo::ShowAndGetInput(g_localizeStrings.Get(70131), g_localizeStrings.Get(70132), bCanceled, g_localizeStrings.Get(106), g_localizeStrings.Get(107)) || bCanceled)
				{
					return true;
				}

				break;
			}
		}

		InstallAddons(strCountryCode);
	}

	g_windowManager.PreviousWindow();

	return true;
}

bool CVDMWindowRecommend::OnMessage(CGUIMessage& message)
{
	switch (message.GetMessage())
	{
	case GUI_MSG_DOWNLOAD_SUCCESS:
	{
		CGUIDialogKaiToast::QueueNotification(g_localizeStrings.Get(70131), g_localizeStrings.Get(70133));

		break;
	}
	case GUI_MSG_DOWNLOAD_FAILED:
	{
		break;
	}
	case GUI_MSG_INSTALL_SUCCESS:
	{
		break;
	}
	case GUI_MSG_INSTALL_FAILED:
	{
		break;
	}
	default:
		break;
	}

	return CGUIMediaWindow::OnMessage(message);
}

bool CVDMWindowRecommend::LoadAddonInstallInfo(void)
{
	m_vecInstalled.clear();

	std::string strPath;
	CXBMCTinyXML doc;

	strPath = URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "AddonInstallInfo");

	if (!doc.LoadFile(strPath))
	{
		return false;
	}

	TiXmlElement *root = doc.RootElement();
	if (!root || strcmp(root->Value(), "AddonInstallInfo"))
	{
		return false;
	}

	TiXmlElement *country = root->FirstChildElement("Country");
	while (country)
	{
		m_vecInstalled.push_back(country->Attribute("Code"));

		country = country->NextSiblingElement("Country");
	}

	return true;
}

bool CVDMWindowRecommend::SaveAddonInstallInfo(void)
{
	std::string strPath;
	CXBMCTinyXML doc;

	strPath = URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "AddonInstallInfo");

	TiXmlElement root("AddonInstallInfo");

	for (size_t i = 0; i < m_vecInstalled.size(); ++i)
	{
		TiXmlElement country("Country");

		country.SetAttribute("Code", m_vecInstalled[i].c_str());

		root.InsertEndChild(country);
	}

	doc.InsertEndChild(root);
	return doc.SaveFile(strPath.c_str());
}

CGetAddonMapJob::CGetAddonMapJob()
{
}

void CGetAddonMapJob::GetAddonInfo(std::string& strCountryCode, std::string& strMapPath, AddonRepoVec& vecAddons)
{
	strCountryCode = m_strCountryCode;
	strMapPath = m_strMapPath;
	vecAddons = m_vecAddons;
}

bool CGetAddonMapJob::DoWork()
{
	std::string strAddonMap;
	CXBMCTinyXML doc;

	while (!ShouldCancel(1, 1))
	{
		Sleep(100);

		strAddonMap = g_DllVidonUtils.Vidon_GetAddonsInfo("");

		if (strAddonMap.empty())
		{
			continue;
		}

		m_strCountryCode = CJSONVariantParser::Parse((unsigned char *)strAddonMap.c_str(), strAddonMap.size())[0].asString();
		m_strMapPath = CJSONVariantParser::Parse((unsigned char *)strAddonMap.c_str(), strAddonMap.size())[1].asString();

		if (m_strMapPath.empty())
		{
			continue;
		}

		if (!doc.LoadFile(m_strMapPath))
		{
			continue;
		}

		break;
	}

	m_vecAddons.clear();

	TiXmlElement *root = doc.RootElement();
	if (!root)
	{
		CLog::Log(LOGERROR, "Addon map is not correct.");
		return false;
	}

	TiXmlElement *repo = root->FirstChildElement("Repo");
	while (repo)
	{
		AddonRepo		addonNote;
		
		addonNote.strCountryCode = repo->Attribute("CountryCode");
		addonNote.strPath = repo->Attribute("RepoPath");

		m_vecAddons.push_back(addonNote);

		repo = repo->NextSiblingElement("Repo");
	}

	return true;
}

#endif