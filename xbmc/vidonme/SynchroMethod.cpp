#if defined(HAS_VIDONME)

#include "SynchroMethod.h"

bool SynchroMethodCall( const boost::shared_ptr<MethodPtr> method)
{
    const int TIME_TO_BUSY_DIALOG = 500;
    //boost::shared_ptr<MethodPtr> method(new MethodPtr(methodBase));

    bool result = false, cancel = false;
    while (!result && !cancel)
    {
        if (g_application.IsCurrentThread())
        {
            CSingleExit ex(g_graphicsContext);

            MethodHolder  methodHolder(method);
            if(!methodHolder.Wait(TIME_TO_BUSY_DIALOG))
            {
                CGUIDialogBusy* dialog = (CGUIDialogBusy*)g_windowManager.GetWindow(WINDOW_DIALOG_BUSY);
                dialog->Show();

                while(!methodHolder.Wait(10))
                {
                    CSingleLock lock(g_graphicsContext);

                    // update progress
                    std::string text;
                    float progress = method->GetProgress(text);
                    if (progress > 0)
                        dialog->SetProgress(progress);

                    if(dialog->IsCanceled())
                    {
                        cancel = method->Cancel();
                        if( cancel ) break;
                    }

                    lock.Leave(); // prevent an occasional deadlock on exit
                    g_windowManager.ProcessRenderLoop(false);
                }
                if(dialog)
                    dialog->Close();
            }
            result = methodHolder.GetResults();
        }
        else
        {
            result = method->MethodCall();
        }

        if (!result)
        {
            //                 if (!cancel && g_application.IsCurrentThread() && proxy->ProcessRequirements())
            //                     continue;
            //                 CLog::Log(LOGERROR, "%s - Error getting %s", __FUNCTION__, CURL::GetRedacted(strPath).c_str());
            return false;
        }

    }
    return true;
}



#endif
