
#ifdef HAS_VIDONME

#pragma once

#include "threads/Thread.h"

#include "cores/IPlayer.h"
#include "cores/vdmplayer/VidonCommon.h"
#include "cores/vdmplayer/IVD_player.h"
#include "cores/vdmplayer/IVD_playtool.h"
#include "cores/vdmplayer/IVD_coreplayer.h"
#include "cores/vdmplayer/VDMPlayerConfig.h"
#include "cores/VideoRenderers/OverlayRendererGUI.h"

using namespace libvp;

class CVDMPlayer : public IPlayer, public CThread
{
public:
	CVDMPlayer(IPlayerCallback& callback);
	virtual ~CVDMPlayer();

	virtual bool Initialize(TiXmlElement* pConfig);

	virtual bool OpenFile(const CFileItem& file, const CPlayerOptions &options);
	virtual bool QueueNextFile(const CFileItem &file);
	virtual void OnNothingToQueueNotify();
	virtual bool CloseFile(bool reopen = false);

	virtual bool IsPlaying() const;
	virtual bool CanPause();
	virtual void Pause();
	virtual bool IsPaused() const;
	virtual bool HasVideo() const;
	virtual bool HasAudio() const;

	virtual bool IsPassthrough() const;
	virtual bool CanSeek();
	virtual void Seek(bool bPlus = true, bool bLargeStep = false, bool bChapterOverride = false);
	virtual bool SeekScene(bool bPlus = true);
	virtual void SeekPercentage(float fPercent = 0);
	virtual float GetPercentage();
	virtual float GetCachePercentage();
	virtual void SetMute(bool bOnOff);
	virtual void SetVolume(float volume);
	virtual bool ControlsVolume();
	virtual void SetDynamicRangeCompression(long drc);
	virtual void GetAudioInfo(std::string& strAudioInfo);
	virtual void GetVideoInfo(std::string& strVideoInfo);
	virtual void GetGeneralInfo(std::string& strGeneralInfo);
	virtual bool CanRecord();
	virtual bool IsRecording();
	virtual bool Record(bool bOnOff);

	virtual void  SetAVDelay(float fValue = 0.0f);
	virtual float GetAVDelay();

	virtual void SetSubTitleDelay(float fValue = 0.0f);
	virtual float GetSubTitleDelay();
	virtual int GetSubtitleCount();
	virtual int GetSubtitle();
	virtual void GetSubtitleStreamInfo(int index, SPlayerSubtitleStreamInfo &info);
	virtual void SetSubtitle(int iStream);
	virtual void SetSubtitleWhetherOverAssOrig(bool bOver);
	virtual bool GetSubtitleVisible();
	virtual void SetSubtitleVisible(bool bVisible);
	virtual void AddSubtitle(const std::string& strSubPath);

	virtual int  GetAudioStreamCount();
	virtual int  GetAudioStream();
	virtual void SetAudioStream(int iStream);
	virtual void GetAudioStreamInfo(int index, SPlayerAudioStreamInfo &info);

	virtual TextCacheStruct_t* GetTeletextCache();
	virtual void LoadPage(int p, int sp, unsigned char* buffer);

	virtual int  GetChapterCount();
	virtual int  GetChapter();
	virtual void GetChapterName(std::string& strChapterName, int chapterIdx = -1);
	virtual int64_t GetChapterPos(int chapterIdx = -1);
	virtual int  SeekChapter(int iChapter);

	virtual void SeekTime(int64_t iTime = 0);
	virtual bool SeekTimeRelative(int64_t iTime);
	virtual int64_t GetTime();
	virtual int64_t GetDisplayTime();
	virtual int64_t GetTotalTime();
	virtual void GetVideoStreamInfo(SPlayerVideoStreamInfo &info);
	virtual int GetSourceBitrate();
	virtual bool GetStreamDetails(CStreamDetails &details);
	virtual void ToFFRW(int iSpeed = 0);
	virtual bool SkipNext();

	virtual bool IsCaching() const;
	virtual int GetCacheLevel() const;

	virtual bool IsInMenu() const;
	virtual bool HasMenu();

	virtual void DoAudioWork();
	virtual bool OnAction(const CAction &action);

	virtual std::string GetPlayerState();
	virtual bool SetPlayerState(const std::string& state);

	virtual std::string GetPlayingTitle();

	virtual bool SwitchChannel(const PVR::CPVRChannelPtr &channel);

	virtual void GetAudioCapabilities(std::vector<int> &audioCaps);
	virtual void GetSubtitleCapabilities(std::vector<int> &subCaps);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual bool IsSelfPresent();
	virtual void Present();
	virtual void SetPlayMode(DIMENSIONMODE mode);
	virtual void UpdateWindowSize(void);

	virtual void NotifyAudioOutputSettingsChanged();
	virtual void SetDeinterlaceMode(EDEINTERLACEMODE mode);
	virtual void SetInterlaceMethod(EINTERLACEMETHOD mode);
	virtual void SetScalingMethod(ESCALINGMETHOD mode);
	virtual void SetCustomZoomAmount(float Value);
	virtual void SetCustomPixelRatio(float Value);
	virtual void SetCustomVerticalShift(float Value);
	virtual void SetCustomNonLinStretch(bool Value);
	virtual void SetViewMode(ViewMode mode);
	virtual void NotifyViewModeChanged();

	virtual void SetWhetherSupportAC3(bool bSupport);
	virtual float GetAudioDelay();
	virtual bool GetSubtitleOn();

	virtual void SetSubColor(unsigned int color);
	virtual bool SetSubtitleSize(int size);
	virtual void SetSubtitlePos(SubtitleAlign align, float yPos);
	virtual void SetSubtitleStyle(int nStyle);
	virtual void SetSubtitleBold(bool bBold);
	virtual void SetSubtitleItalic(bool bItalic);

	virtual void SetBrightness(float fBrightness);
	virtual void SetContrast(float fContrast);
	virtual void SetHue(float fHue);
	virtual void SetSaturation(float fSaturation);
	virtual void SetVdpauNoiseRedution(float f);
	virtual void SetPostProcessOn(bool on);
	virtual void SetCropOn(bool on);
	virtual void SetStereoInvert(bool on);

	virtual void BindRenderThreadID();
	virtual void RenderManagerRender(bool clear, unsigned int flags = 0, unsigned int alpha = 255);
	virtual void RenderManagerFlush() ;
	virtual void RenderManagerFrameMove();
	virtual bool RenderManagerIsStarted();
	virtual void RenderManagerFrameFinish();
	virtual void RenderManagerUpdate();
	virtual RESOLUTION RenderManagerGetResolution();
	virtual void RenderManagerSetupScreenshot();
	virtual float RenderManagerGetAspectRatio();

	virtual bool RenderManagerSupports(EDEINTERLACEMODE mode);
	virtual bool RenderManagerSupports(EINTERLACEMETHOD mode);
	virtual bool RenderManagerSupports(ERENDERFEATURE mode);
	virtual bool RenderManagerSupports(ESCALINGMETHOD mode);

	virtual void RenderManagerSetViewMode(ViewMode mode);

	virtual std::string RenderManagerGetVSyncState();

	virtual void RenderManagerFrameWait(int ms);
	virtual void RenderManagerUpdateResolution();
	virtual void RenderManagerManageCaptures();

	virtual bool SetCSettings(const char* id, const char* value);
	virtual bool SetCSettings(const char* id, double value);
	virtual bool SetCSettings(const char* id, int value);
	virtual bool SetCSettings(const char* id, bool value);

	virtual void SetScreen(int screen);

#if defined(HAS_DX)
	virtual void SetAdapter(unsigned int adapter);
	virtual void SetD3DPP(D3DPRESENT_PARAMETERS pp);
#endif

	virtual void SetStereoMode(RENDER_STEREO_MODE mode);
	virtual void SetStereoMode(RENDER_STEREO_MODE mode, RENDER_STEREO_VIEW view);
	virtual void SetWindowResolution(int width, int height);

	virtual void SetGraphicContextStereoMode(RENDER_STEREO_MODE mode);
	virtual void SetGraphicContextStereoView(RENDER_STEREO_VIEW view);
	virtual void SetGraphicContextFullScreenRoot(bool on);

	virtual void SetGraphicContextFullScreenVideo(bool on);
	virtual void SetGraphicContextCalibrating(bool on);

	virtual void SetGraphicContextVideoResolution(RESOLUTION res, bool bForce = true);

	virtual void SetGraphicContextVideoRect(float x1, float y1, float x2, float y2);
	virtual void SetGraphicContextScreenWidth(int n);
	virtual void SetGraphicContextScreenHeight(int n);
	virtual void SetGraphicContextScissors(float x1, float y1, float x2, float y2);

	virtual void SetRenderViewPort(float x1, float y1, float x2, float y2);
	virtual void SetMaxTextureSize(unsigned int size);

	virtual void AEDeviceChange();

	virtual bool CaptureRenderImage(const char* strSaveUrl, int nWidth);

public:
	static bool InitPlayCore(void);
	static void DeInitPlayCore(void);

protected:
	virtual void Process();

private:
	bool InitPlayer(void);
	void DeInitPlayer(void);
	
	void SetSubttileFeatures(void);

	VD_RenderStereoMode ChangeStereoMode(RENDER_STEREO_MODE modeIn);
	VD_RenderStereoView ChangeStereoView(RENDER_STEREO_VIEW viewIn);
	VD_Resolution ChangeResolution(RESOLUTION viewIn);
	RESOLUTION ChangeResolutionEx(VD_Resolution viewIn);
	VD_DeinterlaceMode ChangeDeinterlaceMode(EDEINTERLACEMODE modeIn);
	VD_InterlaceMethod ChangeInterlaceMode(EINTERLACEMETHOD modeIn);
	VD_ScalingMethod ChangeScalingMethod(ESCALINGMETHOD modeIn);
	VD_ViewMode ChangeViewMod(ViewMode modeIn);
	VD_SubtitleAlign ChangeSubtitleAlign(SubtitleAlign alignIn);
	VD_RenderFeature ChangeRenderFeature(ERENDERFEATURE featureIn);

private:
	CEvent m_event;
	bool m_bIsPlaying;

	CVDMFileItem m_itemPlay;
	CVDMPlayOptions m_optionsPlay;

	bool m_bStreamInfoChanged;
	int m_nAudioStream;

private:
	IVDPlayTool* m_pPlayTool;
	IVDCorePlayer* m_pCorePlayer;
	CVDMPlayCallback* m_pPlcorePlayerCallback;
	CVDMPlayToolConfig* m_pPlayToolConfig;
	CVDMPlaytoolCallback* m_pPlcorePlaytoolCallback;
};

#endif
