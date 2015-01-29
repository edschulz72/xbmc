
#if defined (HAS_VIDONME)

#include "VDMSettingsManager.h"
#include "URL.h"
#include "settings/Settings.h"
#include "settings/AdvancedSettings.h"
#include "settings/lib/Setting.h"
#include "utils/StdString.h"
#include "utils/StringUtils.h"
#include "interfaces/Builtins.h"
#include "Application.h"
#include "settings/VideoSettings.h"
#include "vidonme/VDMUserInfo.h"
#include "vidonme/VDMDialogLogin.h"
#include "vidonme/VDMRegionFeature.h"

CVDMSettingsManager::CVDMSettingsManager()
{
}

CVDMSettingsManager::~CVDMSettingsManager()
{
}

CVDMSettingsManager& CVDMSettingsManager::Get()
{
	static CVDMSettingsManager sVDMSettingsManager;
	return sVDMSettingsManager;
}

void CVDMSettingsManager::OnSettingAction(const CSetting *setting)
{
	if (setting == NULL)
		return;

	const std::string &settingId = setting->GetId();

	if (settingId == "debugging.upload")
	{
      CStdString strUserName;
      CStdString strEmail = CSettings::Get().GetString( "usercenter.email");
	}
	
	if (settingId == "debugging.viewlog")
	{
		CStdString strExecute = "file://" +g_advancedSettings.m_logFolder + "xbmc.log";
		
		strExecute	= StringUtils::Format("VDMOpenURL(%s)", strExecute.c_str());
		CBuiltins::Execute(strExecute);
	}

	if (settingId == "upgrade.website")
	{
		CBuiltins::Execute(StringUtils::Format("VDMOpenURL(%s)", CVDMRegionFeature::Get().GetWebSite().c_str()));
	}
}

bool CVDMSettingsManager::OnSettingChanging(const CSetting *setting)
{
	if (setting == NULL)
		return false;

	const std::string &settingId = setting->GetId();

	if (settingId == "d3.mode")
	{
		if (g_application.m_pPlayer)
		{
			//g_application.m_pPlayer->SetPlayMode((DIMENSIONMODE)((CSettingInt*)setting)->GetValue());
		}
	}

#if !defined(AML_DEMO)
	if (settingId == "audiooutput.passthrough")
	{
		CSettingBool* pSettingsTmp = (CSettingBool*)setting;
		if (pSettingsTmp->GetValue())
		{
			if (!CVDMUserInfo::Instance().IsCurrentLicenseAvailable())
			{
				CVDMDialogLogin::ShowLoginTip();
				if (!CVDMUserInfo::Instance().IsCurrentLicenseAvailable())
				{
					pSettingsTmp->SetValue(false);
				}
			}
		}
	}
#endif

	return true;
}

bool CVDMSettingsManager::OnSettingUpdate(CSetting* &setting, const char *oldSettingId, const TiXmlNode *oldSettingNode)
{
	return true;
}
#endif
