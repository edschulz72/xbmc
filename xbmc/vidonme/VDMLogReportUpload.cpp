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

#if defined(TARGET_ANDROID)
#include "android/activity/XBMCApp.h"
#include "android/jni/PackageInfo.h"
#include "utils/CPUInfo.h"
#endif


bool Method_LogReportUpload::MethodCall()
{
    return Upload(m_strUserID, m_strUserEmail);
}


bool Method_LogReportUpload::Upload(const CStdString& strUserID, const CStdString& strUserEmail)
{
    bool bRet = false;
    if( g_DllVidonUtils.IsLoaded() )
    {
      VidonUploadReportHandle hHandle = g_DllVidonUtils.Vidon_CreateUploadReportHandle();
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VdionUpload_TempDir, CSpecialProtocol::TranslatePath("special://temp/").c_str() );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VidonUploadParams_Title, "XBMC13" );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VidonUploadParams_Username, strUserID.c_str() );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VidonUploadParams_Email, strUserEmail.c_str() );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VidonUploadParams_Version, CVDMVersionCheck::GetCurrVersionCode().c_str() );
      g_DllVidonUtils.Vidon_SetUploadStringParam( hHandle, libVidonUtils::VdionUploadParams_RecordType,"log" );
      g_DllVidonUtils.Vidon_PushUploadFileParam( hHandle, URIUtils::AddFileToFolder(getenv("HOME"), "xbmc.logcat").c_str() );
      g_DllVidonUtils.Vidon_PushUploadFileParam( hHandle, URIUtils::AddFileToFolder(getenv("HOME"), "xbmc.dmp").c_str() );
      g_DllVidonUtils.Vidon_PushUploadFileParam( hHandle, URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "xbmc.log").c_str() );
      g_DllVidonUtils.Vidon_PushUploadFileParam( hHandle, URIUtils::AddFileToFolder(CSpecialProtocol::TranslatePath(g_advancedSettings.m_logFolder), "vdmplayer.log").c_str() );
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
    return true;

}



