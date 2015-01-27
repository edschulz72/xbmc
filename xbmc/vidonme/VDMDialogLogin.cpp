#if defined(HAS_VIDONME)

#include "VDMDialogLogin.h"
#include "guilib/GUIMessage.h"
#include "guilib/GUIEditControl.h"
#include "guilib/GUIRadioButtonControl.h"
#include "guilib/Key.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "FileItem.h"
#include "utils/Variant.h"
#include "VDMUserInfo.h"
#include "utils/md5.h"
#include "DLLVidonUtils.h"
#include "utils/StringUtils.h"
#include "SynchroMethod.h"

#define CONTROL_GROUP_LOGIN   1
#define CONTROL_EDIT_USERNAME 400
#define CONTROL_EDIT_PASSWORD 401
#define CONTROL_EDIT_CARDID   300
#define CONTROL_BUTTON_LOGIN  402
#define CONTROL_BUTTON_CANCEL 203
#define CONTROL_BUTTON_BIND   202
#define CONTROL_LABEL_HEADER  100
#define CONTROL_LABEL_REGISTER_STATUS 101
#define CONTROL_GROUP_CARDID_LIST     500
#define CONTROL_BUTTON_NO_ACCOUNT			403
#define CONTROL_BUTTON_WEBSITE_NOTE		405

#define CONTROL_RADIOBUTTON_COUNTRYCODE 701

#define CONTROL_LIST_LICENSE 201

CVDMDialogLogin::CVDMDialogLogin(void)
    : CGUIDialog(VDM_WINDOW_DIALOG_LOGIN, "special://xbmc/media/VDMResource/DialogVDMLogin.xml")
{
  m_item = CFileItemPtr(new CFileItem);
  m_vecItems = new CFileItemList;
  m_loadType = KEEP_IN_MEMORY;
}

CVDMDialogLogin::~CVDMDialogLogin(void)
{
  delete m_vecItems;
  m_vecItems = NULL;
}

bool CVDMDialogLogin::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      if (m_viewControl.HasControl(CONTROL_LIST_LICENSE))
      {
        int iAction = message.GetParam1();
        if (ACTION_SELECT_ITEM == iAction || ACTION_MOUSE_LEFT_CLICK == iAction)
        {
          int iSelected = m_viewControl.GetSelectedItem();
          if(iSelected >= 0 && iSelected < (int)m_vecItems->Size())
          {
            CFileItemPtr item(m_vecItems->Get(iSelected));
            for (int i = 0 ; i < m_vecItems->Size() ; i++)
                m_vecItems->Get(i)->Select(false);
            item->Select(true);
          }
          UpdateButtons();
        }
      }

      if( CONTROL_BUTTON_LOGIN == iControl )
      {
        OnLoginClicked();
        return true;
      }
      else if( CONTROL_BUTTON_CANCEL == iControl )
      {
        if( CVDMUserInfo::Instance().IsLogin() || m_item->HasProperty("AllowClose") )
        {
          Close();
        }
      }
      else if( CONTROL_BUTTON_BIND == iControl )
      {
        OnBindClicked();
      }
      else if( CONTROL_EDIT_USERNAME == iControl || CONTROL_EDIT_PASSWORD == iControl )
      {
        m_item->ClearProperty("ErrorDesc");
      }
      else if( CONTROL_EDIT_CARDID == iControl )
      {
        m_item->ClearProperty("BindErrorDesc");
        UpdateButtons();
      }
			else if (CONTROL_BUTTON_NO_ACCOUNT == iControl)
			{
				SET_CONTROL_VISIBLE(CONTROL_BUTTON_WEBSITE_NOTE);
			}
      else
      {
      }
      break;
    }
  case GUI_MSG_VISIBLE:
    {
      if( !this->IsDialogRunning() )
      {
        CVDMDialogLogin::ShowLogin();
      }
      break;
    }
  default:
    break;
  }
  return CGUIDialog::OnMessage(message);
}

void CVDMDialogLogin::OnInitWindow()
{
  CGUIDialog::OnInitWindow();

  CGUIEditControl* pUserNameControl = (CGUIEditControl*)GetControl(CONTROL_EDIT_USERNAME);
  CGUIEditControl* pPasswordControl = (CGUIEditControl*)GetControl(CONTROL_EDIT_PASSWORD);
  CGUIEditControl* pCardIDControl = (CGUIEditControl*)GetControl(CONTROL_EDIT_CARDID);
  if( NULL != pUserNameControl && NULL != pPasswordControl && NULL != pCardIDControl )
  {
    pUserNameControl->SetLabel2("");
    pPasswordControl->SetLabel2("");
    pCardIDControl->SetLabel2("");
  }

	SET_CONTROL_HIDDEN(CONTROL_BUTTON_WEBSITE_NOTE);

  m_viewControl.Reset();
  m_viewControl.SetParentWindow(GetID());
  m_viewControl.AddView(GetControl(CONTROL_LIST_LICENSE));
  UpdateControls();
}

void CVDMDialogLogin::OnDeinitWindow(int nextWindowID)
{
  m_viewControl.Reset();
  m_vecItems->Clear();
  m_item->ClearProperties();
  CGUIDialog::OnDeinitWindow( nextWindowID );
}

bool CVDMDialogLogin::ShowLoginTip()
{
  CFileItemPtr item = CFileItemPtr(new CFileItem);
  if( CVDMUserInfo::Instance().IsLogin() )
  {
    if( CVDMUserInfo::Instance().IsCurrentLicenseAvailable() )
    {
      return false;
    }
    else if( CVDMUserInfo::Instance().IsCurrentLicenseEmpty() || CVDMUserInfo::Instance().IsCurrentLicenseExpired() )
    {
      item->SetProperty( "HideInputUserName", "true" );
      item->SetProperty("UseAdvanceFeature", "true");
    }
  }
  CVDMDialogLogin* dialog = (CVDMDialogLogin*)g_windowManager.GetWindow(VDM_WINDOW_DIALOG_LOGIN);
  if (dialog)
  {
    item->SetProperty("AllowClose", "true");
    dialog->SetItem(item);
    dialog->DoModal();
  }
  return true;
}

bool CVDMDialogLogin::ShowLogin()
{
  CFileItemPtr item = CFileItemPtr(new CFileItem);
  CVDMDialogLogin* dialog = (CVDMDialogLogin*)g_windowManager.GetWindow(VDM_WINDOW_DIALOG_LOGIN);
  if (dialog)
  {
    item->SetProperty( "FirstLogin", "true" );
    dialog->SetItem(item);
    dialog->DoModal();
  }
  return true;
}

bool CVDMDialogLogin::ShowSwitchUser()
{
  CFileItemPtr item = CFileItemPtr(new CFileItem);
  item->SetProperty("SwitchUser", "true");
  item->SetProperty("AllowClose", "true");
  CVDMDialogLogin* dialog = (CVDMDialogLogin*)g_windowManager.GetWindow(VDM_WINDOW_DIALOG_LOGIN);
  if (dialog)
  {
    dialog->SetItem(item);
    dialog->DoModal();
  }
  return true;
}

bool CVDMDialogLogin::SetItem(const CFileItemPtr &item)
{
  *m_item = *item;
  return true;
}

void CVDMDialogLogin::UpdateButtons()
{
  if( m_item->HasProperty( "HideInputUserName" ) )
  {
    if( m_item->HasProperty( "ExistCode" ) )
    {
      bool bSelected = false;
      for (int i = 0 ; i < m_vecItems->Size() ; i++)
      {
        if( m_vecItems->Get(i)->IsSelected() )
        {
          bSelected = true;
          break;
        }
      }
      CONTROL_ENABLE_ON_CONDITION( CONTROL_BUTTON_BIND, bSelected );
    }
    else
    {
      CGUIEditControl* pCardIDControl = (CGUIEditControl*)GetControl(CONTROL_EDIT_CARDID);
      CONTROL_ENABLE_ON_CONDITION( CONTROL_BUTTON_BIND, (NULL != pCardIDControl && !pCardIDControl->GetLabel2().empty()) );
    }
  }
}

void CVDMDialogLogin::UpdateControls()
{
  SendMessage(GUI_MSG_SET_TYPE, CONTROL_EDIT_PASSWORD, CGUIEditControl::INPUT_TYPE_PASSWORD, 12326);
  if( m_item->HasProperty( "HideInputUserName" ) ) // login success
  {
    if( CVDMUserInfo::Instance().IsCurrentLicenseEmpty() || CVDMUserInfo::Instance().IsCurrentLicenseExpired() )
    {
      m_item->SetProperty( "HeaderLabel", g_localizeStrings.Get(70007) );
      SET_CONTROL_FOCUS( CONTROL_GROUP_CARDID_LIST, 0 );
      std::vector<libVidonUtils::Code> vecCode;
      CVDMUserInfo::Instance().GetAvailableCode( vecCode );
      if( !vecCode.empty() )
      {
        m_item->SetProperty( "ExistCode", "true" );

        if( m_item->HasProperty( "UseAdvanceFeature" ) )
        {
          m_item->SetProperty( "RegisterStatus", StringUtils::Format( "%s[CR]%s", g_localizeStrings.Get(70008).c_str(), g_localizeStrings.Get(70009).c_str() ) );
        }
        else
        {
          m_item->SetProperty( "RegisterStatus", g_localizeStrings.Get(70010) );
        }

        std::vector<libVidonUtils::Code>::iterator iter = vecCode.begin();
        for( ; iter != vecCode.end(); iter++ )
        {
          CFileItemPtr item( new CFileItem( iter->key ) );
          item->SetProperty("CodeID", iter->cid);
          m_vecItems->Add( item );
        }
        m_viewControl.SetCurrentView( CONTROL_LIST_LICENSE );
        m_viewControl.SetItems( *m_vecItems );
        SET_CONTROL_FOCUS( CONTROL_LIST_LICENSE, 0 );
      }
      else
      {
        if( m_item->HasProperty( "UseAdvanceFeature" ) )
        {
          m_item->SetProperty( "RegisterStatus", StringUtils::Format( "%s[CR]%s", g_localizeStrings.Get(70008).c_str(), g_localizeStrings.Get(70011).c_str() ) );
        }
        else
        {
          m_item->SetProperty( "RegisterStatus", g_localizeStrings.Get(70012) );
        }
        SET_CONTROL_FOCUS( CONTROL_EDIT_CARDID, 0 );
      }
      UpdateButtons();
    }
    else
    {
      Close();
    }
  }
  else
  {
    SET_CONTROL_FOCUS( CONTROL_EDIT_USERNAME, 0 );
    SET_CONTROL_SELECTED( GetID(), CONTROL_RADIOBUTTON_COUNTRYCODE, (CVDMUserInfo::Instance().GetCountryCode() == libVidonUtils::CountryCode_China) );
    m_item->SetProperty( "HeaderLabel", g_localizeStrings.Get(70007) );
  }
}

class Method_Login: public MethodBase
{
public:
    Method_Login( const CStdString& strUserName, const CStdString& strPassword )
    {
      m_strUserName = strUserName;
      m_strPassword = strPassword;
    }
    virtual bool MethodCall();
    virtual bool Cancel(){ return false; };
private:
  CStdString m_strUserName;
  CStdString m_strPassword;
};

bool Method_Login::MethodCall()
{
  CVDMUserInfo::Instance().DoLogin( m_strUserName, m_strPassword );
  return true;
}

void CVDMDialogLogin::OnLoginClicked()
{
  CGUIEditControl* pUserNameControl = (CGUIEditControl*)GetControl(CONTROL_EDIT_USERNAME);
  CGUIEditControl* pPasswordControl = (CGUIEditControl*)GetControl(CONTROL_EDIT_PASSWORD);

  CStdString strUserName = pUserNameControl->GetLabel2();
  CStdString strPassword = pPasswordControl->GetLabel2();

  m_item->ClearProperty( "ErrorDesc" );
  if( !strUserName.empty() && !strPassword.empty() )
  {
    CGUIRadioButtonControl* pCountryCode = (CGUIRadioButtonControl*)GetControl(CONTROL_RADIOBUTTON_COUNTRYCODE);
    if( NULL == pCountryCode || pCountryCode->IsSelected() )
    {
      CVDMUserInfo::Instance().SetCountryCode(libVidonUtils::CountryCode_China);
    }
    else
    {
      CVDMUserInfo::Instance().SetCountryCode(libVidonUtils::CountryCode_Others);
    }
    strPassword = XBMC::XBMC_MD5::GetMD5(strPassword);
    Method_Login* pLogin = new Method_Login( strUserName, strPassword );
    SynchroMethodCall(boost::shared_ptr<MethodPtr>(new MethodPtr(pLogin)));
    if( CVDMUserInfo::Instance().IsLogin() && CVDMUserInfo::Instance().GetLoginErrorDescription().empty()/*may be switch user*/ )
    {
      m_item->SetProperty( "HideInputUserName", "true" );
      if( CVDMUserInfo::Instance().IsCurrentLicenseAvailable() )
      {
        if( CVDMUserInfo::Instance().IsCurrentLicenseTrial() && m_item->HasProperty( "FirstLogin" ) )
        {
          m_item->SetProperty( "AllowClose","true" );
          m_item->SetProperty( "HeaderLabel", g_localizeStrings.Get(70093) );
          CStdString strEndTime = StringUtils::Format("[COLOR=red]%s[/COLOR]", CVDMUserInfo::Instance().GetCurrentLicenseEndTime().c_str());
          m_item->SetProperty( "ShowTrialTips", StringUtils::Format("Hi, %s [CR][CR]%s", strUserName.c_str(), StringUtils::Format(g_localizeStrings.Get(70094).c_str(), strEndTime.c_str()).c_str()) );
        }
        else
        {
          CGUIDialogKaiToast::QueueNotification( CGUIDialogKaiToast::Info, g_localizeStrings.Get(70014), g_localizeStrings.Get(70015) );
          Close();
        }
      }
      else if( CVDMUserInfo::Instance().IsCurrentLicenseEmpty() || CVDMUserInfo::Instance().IsCurrentLicenseExpired() )
      {
        UpdateControls();
      }
      else
      {
        //error?
      }
    }
    else
    {
      m_item->SetProperty( "ErrorDesc",CVDMUserInfo::Instance().GetLoginErrorDescription() );
    }
  }
  else
  {
    m_item->SetProperty("ErrorDesc", g_localizeStrings.Get(70013));
  }
}

class Method_Bind: public MethodBase
{
public:
    Method_Bind(const CStdString& strKey, const CStdString& strCodeID)
    {
      m_strKey = strKey;
      m_strCodeID = strCodeID;
    }
    virtual bool MethodCall();
    virtual bool Cancel(){ return false; };

private:

  CStdString m_strKey;
  CStdString m_strCodeID;
};

bool Method_Bind::MethodCall()
{
  int nResult = -1;
  CStdString strUserName;
  CStdString strPassword;
  CVDMUserInfo::Instance().GetUsernameAndPassword( strUserName, strPassword );
  CStdString strLicenseID = CVDMUserInfo::Instance().IsCurrentLicenseTrial() ? "" : CVDMUserInfo::Instance().GetCurrentLicenseID();
  if( !m_strKey.empty() )
  {
    g_DllVidonUtils.SetByKey( strUserName.c_str(), strPassword.c_str(), m_strKey.c_str(), strLicenseID.c_str(), nResult );
  }
  else
  {
    g_DllVidonUtils.SetByCid( strUserName.c_str(), strPassword.c_str(), m_strCodeID.c_str(), strLicenseID.c_str(), nResult );
  }
  if( nResult == 1 )
  {
    CVDMUserInfo::Instance().DoLogin( strUserName, strPassword );
  }
  return true;
}

void CVDMDialogLogin::OnBindClicked()
{
  CStdString strKey;
  CStdString strCodeID;
  if( m_item->HasProperty( "ExistCode" ) )
  {
    for (int i = 0 ; i < m_vecItems->Size() ; i++)
    {
      if( m_vecItems->Get(i)->IsSelected() )
      {
        strCodeID = m_vecItems->Get(i)->GetProperty("CodeID").asString();
        break;
      }
    }
  }
  else
  {
    CGUIEditControl* pCardIDControl = (CGUIEditControl*)GetControl(CONTROL_EDIT_CARDID);
    if( NULL != pCardIDControl )
    {
      strKey = pCardIDControl->GetLabel2();
    }
  }
  m_item->ClearProperty("BindErrorDesc");
  if( !strKey.empty() || !strCodeID.empty() )
  {
    Method_Bind* pBind = new Method_Bind( strKey, strCodeID );
    SynchroMethodCall(boost::shared_ptr<MethodPtr>(new MethodPtr(pBind)));
    if( CVDMUserInfo::Instance().IsCurrentLicenseAvailable() )
    {
      CGUIDialogKaiToast::QueueNotification( CGUIDialogKaiToast::Info, g_localizeStrings.Get(70014), g_localizeStrings.Get(70015) );
      Close();
    }
    else
    {
      m_item->SetProperty( "BindErrorDesc", g_localizeStrings.Get(70016) );
    }
  }
}

bool CVDMDialogLogin::OnBack(int actionID)
{
  if( m_item->HasProperty("AllowClose") )
  {
    return CGUIDialog::OnBack( actionID );
  }
  else
  {
    return false;
  }
}


#endif //#if defined(HAS_VIDONME)
