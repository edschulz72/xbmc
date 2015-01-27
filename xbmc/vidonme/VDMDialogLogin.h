#if defined(HAS_VIDONME)

#pragma once

#include "guilib/GUIDialog.h"
#include "view/GUIViewControl.h"

class CVDMDialogLogin: public CGUIDialog
{
public:

  CVDMDialogLogin(void);
  virtual ~CVDMDialogLogin(void);

  virtual CFileItemPtr GetCurrentListItem(int offset = 0) { return m_item; }
  virtual bool HasListItems() const { return true; }
  virtual bool OnMessage(CGUIMessage& message);

  virtual void OnInitWindow();
  virtual void OnDeinitWindow(int nextWindowID);

  virtual bool OnBack(int actionID);

  static bool ShowLoginTip();
  static bool ShowLogin();
  static bool ShowSwitchUser();


private:

  bool SetItem(const CFileItemPtr &item);
  void OnLoginClicked();
  void OnBindClicked();
  void UpdateButtons();
  void UpdateControls();

  CFileItemPtr m_item;

  CFileItemList* m_vecItems;
  CGUIViewControl m_viewControl;

};

#endif // if defined(HAS_VIDONME)