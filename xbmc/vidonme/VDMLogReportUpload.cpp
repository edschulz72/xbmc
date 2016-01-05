
#ifdef HAS_VIDONME

#include "VDMLogReportUpload.h"

#include "filesystem/SpecialProtocol.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "filesystem/CurlFile.h"
#include "filesystem/File.h"
#include "utils/URIUtils.h"
#include "utils/StringUtils.h"
#include "settings/AdvancedSettings.h"
#include "guilib/LocalizeStrings.h"
#include "VDMVersionUpdate.h"
#include "vidonme/VDMRegionFeature.h"
#include "DLLVidonUtils.h"
#include "filesystem/Directory.h"

#if defined(TARGET_ANDROID)
#include "android/activity/XBMCApp.h"
#include "android/jni/PackageInfo.h"
#include "utils/CPUInfo.h"
#endif


bool Method_LogReportUpload::MethodCall()
{
    return Upload(m_strUserID, m_strUserEmail);
}


bool Method_LogReportUpload::Upload(const std::string& strUserID, const std::string& strUserEmail)
{
    bool bRet = false;
		
		CFileItemList items;
		XFILE::CDirectory::GetDirectory(g_advancedSettings.m_logFolder, items, "*.dmp");

    if( g_DllVidonUtils.IsLoaded() )
    {
      VidonUploadReportHandle hHandle = g_DllVidonUtils.Vidon_CreateUploadReportHandle();
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VdionUpload_TempDir, CSpecialProtocol::TranslatePath("special://temp/").c_str() );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VidonUploadParams_Title, "XBMC15" );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VidonUploadParams_Username, strUserID.c_str() );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VidonUploadParams_Email, strUserEmail.c_str() );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VidonUploadParams_Version, CVDMVersionCheck::GetCurrVersionCode().c_str() );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VdionUploadParams_RecordType,"log" );
#ifdef TARGET_ANDROID
      g_DllVidonUtils.Vidon_PushUploadFileParam( hHandle, URIUtils::AddFileToFolder(getenv("HOME"), "kodi.logcat").c_str() );
      g_DllVidonUtils.Vidon_PushUploadFileParam( hHandle, URIUtils::AddFileToFolder(getenv("HOME"), "kodi.dmp").c_str() );
#endif
			g_DllVidonUtils.Vidon_PushUploadFileParam(hHandle, URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "xbmc.log").c_str());
			g_DllVidonUtils.Vidon_PushUploadFileParam(hHandle, URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "xbmc.old.log").c_str());
			g_DllVidonUtils.Vidon_PushUploadFileParam(hHandle, URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "vdplayer.log").c_str());
			g_DllVidonUtils.Vidon_PushUploadFileParam(hHandle, URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "vidon_internal.log").c_str());

			for (int i = 0; i < items.Size(); ++i)
			{
				if (!items[i]->m_bIsFolder)
				{
					g_DllVidonUtils.Vidon_PushUploadFileParam(hHandle, items.Get(i)->GetPath().c_str());
				}
			}

      bRet = g_DllVidonUtils.Vidon_UploadReport( hHandle );
      g_DllVidonUtils.Vidon_DestroyUploadReportHandle( hHandle );
    }

    if ( !bRet )
    {
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Error,
            g_localizeStrings.Get(70077),
            g_localizeStrings.Get(70078),
            TOAST_DISPLAY_TIME, false);
        return false;
    }

    CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info,
        g_localizeStrings.Get(70077),
        g_localizeStrings.Get(70079),
        TOAST_DISPLAY_TIME, false);

		for (int i = 0; i < items.Size(); ++i)
		{
			if (!items[i]->m_bIsFolder)
			{
				XFILE::CFile::Delete(items.Get(i)->GetPath());
			}
		}

    return true;

}

#endif