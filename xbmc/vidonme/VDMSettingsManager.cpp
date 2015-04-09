
#if defined (HAS_VIDONME)

#include "VDMSettingsManager.h"
#include "URL.h"
#include "GUIUserMessages.h"
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
#include "vidonme/VDMVersionUpdate.h"
#include "vidonme/VDMDialogVersionCheck.h"
#include "vidonme/VDMLogReportUpload.h"

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
    CStdString strPassword;
    if (!CVDMUserInfo::Instance().GetUsernameAndPassword(strUserName, strPassword))
    {
      strUserName = "unknown username";
    }

    CStdString strEmail = CSettings::Get().GetString("usercenter.email");

    Method_LogReportUpload* LogUpload = new Method_LogReportUpload(strUserName, strEmail);
    SynchroMethodCall(boost::shared_ptr<MethodPtr>(new MethodPtr(LogUpload)));
	}
	
	if (settingId == "debugging.viewlog")
	{
		CStdString strExecute = "file://" +g_advancedSettings.m_logFolder + "kodi.log";
		
		strExecute	= StringUtils::Format("VDMOpenURL(%s)", strExecute.c_str());
		CBuiltins::Execute(strExecute);
	}

	if (settingId == "usercenter.switchuser")
	{
#if !defined(AML_DEMO)
		CVDMDialogLogin::ShowSwitchUser();
		if (CVDMUserInfo::Instance().IsLogin())
		{
			int label = setting->GetLabel();
			if (label != 70061)
			{
				CGUIMessage msg(GUI_MSG_UPDATE, VDM_WINDOW_SETTINGS_VIDONME, 0, 0);
				g_windowManager.SendThreadMessage(msg, VDM_WINDOW_SETTINGS_VIDONME);
			}
		}
#endif
	}

	if (settingId == "upgrade.website")
	{
		CBuiltins::Execute(StringUtils::Format("VDMOpenURL(%s)", CVDMRegionFeature::Get().GetWebSite().c_str()));
	}

	if (settingId == "upgrade.forum")
	{
		CBuiltins::Execute(StringUtils::Format("VDMOpenURL(%s)", CVDMRegionFeature::Get().GetForum().c_str()));
	}

	if (settingId == "upgrade.versioncheck")
	{
		if (g_vdmVersionUpdate.running())
		{
			return;
		}

		if (g_vdmVersionUpdate.IsCheckInBackground())
		{
			g_vdmVersionUpdate.CancelCheckVersionInBackground();
		}

		CVDMVersionInfo info;
		if (!CVDMDialogVersionCheck::ShowAndGetInput(info)) 
			return;

		g_vdmVersionUpdate.Start(info);
		return;
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
			g_application.m_pPlayer->SetPlayMode((DIMENSIONMODE)((CSettingInt*)setting)->GetValue());
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
