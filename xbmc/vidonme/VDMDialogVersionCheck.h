#include "guilib/GUIDialog.h"
#include "VDMVersionUpdate.h"

class CVDMDialogVersionCheck :
    public CGUIDialog
{
public:
    CVDMDialogVersionCheck(void);
    virtual ~CVDMDialogVersionCheck(void);
    virtual bool OnMessage(CGUIMessage& message);
    virtual bool OnBack(int actionID);
    static bool ShowAndGetInput(OUT CVDMVersionInfo& info);

    static bool ShowNewVersionDialog( const CVDMVersionInfo& info );
    void SetVersionInfo( const CVDMVersionInfo& info );

    virtual void OnInitWindow();
protected:
    bool m_bConfirmed;

private:
    CVDMVersionInfo m_info;
    bool VersionCheck();
    
    enum State
    {
        sUnknown = -1,
        sChecking,
        sError,
        sUpdateNone,
        sUpdateAvail,
    };
    State m_state;
    void SetUpdateState(const State state);
    void DoVersionCheck();

};


