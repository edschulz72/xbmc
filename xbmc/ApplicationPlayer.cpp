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

#include "ApplicationPlayer.h"
#include "cores/IPlayer.h"
#include "Application.h"
#include "settings/MediaSettings.h"

#ifdef HAS_VIDONME
#include "guilib/GraphicContext.h"
#endif

CApplicationPlayer::CApplicationPlayer()
{
  m_iPlayerOPSeq = 0;
  m_eCurrentPlayer = EPC_NONE;
}

std::shared_ptr<IPlayer> CApplicationPlayer::GetInternal() const
{
  CSingleLock lock(m_player_lock);
  return m_pPlayer;
}

void CApplicationPlayer::ClosePlayer()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
  {
    CloseFile();
    // we need to do this directly on the member
    CSingleLock lock(m_player_lock);
    m_pPlayer.reset();
  }
}

void CApplicationPlayer::CloseFile(bool reopen)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
  {
    ++m_iPlayerOPSeq;
    player->CloseFile(reopen);
  }
}

void CApplicationPlayer::ClosePlayerGapless(PLAYERCOREID newCore)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (!player)
    return;

  bool gaplessSupported = (m_eCurrentPlayer == EPC_DVDPLAYER || m_eCurrentPlayer == EPC_PAPLAYER);
  gaplessSupported = gaplessSupported && (m_eCurrentPlayer == newCore);
  if (!gaplessSupported)
  {
    ClosePlayer();
  }
  else
  {
    // XXX: we had to stop the previous playing item, it was done in dvdplayer::OpenFile.
    // but in paplayer::OpenFile, it sometimes just fade in without call CloseFile.
    // but if we do not stop it, we can not distingush callbacks from previous
    // item and current item, it will confused us then we can not make correct delay
    // callback after the starting state.
    CloseFile(true);
  }
}

void CApplicationPlayer::CreatePlayer(PLAYERCOREID newCore, IPlayerCallback& callback)
{
  CSingleLock lock(m_player_lock);
  if (!m_pPlayer)
  {
    m_eCurrentPlayer = newCore;
    m_pPlayer.reset(CPlayerCoreFactory::Get().CreatePlayer(newCore, callback));
  }
}

PlayBackRet CApplicationPlayer::OpenFile(const CFileItem& item, const CPlayerOptions& options)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  PlayBackRet iResult = PLAYBACK_FAIL;
  if (player)
  {
    // op seq for detect cancel (CloseFile be called or OpenFile be called again) during OpenFile.
    unsigned int startingSeq = ++m_iPlayerOPSeq;

    iResult = player->OpenFile(item, options) ? PLAYBACK_OK : PLAYBACK_FAIL;
    // check whether the OpenFile was canceled by either CloseFile or another OpenFile.
    if (m_iPlayerOPSeq != startingSeq)
      iResult = PLAYBACK_CANCELED;
  }
  return iResult;
}

bool CApplicationPlayer::HasPlayer() const 
{ 
  std::shared_ptr<IPlayer> player = GetInternal();
  return player != NULL; 
}

int CApplicationPlayer::GetChapter()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetChapter();
  else 
    return -1;
}

int CApplicationPlayer::GetChapterCount()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetChapterCount();
  else 
    return 0;
}

void CApplicationPlayer::GetChapterName(std::string& strChapterName,
                                        int chapterIdx)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetChapterName(strChapterName, chapterIdx);
}

int64_t CApplicationPlayer::GetChapterPos(int chapterIdx)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetChapterPos(chapterIdx);

  return -1;
}

bool CApplicationPlayer::HasAudio() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->HasAudio());
}

bool CApplicationPlayer::HasVideo() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->HasVideo());
}

int CApplicationPlayer::GetPreferredPlaylist() const
{
  if (IsPlayingVideo())
    return PLAYLIST_VIDEO;

  if (IsPlayingAudio())
    return PLAYLIST_MUSIC;

  return PLAYLIST_NONE;
}

bool CApplicationPlayer::IsPaused() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->IsPaused());
}

bool CApplicationPlayer::IsPlaying() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->IsPlaying());
}

bool CApplicationPlayer::IsPausedPlayback() const
{
  return (IsPlaying() && IsPaused());
}

bool CApplicationPlayer::IsPlayingAudio() const
{
  return (IsPlaying() && !HasVideo() && HasAudio());
}

bool CApplicationPlayer::IsPlayingVideo() const
{
  return (IsPlaying() && HasVideo());
}

void CApplicationPlayer::Pause()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->Pause();
}

bool CApplicationPlayer::ControlsVolume() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->ControlsVolume());
}

void CApplicationPlayer::SetMute(bool bOnOff)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->SetMute(bOnOff);
}

void CApplicationPlayer::SetVolume(float volume)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->SetVolume(volume);
}

void CApplicationPlayer::Seek(bool bPlus, bool bLargeStep, bool bChapterOverride)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->Seek(bPlus, bLargeStep, bChapterOverride);
}

void CApplicationPlayer::SeekPercentage(float fPercent)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->SeekPercentage(fPercent);
}

bool CApplicationPlayer::IsPassthrough() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->IsPassthrough());
}

bool CApplicationPlayer::CanSeek()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->CanSeek());
}

bool CApplicationPlayer::SeekScene(bool bPlus)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->SeekScene(bPlus));
}

void CApplicationPlayer::SeekTime(int64_t iTime)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->SeekTime(iTime);
}

void CApplicationPlayer::SeekTimeRelative(int64_t iTime)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
  {
    // use relative seeking if implemented by player
    if (!player->SeekTimeRelative(iTime))
    {
      int64_t abstime = player->GetTime() + iTime;
      player->SeekTime(abstime);
    }
  }
}

std::string CApplicationPlayer::GetPlayingTitle()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetPlayingTitle();
  else
    return "";
}

int64_t CApplicationPlayer::GetTime() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetTime();
  else
    return 0;
}

int64_t CApplicationPlayer::GetDisplayTime() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetDisplayTime();
  else
    return 0;
}

bool CApplicationPlayer::IsCaching() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->IsCaching());
}

bool CApplicationPlayer::IsInMenu() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->IsInMenu());
}

bool CApplicationPlayer::HasMenu() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->HasMenu());
}

int CApplicationPlayer::GetCacheLevel() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetCacheLevel();
  else
    return 0;
}

int CApplicationPlayer::GetSubtitleCount()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetSubtitleCount();
  else
    return 0;
}

int CApplicationPlayer::GetAudioStream()
{
  if (!m_audioStreamUpdate.IsTimePast())
    return m_iAudioStream;

  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
  {
    m_iAudioStream = player->GetAudioStream();
    m_audioStreamUpdate.Set(1000);
    return m_iAudioStream;
  }
  else
    return 0;
}

int CApplicationPlayer::GetSubtitle()
{
  if (!m_subtitleStreamUpdate.IsTimePast())
    return m_iSubtitleStream;

  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
  {
    m_iSubtitleStream = player->GetSubtitle();
    m_subtitleStreamUpdate.Set(1000);
    return m_iSubtitleStream;
  }
  else
    return 0;
}

bool CApplicationPlayer::GetSubtitleVisible()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->GetSubtitleVisible());
}

bool CApplicationPlayer::CanRecord()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->CanRecord());
}

bool CApplicationPlayer::CanPause()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->CanPause());
}

bool CApplicationPlayer::IsRecording() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->IsRecording());
}

TextCacheStruct_t* CApplicationPlayer::GetTeletextCache()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetTeletextCache();
  else
    return NULL;
}

int64_t CApplicationPlayer::GetTotalTime() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetTotalTime();
  else
    return 0;
}

float CApplicationPlayer::GetPercentage() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetPercentage();
  else
    return 0.0;
}

float CApplicationPlayer::GetCachePercentage() const
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetCachePercentage();
  else
    return 0.0;
}

void CApplicationPlayer::ToFFRW(int iSpeed)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->ToFFRW(iSpeed);
}

void CApplicationPlayer::DoAudioWork()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->DoAudioWork();
}

std::string CApplicationPlayer::GetPlayerState()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetPlayerState();
  else
    return "";
}

bool CApplicationPlayer::QueueNextFile(const CFileItem &file)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->QueueNextFile(file));
}

bool CApplicationPlayer::GetStreamDetails(CStreamDetails &details)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->GetStreamDetails(details));
}

bool CApplicationPlayer::SetPlayerState(const std::string& state)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->SetPlayerState(state));
}

void CApplicationPlayer::OnNothingToQueueNotify()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->OnNothingToQueueNotify();
}

void CApplicationPlayer::GetVideoStreamInfo(SPlayerVideoStreamInfo &info)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetVideoStreamInfo(info);
}

void CApplicationPlayer::GetAudioStreamInfo(int index, SPlayerAudioStreamInfo &info)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetAudioStreamInfo(index, info);
}

bool CApplicationPlayer::OnAction(const CAction &action)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->OnAction(action));
}

bool CApplicationPlayer::Record(bool bOnOff)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->Record(bOnOff));
}

int  CApplicationPlayer::GetAudioStreamCount()
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->GetAudioStreamCount();
  else
    return 0;
}

void CApplicationPlayer::SetAudioStream(int iStream)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
  {
    player->SetAudioStream(iStream);
    m_iAudioStream = iStream;
    m_audioStreamUpdate.Set(1000);
    CMediaSettings::Get().GetCurrentVideoSettings().m_AudioStream = iStream;
  }
}

void CApplicationPlayer::GetSubtitleStreamInfo(int index, SPlayerSubtitleStreamInfo &info)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetSubtitleStreamInfo(index, info);
}

void CApplicationPlayer::SetSubtitle(int iStream)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
  {
    player->SetSubtitle(iStream);
    m_iSubtitleStream = iStream;
    m_subtitleStreamUpdate.Set(1000);
    CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleStream = iStream;
  }
}

void CApplicationPlayer::SetSubtitleVisible(bool bVisible)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
  {
    player->SetSubtitleVisible(bVisible);
    CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleOn = bVisible;
    CMediaSettings::Get().GetCurrentVideoSettings().m_SubtitleStream = player->GetSubtitle();
  }
}

void CApplicationPlayer::AddSubtitle(const std::string& strSubPath)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->AddSubtitle(strSubPath);
}

void CApplicationPlayer::SetSubTitleDelay(float fValue)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->SetSubTitleDelay(fValue);
}

void CApplicationPlayer::SetAVDelay(float fValue)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->SetAVDelay(fValue);
}

void CApplicationPlayer::SetDynamicRangeCompression(long drc)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->SetDynamicRangeCompression(drc);
}

bool CApplicationPlayer::SwitchChannel(const PVR::CPVRChannelPtr &channel)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  return (player && player->SwitchChannel(channel));
}

void CApplicationPlayer::LoadPage(int p, int sp, unsigned char* buffer)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->LoadPage(p, sp, buffer);
}

void CApplicationPlayer::GetAudioCapabilities(std::vector<int> &audioCaps)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetAudioCapabilities(audioCaps);
}

void CApplicationPlayer::GetSubtitleCapabilities(std::vector<int> &subCaps)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetSubtitleCapabilities(subCaps);
}

void CApplicationPlayer::GetAudioInfo(std::string& strAudioInfo)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetAudioInfo(strAudioInfo);
}

void CApplicationPlayer::GetVideoInfo(std::string& strVideoInfo)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetVideoInfo(strVideoInfo);
}

void CApplicationPlayer::GetGeneralInfo(std::string& strVideoInfo)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->GetGeneralInfo(strVideoInfo);
}

int  CApplicationPlayer::SeekChapter(int iChapter)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    return player->SeekChapter(iChapter);
  else
    return 0;
}

void CApplicationPlayer::GetRenderFeatures(std::vector<int> &renderFeatures)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->OMXGetRenderFeatures(renderFeatures);
}

void CApplicationPlayer::GetDeinterlaceMethods(std::vector<int> &deinterlaceMethods)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->OMXGetDeinterlaceMethods(deinterlaceMethods);
}

void CApplicationPlayer::GetDeinterlaceModes(std::vector<int> &deinterlaceModes)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->OMXGetDeinterlaceModes(deinterlaceModes);
}

void CApplicationPlayer::GetScalingMethods(std::vector<int> &scalingMethods)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (player)
    player->OMXGetScalingMethods(scalingMethods);
}

void CApplicationPlayer::SetPlaySpeed(int iSpeed, bool bApplicationMuted)
{
  std::shared_ptr<IPlayer> player = GetInternal();
  if (!player)
    return;

  if (!IsPlayingAudio() && !IsPlayingVideo())
    return ;
  if (m_iPlaySpeed == iSpeed)
    return ;
  if (!CanSeek())
    return;
  if (IsPaused())
  {
    if (
      ((m_iPlaySpeed > 1) && (iSpeed > m_iPlaySpeed)) ||
      ((m_iPlaySpeed < -1) && (iSpeed < m_iPlaySpeed))
    )
    {
      iSpeed = m_iPlaySpeed; // from pause to ff/rw, do previous ff/rw speed
    }
    Pause();
  }
  m_iPlaySpeed = iSpeed;

  ToFFRW(m_iPlaySpeed);

  // if player has volume control, set it.
  if (ControlsVolume())
  {
    if (m_iPlaySpeed == 1)
    { // restore volume
      player->SetVolume(g_application.GetVolume(false));
    }
    else
    { // mute volume
      player->SetVolume(VOLUME_MINIMUM);
    }
    player->SetMute(bApplicationMuted);
  }
}

int CApplicationPlayer::GetPlaySpeed() const
{
  return m_iPlaySpeed;
}

#ifdef HAS_VIDONME

void CApplicationPlayer::Present()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->Present();
}

bool CApplicationPlayer::IsSelfPresent()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->IsSelfPresent();

	return false;
}

void CApplicationPlayer::SetPlayMode(DIMENSIONMODE mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetPlayMode(mode);
}

void CApplicationPlayer::UpdateWindowSize()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->UpdateWindowSize();
}

bool CApplicationPlayer::SetCSettings(const char* id, const char* value)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->SetCSettings(id, value);

	return false;
}

bool CApplicationPlayer::SetCSettings(const char* id, double value)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->SetCSettings(id, value);

	return false;
}

bool CApplicationPlayer::SetCSettings(const char* id, int value)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->SetCSettings(id, value);

	return false;
}

bool CApplicationPlayer::SetCSettings(const char* id, bool value)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->SetCSettings(id, value);

	return false;
}

void CApplicationPlayer::NotifyAudioOutputSettingsChanged()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->NotifyAudioOutputSettingsChanged();
}

void CApplicationPlayer::SetDeinterlaceMode(EDEINTERLACEMODE mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetDeinterlaceMode(mode);
}

void CApplicationPlayer::SetInterlaceMethod(EINTERLACEMETHOD mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetInterlaceMethod(mode);
}

void CApplicationPlayer::SetScalingMethod(ESCALINGMETHOD mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetScalingMethod(mode);
}

void CApplicationPlayer::SetCustomZoomAmount(float Value)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetCustomZoomAmount(Value);
}

void CApplicationPlayer::SetCustomPixelRatio(float Value)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetCustomPixelRatio(Value);
}

void CApplicationPlayer::SetCustomVerticalShift(float Value)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetCustomVerticalShift(Value);
}

void CApplicationPlayer::SetCustomNonLinStretch(bool Value)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetCustomNonLinStretch(Value);
}

void CApplicationPlayer::SetViewMode(ViewMode mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetViewMode(mode);
}

void CApplicationPlayer::NotifyViewModeChanged()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->NotifyViewModeChanged();
}

void CApplicationPlayer::SetWhetherSupportAC3(bool bSupport)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetWhetherSupportAC3(bSupport);
}

float CApplicationPlayer::GetAudioDelay()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->GetAudioDelay();

	return 0;
}

bool CApplicationPlayer::GetSubtitleOn()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->GetSubtitleOn();
	
	return false;
}

void CApplicationPlayer::SetSubColor(unsigned int color)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetSubColor(color);
}

bool CApplicationPlayer::SetSubtitleSize(int size)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->SetSubtitleSize(size);

	return false;
}

void CApplicationPlayer::SetSubtitlePos(SubtitleAlign align, float yPos)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetSubtitlePos(align, yPos);
}

void CApplicationPlayer::SetSubtitleStyle(int nStyle)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetSubtitleStyle(nStyle);
}

void CApplicationPlayer::SetSubtitleBold(bool bBold)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetSubtitleBold(bBold);
}

void CApplicationPlayer::SetSubtitleItalic(bool bItalic)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetSubtitleItalic(bItalic);
}

void CApplicationPlayer::SetBrightness(float fBrightness)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetBrightness(fBrightness);
}

void CApplicationPlayer::SetContrast(float fContrast)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetContrast(fContrast);
}

void CApplicationPlayer::SetHue(float fHue)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetHue(fHue);
}

void CApplicationPlayer::SetSaturation(float fSaturation)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetSaturation(fSaturation);
}

void CApplicationPlayer::SetVdpauNoiseRedution(float f)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetVdpauNoiseRedution(f);
}

void CApplicationPlayer::SetPostProcessOn(bool on)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetPostProcessOn(on);
}

void CApplicationPlayer::SetCropOn(bool on)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetCropOn(on);
}

void CApplicationPlayer::SetStereoInvert(bool on)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetStereoInvert(on);
}

void CApplicationPlayer::BindRenderThreadID()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->BindRenderThreadID();
}

void CApplicationPlayer::RenderManagerRender(bool clear, unsigned int flags, unsigned int alpha)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerRender(clear, flags, alpha);
}

void CApplicationPlayer::RenderManagerFlush()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerFlush();
}

void CApplicationPlayer::RenderManagerFrameMove()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerFrameMove();
}

bool CApplicationPlayer::RenderManagerIsStarted()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->RenderManagerIsStarted();

	return false;
}

void CApplicationPlayer::RenderManagerFrameFinish()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerFrameFinish();
}

void CApplicationPlayer::RenderManagerUpdate()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerUpdate();
}

RESOLUTION CApplicationPlayer::RenderManagerGetResolution()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->RenderManagerGetResolution();

	return RES_INVALID;
}

void CApplicationPlayer::RenderManagerSetupScreenshot()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerSetupScreenshot();
}

float CApplicationPlayer::RenderManagerGetAspectRatio()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->RenderManagerGetAspectRatio();

	return 0;
}

bool CApplicationPlayer::RenderManagerSupports(EDEINTERLACEMODE mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->RenderManagerSupports(mode);

	return false;
}

bool CApplicationPlayer::RenderManagerSupports(EINTERLACEMETHOD mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->RenderManagerSupports(mode);

	return false;
}

bool CApplicationPlayer::RenderManagerSupports(ERENDERFEATURE mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->RenderManagerSupports(mode);

	return false;
}

bool CApplicationPlayer::RenderManagerSupports(ESCALINGMETHOD mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->RenderManagerSupports(mode);

	return false;
}

void CApplicationPlayer::RenderManagerSetViewMode(ViewMode mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerSetViewMode(mode);
}

std::string CApplicationPlayer::RenderManagerGetVSyncState()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->RenderManagerGetVSyncState();

	return "";
}

void CApplicationPlayer::RenderManagerFrameWait(int ms)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerFrameWait(ms);
}

void CApplicationPlayer::RenderManagerUpdateResolution()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerUpdateResolution();
}

void CApplicationPlayer::RenderManagerManageCaptures()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->RenderManagerManageCaptures();
}

void CApplicationPlayer::SetScreen(int screen)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetScreen(screen);
}

#if defined(HAS_DX)
void CApplicationPlayer::SetAdapter(unsigned int adapter)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetAdapter(adapter);
}

void CApplicationPlayer::SetD3DPP(D3DPRESENT_PARAMETERS pp)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetD3DPP(pp);
}
#endif

void CApplicationPlayer::SetStereoMode(RENDER_STEREO_MODE mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetStereoMode(mode);
}

void CApplicationPlayer::SetStereoMode(RENDER_STEREO_MODE mode, RENDER_STEREO_VIEW view)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetStereoMode(mode, view);
}

void CApplicationPlayer::SetWindowResolution(int width, int height)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetWindowResolution(width, height);
}

void CApplicationPlayer::SetGraphicContextStereoMode(RENDER_STEREO_MODE mode)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextStereoMode(mode);
}

void CApplicationPlayer::SetGraphicContextStereoView(RENDER_STEREO_VIEW view)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextStereoView(view);
}

void CApplicationPlayer::SetGraphicContextFullScreenRoot(bool on)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextFullScreenRoot(on);
}

void CApplicationPlayer::SetGraphicContextFullScreenVideo(bool on)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextFullScreenVideo(on);
}

void CApplicationPlayer::SetGraphicContextCalibrating(bool on)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextCalibrating(on);
}

void CApplicationPlayer::SetGraphicContextVideoResolution(RESOLUTION res, bool bForce)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player && g_graphicsContext.IsFullScreenVideo())
		player->SetGraphicContextVideoResolution(res, bForce);
}

void CApplicationPlayer::SetGraphicContextVideoRect(float x1, float y1, float x2, float y2)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextVideoRect(x1, y1, x2, y2);
}

void CApplicationPlayer::SetGraphicContextScreenWidth(int n)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextScreenWidth(n);
}

void CApplicationPlayer::SetGraphicContextScreenHeight(int n)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextScreenHeight(n);
}

void CApplicationPlayer::SetGraphicContextScissors(float x1, float y1, float x2, float y2)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetGraphicContextScissors(x1, y1, x2, y2);
}

void CApplicationPlayer::SetRenderViewPort(float x1, float y1, float x2, float y2)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player && g_graphicsContext.IsFullScreenVideo())
		player->SetRenderViewPort(x1, y1, x2, y2);
}

void CApplicationPlayer::SetMaxTextureSize(unsigned int size)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->SetMaxTextureSize(size);
}

void CApplicationPlayer::AEDeviceChange()
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		player->AEDeviceChange();
}

bool CApplicationPlayer::CaptureRenderImage(const char* strSaveUrl, int nWidth)
{
	std::shared_ptr<IPlayer> player = GetInternal();
	if (player)
		return player->CaptureRenderImage(strSaveUrl, nWidth);

	return false;
}

#endif 