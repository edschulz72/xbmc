#pragma once

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

#include <memory>
#include "threads/SystemClock.h"
#include "cores/playercorefactory/PlayerCoreFactory.h"

#ifdef HAS_VIDONME
#include "settings/VideoSettings.h"
#include "rendering/RenderSystem.h"
#include "guilib/Resolution.h"
#include "cores/VideoRenderers/OverlayRendererGUI.h"
#endif

typedef enum
{
  PLAYBACK_CANCELED = -1,
  PLAYBACK_FAIL = 0,
  PLAYBACK_OK = 1,
} PlayBackRet;

namespace PVR
{
  class CPVRChannel;
  typedef std::shared_ptr<PVR::CPVRChannel> CPVRChannelPtr;
}

class CAction;
class CPlayerOptions;
class CStreamDetails;

struct SPlayerAudioStreamInfo;
struct SPlayerVideoStreamInfo;
struct SPlayerSubtitleStreamInfo;
struct TextCacheStruct_t;

class CApplicationPlayer
{
  std::shared_ptr<IPlayer> m_pPlayer;
  unsigned int m_iPlayerOPSeq;  // used to detect whether an OpenFile request on player is canceled by us.
  PLAYERCOREID m_eCurrentPlayer;

  CCriticalSection  m_player_lock;

  // cache player state
  XbmcThreads::EndTime m_audioStreamUpdate;
  int m_iAudioStream;
  XbmcThreads::EndTime m_subtitleStreamUpdate;
  int m_iSubtitleStream;
  
public:
  CApplicationPlayer();

  int m_iPlaySpeed;

  // player management
  void CloseFile(bool reopen = false);
  void ClosePlayer();
  void ClosePlayerGapless(PLAYERCOREID newCore);
  void CreatePlayer(PLAYERCOREID newCore, IPlayerCallback& callback);
  PLAYERCOREID GetCurrentPlayer() const { return m_eCurrentPlayer; }
  std::shared_ptr<IPlayer> GetInternal() const;
  int  GetPlaySpeed() const;
  bool HasPlayer() const;
  PlayBackRet OpenFile(const CFileItem& item, const CPlayerOptions& options);
  void ResetPlayer() { m_eCurrentPlayer = EPC_NONE; }
  void SetPlaySpeed(int iSpeed, bool bApplicationMuted);

  // proxy calls
  void   AddSubtitle(const std::string& strSubPath);
  bool  CanPause();
  bool  CanRecord();
  bool  CanSeek();
  bool  ControlsVolume() const;
  void  DoAudioWork();
  void  GetAudioCapabilities(std::vector<int> &audioCaps);
  void  GetAudioInfo(std::string& strAudioInfo);
  int   GetAudioStream();
  int   GetAudioStreamCount();
  void  GetAudioStreamInfo(int index, SPlayerAudioStreamInfo &info);
  int   GetCacheLevel() const;
  float GetCachePercentage() const;
  int   GetChapterCount();
  int   GetChapter();  
  void  GetChapterName(std::string& strChapterName, int chapterIdx=-1);
  int64_t GetChapterPos(int chapterIdx=-1);
  void  GetDeinterlaceMethods(std::vector<int> &deinterlaceMethods);
  void  GetDeinterlaceModes(std::vector<int> &deinterlaceModes);
  void  GetGeneralInfo(std::string& strVideoInfo);
  float GetPercentage() const;
  std::string GetPlayerState();
  std::string GetPlayingTitle();
  int   GetPreferredPlaylist() const;
  void  GetRenderFeatures(std::vector<int> &renderFeatures);
  void  GetScalingMethods(std::vector<int> &scalingMethods);
  bool  GetStreamDetails(CStreamDetails &details);
  int   GetSubtitle();
  void  GetSubtitleCapabilities(std::vector<int> &subCaps);
  int   GetSubtitleCount();
  void  GetSubtitleStreamInfo(int index, SPlayerSubtitleStreamInfo &info);
  bool  GetSubtitleVisible();
  TextCacheStruct_t* GetTeletextCache();
  int64_t GetTime() const;
  int64_t GetDisplayTime() const;
  int64_t GetTotalTime() const;
  void  GetVideoInfo(std::string& strVideoInfo);
  void  GetVideoStreamInfo(SPlayerVideoStreamInfo &info);
  bool  HasAudio() const;
  bool  HasMenu() const;
  bool  HasVideo() const;
  bool  IsCaching() const;
  bool  IsInMenu() const;
  bool  IsPaused() const;
  bool  IsPausedPlayback() const;
  bool  IsPassthrough() const;
  bool  IsPlaying() const;
  bool  IsPlayingAudio() const;
  bool  IsPlayingVideo() const;
  bool  IsRecording() const;
  void  LoadPage(int p, int sp, unsigned char* buffer);
  bool  OnAction(const CAction &action);
  void  OnNothingToQueueNotify();
  void  Pause();
  bool  QueueNextFile(const CFileItem &file);
  bool  Record(bool bOnOff);
  void  Seek(bool bPlus = true, bool bLargeStep = false, bool bChapterOverride = false);
  int   SeekChapter(int iChapter);
  void  SeekPercentage(float fPercent = 0);
  bool  SeekScene(bool bPlus = true);
  void  SeekTime(int64_t iTime = 0);
  void  SeekTimeRelative(int64_t iTime = 0);
  void  SetAudioStream(int iStream);
  void  SetAVDelay(float fValue = 0.0f);
  void  SetDynamicRangeCompression(long drc);
  void  SetMute(bool bOnOff);
  bool  SetPlayerState(const std::string& state);
  void  SetSubtitle(int iStream);
  void  SetSubTitleDelay(float fValue = 0.0f);
  void  SetSubtitleVisible(bool bVisible);
  void  SetVolume(float volume);
  bool  SwitchChannel(const PVR::CPVRChannelPtr &channel);
  void  ToFFRW(int iSpeed = 0);

#ifdef HAS_VIDONME
	void Present();
	bool IsSelfPresent();
	void SetPlayMode(DIMENSIONMODE mode);
	void UpdateWindowSize(void);

	bool SetCSettings(const char* id, const char* value);
	bool SetCSettings(const char* id, double value);
	bool SetCSettings(const char* id, int value);
	bool SetCSettings(const char* id, bool value);

	void NotifyAudioOutputSettingsChanged();
	void SetDeinterlaceMode(EDEINTERLACEMODE mode);

	void SetInterlaceMethod(EINTERLACEMETHOD mode);
	void SetScalingMethod(ESCALINGMETHOD mode);
	void SetCustomZoomAmount(float Value);
	void SetCustomPixelRatio(float Value);
	void SetCustomVerticalShift(float Value);
	void SetCustomNonLinStretch(bool Value);
	void SetViewMode(ViewMode mode);
	void NotifyViewModeChanged();
	void SetWhetherSupportAC3(bool bSupport);
	float GetAudioDelay();
	bool GetSubtitleOn();
	void SetSubColor(unsigned int color);
	bool SetSubtitleSize(int size);
	void SetSubtitlePos(SubtitleAlign align, float yPos);
	void SetSubtitleStyle(int nStyle);
	void SetSubtitleBold(bool bBold);
	void SetSubtitleItalic(bool bItalic);
	void SetBrightness(float fBrightness);
	void SetContrast(float fContrast);
	void SetHue(float fHue);
	void SetSaturation(float fSaturation);
	void SetVdpauNoiseRedution(float f);
	void SetPostProcessOn(bool on);
	void SetCropOn(bool on);
	void SetStereoInvert(bool on);

	void BindRenderThreadID();
	void RenderManagerRender(bool clear, unsigned int flags = 0, unsigned int alpha = 255);
	void RenderManagerFlush();
	void RenderManagerFrameMove();
	bool RenderManagerIsStarted();
	void RenderManagerFrameFinish();
	void RenderManagerUpdate();
	RESOLUTION RenderManagerGetResolution();
	void RenderManagerSetupScreenshot();
	float RenderManagerGetAspectRatio();

	bool RenderManagerSupports(EDEINTERLACEMODE mode);
	bool RenderManagerSupports(EINTERLACEMETHOD mode);
	bool RenderManagerSupports(ERENDERFEATURE mode);
	bool RenderManagerSupports(ESCALINGMETHOD mode);

	void RenderManagerSetViewMode(ViewMode mode);

	std::string RenderManagerGetVSyncState();

	void RenderManagerFrameWait(int ms);
	void RenderManagerUpdateResolution();
	void RenderManagerManageCaptures();

	void SetScreen(int screen);

#if defined(HAS_DX)
	void SetAdapter(unsigned int adapter);
	void SetD3DPP(D3DPRESENT_PARAMETERS pp);
#endif
	void SetStereoMode(RENDER_STEREO_MODE mode);
	void SetStereoMode(RENDER_STEREO_MODE mode, RENDER_STEREO_VIEW view);
	void SetWindowResolution(int width, int height);

	void SetGraphicContextStereoMode(RENDER_STEREO_MODE mode);
	void SetGraphicContextStereoView(RENDER_STEREO_VIEW view);
	void SetGraphicContextFullScreenRoot(bool on);

	void SetGraphicContextFullScreenVideo(bool on);
	void SetGraphicContextCalibrating(bool on);

	void SetGraphicContextVideoResolution(RESOLUTION res, bool bForce = true);

	void SetGraphicContextVideoRect(float x1, float y1, float x2, float y2);
	void SetGraphicContextScreenWidth(int n);
	void SetGraphicContextScreenHeight(int n);
	void SetGraphicContextScissors(float x1, float y1, float x2, float y2);

	void SetRenderViewPort(float x1, float y1, float x2, float y2);
	void SetMaxTextureSize(unsigned int size);

	void AEDeviceChange();

	bool CaptureRenderImage(const char* strSaveUrl, int nWidth);

#endif 
};
