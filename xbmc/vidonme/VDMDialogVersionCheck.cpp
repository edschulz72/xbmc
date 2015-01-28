/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "VDMDialogVersionCheck.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/Key.h"
#include "GUIUserMessages.h"
#include "utils/StringUtils.h"

#define GROUP_CHECKING                  101
#define GROUP_ERROR                     102
#define GROUP_UPDATENONE                103
#define GROUP_UPDATEAVAIL               104

#define BUTTON__ERROR_RETRY             201
#define BUTTON__ERROR_CANCEL            202
#define BUTTON__UPDATENONE_CLOSE        203
#define BUTTON__UPDATEAVAIL_INSTALL     204
#define BUTTON__UPDATEAVAIL_CANCEL      205

#define TEXT__TITLE                     301
#define TEXT__VERSION                   302
#define TEXT__CHECKING                  303
#define TEXT__NEWVERSION                304
#define TEXT__CHANGELOG                 305


class Method_VersionCheck: public MethodBase
{
public:
    virtual bool MethodCall()
    {
        return CVDMVersionCheck::VersionCheck(m_info);
    }
    virtual bool Cancel(){ return false; };
    CVDMVersionInfo& info() { return m_info; }
private:
    CVDMVersionInfo m_info;
};


CVDMDialogVersionCheck::CVDMDialogVersionCheck(void)
    : CGUIDialog(VDM_DIALOG_VERSIONCHECK, "special://xbmc/media/VDMResource/VDMDialogVersionCheck.xml")
    , m_state(sUnknown)
{
}

CVDMDialogVersionCheck::~CVDMDialogVersionCheck(void)
{}


bool CVDMDialogVersionCheck::VersionCheck()
{
    Method_VersionCheck *methodBase = new Method_VersionCheck;
    boost::shared_ptr<MethodPtr> method(new MethodPtr(methodBase));
    if ( !SynchroMethodCall(method) ) return false;
    m_info = methodBase->info();
    return true;
}

void CVDMDialogVersionCheck::DoVersionCheck()
{
    SetUpdateState(sChecking);
    if (!VersionCheck())
        SetUpdateState(sError);
    else if (m_info.state == CVDMVersionInfo::sUpdateNone)
        SetUpdateState(sUpdateNone);
    else if (m_info.state == CVDMVersionInfo::sUpdateAvail)
        SetUpdateState(sUpdateAvail);
    else
        SetUpdateState(sError);

}

void CVDMDialogVersionCheck::SetUpdateState(const State state)
{
    switch (state)
    {
    case sChecking:
        SET_CONTROL_VISIBLE(GROUP_CHECKING);
        SET_CONTROL_HIDDEN(GROUP_ERROR);
        SET_CONTROL_HIDDEN(GROUP_UPDATENONE);
        SET_CONTROL_HIDDEN(GROUP_UPDATEAVAIL);
        SET_CONTROL_LABEL(TEXT__VERSION, StringUtils::Format("%s: %s", g_localizeStrings.Get(70081).c_str(), CVDMVersionCheck::GetCurrVersionName().c_str()));
        //SET_CONTROL_LABEL(TEXT__CHECKING, "checking ...");
        break;
    case sError:
        SET_CONTROL_HIDDEN(GROUP_CHECKING);
        SET_CONTROL_VISIBLE(GROUP_ERROR);
        SET_CONTROL_HIDDEN(GROUP_UPDATENONE);
        SET_CONTROL_HIDDEN(GROUP_UPDATEAVAIL);
        SET_CONTROL_FOCUS(BUTTON__ERROR_RETRY, 0);
        break;
    case sUpdateNone:
        SET_CONTROL_HIDDEN(GROUP_CHECKING);
        SET_CONTROL_HIDDEN(GROUP_ERROR);
        SET_CONTROL_VISIBLE(GROUP_UPDATENONE);
        SET_CONTROL_HIDDEN(GROUP_UPDATEAVAIL);
        SET_CONTROL_FOCUS(BUTTON__UPDATENONE_CLOSE, 0);
        break;
    case sUpdateAvail:
        SET_CONTROL_HIDDEN(GROUP_CHECKING);
        SET_CONTROL_HIDDEN(GROUP_ERROR);
        SET_CONTROL_HIDDEN(GROUP_UPDATENONE);
        SET_CONTROL_VISIBLE(GROUP_UPDATEAVAIL);
        SET_CONTROL_LABEL(TEXT__NEWVERSION, StringUtils::Format("%s: %s", g_localizeStrings.Get(70075).c_str(), m_info.strVersionName.c_str()));
        SET_CONTROL_LABEL(TEXT__CHANGELOG, m_info.strChangeLog);
        SET_CONTROL_FOCUS(BUTTON__UPDATEAVAIL_INSTALL, 0);
        break;
    }

}

void CVDMDialogVersionCheck::OnInitWindow()
{
    m_bConfirmed = false;
    CGUIDialog::OnInitWindow();

    if( m_state == sUnknown )
    {
      DoVersionCheck();
    }
    else
    {
      SetUpdateState( m_state );
    }
}

bool CVDMDialogVersionCheck::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_CLICKED:
    {
      int iControl = message.GetSenderId();
      switch(iControl)
      {
      case BUTTON__ERROR_RETRY:
          DoVersionCheck();
          return true;

      case BUTTON__UPDATEAVAIL_INSTALL:
          m_bConfirmed = true;
          Close();
          return true;

      case BUTTON__ERROR_CANCEL:
      case BUTTON__UPDATENONE_CLOSE:
      case BUTTON__UPDATEAVAIL_CANCEL:
          m_bConfirmed = false;
          Close();
          return true;
      }
    }
    break;
  }
  return CGUIDialog::OnMessage(message);
}

bool CVDMDialogVersionCheck::OnBack(int actionID)
{
  m_bConfirmed = false;
  return CGUIDialog::OnBack(actionID);
}

// \brief Show CVDMDialogVersionCheck dialog, then wait for user to dismiss it.
bool CVDMDialogVersionCheck::ShowAndGetInput(CVDMVersionInfo& info)
{
    CVDMDialogVersionCheck *dialog = (CVDMDialogVersionCheck *)g_windowManager.GetWindow(VDM_DIALOG_VERSIONCHECK);
    if (!dialog) return false;
    /*
    dialog->SetHeading(heading);
    dialog->SetLine(0, line0);
    dialog->SetLine(1, line1);
    dialog->SetLine(2, line2);
    if (autoCloseTime)
        dialog->SetAutoClose(autoCloseTime);
    if (iNoLabel != -1)
        dialog->SetChoice(0,iNoLabel);
    else
        dialog->SetChoice(0,106);
    if (iYesLabel != -1)
        dialog->SetChoice(1,iYesLabel);
    else
        dialog->SetChoice(1,107);
    dialog->m_bCanceled = false;
    */
    dialog->DoModal();
    if (!dialog->m_bConfirmed) return false;

    info = dialog->m_info;
    return true;
}

bool CVDMDialogVersionCheck::ShowNewVersionDialog( const CVDMVersionInfo& info )
{
  if( info.state != CVDMVersionInfo::sUpdateAvail )
  {
    return false;
  }

  CVDMDialogVersionCheck *dialog = (CVDMDialogVersionCheck *)g_windowManager.GetWindow(VDM_DIALOG_VERSIONCHECK);
  if (!dialog) return false;

  dialog->SetVersionInfo( info );
  dialog->DoModal();

  if (!dialog->m_bConfirmed) return false;

  g_vdmVersionUpdate.Start(info);
  return true;
}

void CVDMDialogVersionCheck::SetVersionInfo( const CVDMVersionInfo& info )
{
  m_info = info;

  if( info.state == CVDMVersionInfo::sUpdateAvail )
  {
    m_state = sUpdateAvail;
  }
  else
  {
    m_state = sUpdateNone;
  }
}

