/*
 *      Copyright (C) 2005-2015 Team XBMC
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


#include "GUIDialogSimpleMenu.h"
#include "guilib/GUIWindowManager.h"
#include "GUIDialogSelect.h"
#include "settings/DiscSettings.h"
#include "settings/Settings.h"
#include "utils/URIUtils.h"
#include "filesystem/Directory.h"
#include "filesystem/File.h"
#include "utils/log.h"
#include "video/VideoInfoTag.h"
#include "URL.h"
#include "utils/Variant.h"


#ifdef HAS_VIDONME

#include "PlayListPlayer.h"
#include "Application.h"
#include "utils/StringUtils.h"
#include "guilib/LocalizeStrings.h"
#include "windows/GUIMediaWindow.h"
#include "playlists/PlayList.h"
#include "GUIUserMessages.h"

using namespace PLAYLIST;

#endif

bool CGUIDialogSimpleMenu::ShowPlaySelection(CFileItem& item)
{
#ifndef HAS_VIDONME
  /* if asked to resume somewhere, we should not show anything */
  if (item.m_lStartOffset || (item.HasVideoInfoTag() && item.GetVideoInfoTag()->m_iBookmarkId > 0))
    return true;
#endif

  if (CSettings::GetInstance().GetInt(CSettings::SETTING_DISC_PLAYBACK) != BD_PLAYBACK_SIMPLE_MENU)
    return true;

  std::string path;
  if (item.IsVideoDb())
    path = item.GetVideoInfoTag()->m_strFileNameAndPath;
  else
    path = item.GetPath();

  if (item.IsBDFile())
  {
    std::string root = URIUtils::GetParentPath(path);
    URIUtils::RemoveSlashAtEnd(root);
    if (URIUtils::GetFileName(root) == "BDMV")
    {
      CURL url("bluray://");
      url.SetHostName(URIUtils::GetParentPath(root));
      url.SetFileName("root");
      return ShowPlaySelection(item, url.Get());
    }
  }

  if (item.IsDiscImage())
  {
    CURL url2("udf://");
    url2.SetHostName(item.GetPath());
    url2.SetFileName("BDMV/index.bdmv");
    if (XFILE::CFile::Exists(url2.Get()))
    {
      url2.SetFileName("");

      CURL url("bluray://");
      url.SetHostName(url2.Get());
      url.SetFileName("root");
      return ShowPlaySelection(item, url.Get());
    }
  }
  return true;
}

bool CGUIDialogSimpleMenu::ShowPlaySelection(CFileItem& item, const std::string& directory)
{

  CFileItemList items;

#ifdef HAS_VIDONME

	CFileItemPtr pItem;
	std::string originPath = item.GetPath();
	std::string strFolerType = item.GetProperty("type").asString();

	pItem.reset(new CFileItem());

	if (CSettings::GetInstance().GetBool("using.vdmplayer"))
	{
		if (URIUtils::GetFileName(originPath) == "index.bdmv" ||
			URIUtils::GetFileName(originPath) == "MovieObject.bdmv")
		{
			std::string strParentPath = URIUtils::GetParentPath(originPath);
			URIUtils::RemoveSlashAtEnd(strParentPath);

			if (URIUtils::GetFileName(strParentPath) == "BDMV")
			{
				std::string strTmpPath = URIUtils::AddFileToFolder(strParentPath, "PLAYLIST");
				strTmpPath = URIUtils::AddFileToFolder(strTmpPath, "1048575.mpls");
				pItem->SetPath(strTmpPath);
				pItem->SetProperty("original_listitem_url", URIUtils::GetParentPath(strParentPath));
			}
		}
		else if (URIUtils::GetExtension(originPath) == ".iso")
		{
			std::string strTmpPath = URIUtils::AddFileToFolder(originPath, "BDMV");
			strTmpPath = URIUtils::AddFileToFolder(strTmpPath, "PLAYLIST");
			strTmpPath = URIUtils::AddFileToFolder(strTmpPath, "1048575.mpls");
			pItem->SetPath(strTmpPath);
		}

		if (g_application.GetLastPlayedFile() == pItem->GetPath())
		{
			pItem->Select(true);
		}

		pItem->m_bIsFolder = false;
		pItem->SetLabel(g_localizeStrings.Get(70095) /* MainTitle */);
		pItem->SetIconImage("DefaultVideo.png");
		pItem->SetProperty("chooseitem", "maintitle");
		items.Add(pItem);

		pItem.reset(new CFileItem());
		pItem->SetPath(URIUtils::AddFileToFolder(directory, "titles"));
		pItem->m_bIsFolder = true;
		pItem->SetLabel(g_localizeStrings.Get(25002) /* All titles */);
		pItem->SetIconImage("DefaultVideoPlaylists.png");
		pItem->SetProperty("chooseitem", "alltitle");
		items.Add(pItem);

		pItem.reset(new CFileItem());
		pItem->SetPath(originPath + "/Menu");
		pItem->m_bIsFolder = false;
		pItem->SetLabel(g_localizeStrings.Get(25003) /* BDMenus */);
		pItem->SetProperty("chooseitem", "menu");
		pItem->SetIconImage("DefaultProgram.png");
		if (g_application.GetLastPlayedFile() == originPath)
		{
			pItem->Select(true);
		}
		items.Add(pItem);
	}
	else
	{
		if (!XFILE::CDirectory::GetDirectory(directory, items, XFILE::CDirectory::CHints(), true))
		{
			CLog::Log(LOGERROR, "CGUIWindowVideoBase::ShowPlaySelection - Failed to get play directory for %s", directory.c_str());
			return false;
		}
	}

#else

  if (!XFILE::CDirectory::GetDirectory(directory, items, XFILE::CDirectory::CHints(), true))
  {
    CLog::Log(LOGERROR, "CGUIWindowVideoBase::ShowPlaySelection - Failed to get play directory for %s", directory.c_str());
    return true;
  }

#endif

#ifdef HAS_VIDONME

	pItem.reset(new CFileItem());

	if (strFolerType == "BDFolder" || strFolerType == "DVDFolder")
	{
		pItem.reset(new CFileItem());
		std::string strFolderPath = StringUtils::Left(originPath, originPath.size() - 15);
		pItem->SetPath(strFolderPath + "/openfolder");
		pItem->m_bIsFolder = true;
		pItem->SetLabel(g_localizeStrings.Get(70096) /* Open Folder */);
		pItem->SetIconImage("DefaultFolder.png");
		pItem->SetProperty("chooseitem", "update");
		items.Add(pItem);
	}

#endif

  if (items.IsEmpty())
  {
    CLog::Log(LOGERROR, "CGUIWindowVideoBase::ShowPlaySelection - Failed to get any items %s", directory.c_str());
    return true;
  }

  CGUIDialogSelect* dialog = (CGUIDialogSelect*)g_windowManager.GetWindow(WINDOW_DIALOG_SELECT);
  while (true)
  {
    dialog->Reset();
    dialog->SetHeading(CVariant{25006}); // Select playback item
    dialog->SetItems(items);
    dialog->SetUseDetails(true);

#ifdef HAS_VIDONME
		CFileItemPtr item_new;

		if (item.m_lStartOffset == STARTOFFSET_RESUME)
		{
			item_new = items.Get(0);
			item_new->SetProperty("type", item.GetProperty("type"));
			
			if (!CSettings::GetInstance().GetBool("using.vdmplayer") && 
				(item.GetProperty("type") == "BDFolder" || 
				item.GetProperty("type") == "DVDFolder"))
			{
				item_new->SetProperty("original_listitem_url", item.GetProperty("original_listitem_url"));
			}

			item_new->m_lStartOffset = STARTOFFSET_RESUME;
		}
		else if (g_playlistPlayer.GetPlaylist(PLAYLIST_VIDEO).size() > 1 && g_playlistPlayer.HasPlayedFirstFile())
		{
			if (!g_application.IsPlayWithMenu())
			{
				item.SetPath(items.Get(0)->GetPath());
			}
			return true;
		}
		else
		{
			dialog->Open();
			item_new = dialog->GetSelectedItem();

			if (!dialog->IsConfirmed() || !item_new || dialog->GetSelectedLabel() < 0)
			{
				CLog::Log(LOGDEBUG, "CGUIWindowVideoBase::ShowPlaySelection - User aborted %s", directory.c_str());
				break;
			}
		}

		if (item_new->GetProperty("chooseitem").asString() == "update")
		{
			std::string strRealPath = item_new->GetPath();
			strRealPath = StringUtils::Left(strRealPath, strRealPath.find("/openfolder"));
			item_new->SetPath(strRealPath);

			item.SetProperty("chooseitem", item_new->GetProperty("chooseitem").asString());
			item.SetPath(item_new->GetPath());

			CGUIMessage msg(GUI_MSG_NOTIFY_ALL, 0, 0, GUI_MSG_UPDATE);
			msg.SetStringParam(item.GetPath());
			g_windowManager.SendThreadMessage(msg);

			return false;
		}

		if (item_new->GetProperty("chooseitem").asString() == "menu")
		{
			std::string strRealPath = item_new->GetPath();
			strRealPath = StringUtils::Left(strRealPath, strRealPath.find("/Menu"));
			item_new->SetPath(strRealPath);

			g_application.SetPlayWithMenu(true);
		}
		else
		{
			g_application.SetPlayWithMenu(false);
		}

#else

		dialog->Open();

    CFileItemPtr item_new = dialog->GetSelectedItem();
    if (!item_new || dialog->GetSelectedLabel() < 0)
    {
      CLog::Log(LOGDEBUG, "CGUIWindowVideoBase::ShowPlaySelection - User aborted %s", directory.c_str());
      break;
    }

#endif

    if (item_new->m_bIsFolder == false)
    {
      std::string original_path = item.GetPath();
      item.Reset();
			item = *item_new;

#ifdef HAS_VIDONME
			if (item.GetProperty("type") != "DVDFolder" && item.GetProperty("type") != "BDFolder")
#endif

      item.SetProperty("original_listitem_url", original_path);
      return true;
    }

    items.Clear();
    if (!XFILE::CDirectory::GetDirectory(item_new->GetPath(), items, XFILE::CDirectory::CHints(), true) || items.IsEmpty())
    {
      CLog::Log(LOGERROR, "CGUIWindowVideoBase::ShowPlaySelection - Failed to get any items %s", item_new->GetPath().c_str());
      break;
    }
  }

  return false;
}
