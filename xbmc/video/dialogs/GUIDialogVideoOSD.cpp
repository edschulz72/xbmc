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

#include "GUIDialogVideoOSD.h"
#include "Application.h"
#include "GUIUserMessages.h"
#include "guilib/GUIWindowManager.h"
#include "input/Key.h"
#include "input/InputManager.h"

using namespace PVR;

#ifdef HAS_VIDONME
#include "utils/SeekHandler.h"

#define CONTROL_BUTTON_PREVIOUS		200
#define CONTROL_BUTTON_PAUSE			202
#define CONTROL_BUTTON_NEXT				205
#define CONTROL_SLIDER_SEEK				87
#define CONTROL_BUTTON_CLOSE			1000
#endif

CGUIDialogVideoOSD::CGUIDialogVideoOSD(void)
    : CGUIDialog(WINDOW_DIALOG_VIDEO_OSD, "VideoOSD.xml")
{
  m_loadType = KEEP_IN_MEMORY;
}

CGUIDialogVideoOSD::~CGUIDialogVideoOSD(void)
{
}

void CGUIDialogVideoOSD::FrameMove()
{
  if (m_autoClosing)
  {
    // check for movement of mouse or a submenu open
    if (CInputManager::Get().IsMouseActive()
                           || g_windowManager.IsWindowActive(WINDOW_DIALOG_AUDIO_OSD_SETTINGS)
                           || g_windowManager.IsWindowActive(WINDOW_DIALOG_VIDEO_OSD_SETTINGS)
                           || g_windowManager.IsWindowActive(WINDOW_DIALOG_VIDEO_BOOKMARKS)
                           || g_windowManager.IsWindowActive(WINDOW_DIALOG_PVR_OSD_CHANNELS)
                           || g_windowManager.IsWindowActive(WINDOW_DIALOG_PVR_OSD_GUIDE)
                           || g_windowManager.IsWindowActive(WINDOW_DIALOG_OSD_TELETEXT))
      // extend show time by original value
      SetAutoClose(m_showDuration);
  }
  CGUIDialog::FrameMove();
}

bool CGUIDialogVideoOSD::OnAction(const CAction &action)
{
  if (action.GetID() == ACTION_SHOW_OSD)
  {
    Close();
    return true;
  }

#ifdef HAS_VIDONME
	if (action.GetID() == ACTION_MOVE_UP)
	{
		if (GetFocusedControlID() >= CONTROL_BUTTON_PREVIOUS && GetFocusedControlID() <= CONTROL_BUTTON_NEXT)
		{
			SET_CONTROL_FOCUS(CONTROL_SLIDER_SEEK, 0);
			return true;
		}

		if (GetFocusedControlID() == CONTROL_SLIDER_SEEK)
		{
			SET_CONTROL_FOCUS(CONTROL_BUTTON_CLOSE, 0);
			return true;
		}
	}

	if (action.GetID() == ACTION_MOVE_DOWN)
	{
		if (GetFocusedControlID() == CONTROL_SLIDER_SEEK)
		{
			SET_CONTROL_FOCUS(CONTROL_BUTTON_PAUSE, 0);
			return true;
		}

		if (GetFocusedControlID() == CONTROL_BUTTON_CLOSE)
		{
			SET_CONTROL_FOCUS(CONTROL_SLIDER_SEEK, 0);
			return true;
		}
	}

	if (action.GetID() == ACTION_MOVE_LEFT && GetFocusedControlID() == CONTROL_SLIDER_SEEK)
	{
		CSeekHandler::Get().Seek(false, action.GetAmount(), action.GetRepeat(), false, SEEK_TYPE_VIDEO);
		return true;
	}

	if (action.GetID() == ACTION_MOVE_RIGHT && GetFocusedControlID() == CONTROL_SLIDER_SEEK)
	{
		CSeekHandler::Get().Seek(true, action.GetAmount(), action.GetRepeat(), false, SEEK_TYPE_VIDEO);
		return true;
	}
#endif
  return CGUIDialog::OnAction(action);
}

EVENT_RESULT CGUIDialogVideoOSD::OnMouseEvent(const CPoint &point, const CMouseEvent &event)
{
  if (event.m_id == ACTION_MOUSE_WHEEL_UP)
  {
    return g_application.OnAction(CAction(ACTION_ANALOG_SEEK_FORWARD, 0.5f)) ? EVENT_RESULT_HANDLED : EVENT_RESULT_UNHANDLED;
  }
  if (event.m_id == ACTION_MOUSE_WHEEL_DOWN)
  {
    return g_application.OnAction(CAction(ACTION_ANALOG_SEEK_BACK, 0.5f)) ? EVENT_RESULT_HANDLED : EVENT_RESULT_UNHANDLED;
  }

  return CGUIDialog::OnMouseEvent(point, event);
}

bool CGUIDialogVideoOSD::OnMessage(CGUIMessage& message)
{
  switch ( message.GetMessage() )
  {
  case GUI_MSG_VIDEO_MENU_STARTED:
    {
      // We have gone to the DVD menu, so close the OSD.
      Close();
    }
    break;
  }
  return CGUIDialog::OnMessage(message);
}

