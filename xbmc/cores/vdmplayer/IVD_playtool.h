#ifndef __IVD_PLAYTOOL_H__
#define __IVD_PLAYTOOL_H__

#include"IVD_playtool_decls.h"
#include"IVD_coreplayer.h"

//==========================================================================================================================//
// ** class IVDPlayTool

namespace libvp
{

class IVDPlayTool 
{
public:

	IVDPlayTool(IVDPlayToolConfig* pConfig, IVDPlaytoolCallback* pCallback){}
	virtual ~IVDPlayTool(){}

	virtual bool Init() = 0;
	virtual bool UnInit() = 0;

	//Call this after your client Resized render window.
	virtual void RenderWndResized(int width, int height) = 0;

	//Return NULL if failed
	virtual IVDCorePlayer* GetPlayer(IVDPlayCallback* pCallback) = 0;
	virtual void ReleasePlayer(IVDCorePlayer* pCorePlayer) = 0;

	// Non-Kodi Client can call this to save cpu usage when there is no need for rendering, e.g. when pausing and hiding app.
  // And ios client has to call this to stop opengl operation before go into background or ios system will kill us.
  // Normally call order is:
  // Before go into background pause playback, then call SuspendRenderSystem(true);
  // After go into foreground call SuspendRenderSystem(false), resume playback.
  // This method blocks until status is switched.
	// default is false.
	virtual void SuspendRenderSystem(bool suspend) = 0;

	// These four methods handle the settings that kodi's CSettings class handled.
	// The settings include audio output, video acceleration, network, video player, VideoScreen, subtitles, dvds, disc...
	// The argus require the same value as kodi's CSettings.
	// And for non-kodi client developer, we defined those id as macro, and described them.
	// Those macros start with VD_CSETTING_ID_. Please go to learn them before use these methods.
	virtual bool SetCSettings(const char* id, const char* value) = 0;
	virtual bool SetCSettings(const char* id, double value) = 0;
	virtual bool SetCSettings(const char* id, int value) = 0;
	virtual bool SetCSettings(const char* id, bool value) = 0;


	//DO NOT change the content returned, only use it
	//DO NOT forget to call ReleaseEnumeratedAudioOutputDevices when you do not use the returned any more
	virtual VD_AudioOutputDeviceNode* EnumerateAudioOutputDevices(bool passthrough) = 0;
	virtual void ReleaseEnumeratedAudioOutputDevices(VD_AudioOutputDeviceNode* pHeadNode) = 0;

	//KODI client should call this when call CAEFactory::OnSettingsChange.
	virtual void NotifyAudioOutputSettingsChanged() = 0;

	//Return the device name
	virtual char* GetDefaultAudioDevice(bool passthrough) = 0;
	virtual void ReleaseDefaultAudioDevice(char* p) = 0;


	//Default is VD_DEINTERLACE_MODE_AUTO
	//This change works immediately while playing back.
	virtual void SetDeinterlaceMode(VD_DeinterlaceMode mode) = 0;

	//Default is VD_INTERLACE_METHOD_NONE_AUTO
	//This change works immediately while playing back.
	virtual void SetInterlaceMethod(VD_InterlaceMethod mode) = 0;

	//Default is VD_SCALING_METHOD_LINEAR
	//This change works immediately while playing back.
	virtual void SetScalingMethod(VD_ScalingMethod mode) = 0;

	//[0.5, 2.0]  default: 1.0 means normal
	//You have to call SetViewMode with argu VD_VIEW_MODE_CUSTOM (if current mode is not custom), 
	//and call NotifyViewModeChanged (whatever current mode is or not custom) to let this change work immediately while playing back.
	virtual void SetCustomZoomAmount(float Value) = 0;

	//[0.5, 2.0]  default: 1.0 means normal
	//You have to call SetViewMode with argu VD_VIEW_MODE_CUSTOM (if current mode is not custom), 
	//and call NotifyViewModeChanged (whatever current mode is or not custom) to let this change work immediately while playing back.
	virtual void SetCustomPixelRatio(float Value) = 0;

	//[-2.0, 2.0] default: 0 means no shift
	//You have to call SetViewMode with argu VD_VIEW_MODE_CUSTOM (if current mode is not custom), 
	//and call NotifyViewModeChanged (whatever current mode is or not custom) to let this change work immediately while playing back.
	virtual void  SetCustomVerticalShift(float Value) = 0;

	//default: false
	//You have to call SetViewMode with argu VD_VIEW_MODE_CUSTOM (if current mode is not custom), 
	//and call NotifyViewModeChanged (whatever current mode is or not custom) to let this change work immediately while playing back.
	virtual void  SetCustomNonLinStretch(bool Value) = 0;

	//default: VD_VIEW_MODE_NORMAL
	//You have to call NotifyViewModeChanged to let this change work immediately while playing back.
	virtual void SetViewMode(VD_ViewMode mode) = 0;

	//KODI client should call this when call g_renderManager.SetViewMode.
	virtual void NotifyViewModeChanged() = 0;

  // Default is support(ios is non-support in Default)
  // This would not work immediately during playback
  virtual void SetWhetherSupportAC3(bool bSupport) = 0;

  // Default is support
  // This works immediately during playback
  virtual void SupportDTS(bool bSupport) = 0;

	//volume(0-1.0f)
	virtual void SetVolume(float Volume) = 0;

	virtual void SetMute(bool bmute) = 0;

	//Default is 0
	//[0,60]
	//This change works immediately when playing back.
	virtual void SetVolumeAmplification(float Value) = 0;

	//Default is 0
	//in seconds
	//To let video faster or slower to make av sync(video rendering pts = video own pts + fValue * TIME_BASE)
	//This change works immediately when playing back.
	virtual void  SetAudioDelay(float fdelay) = 0;
	virtual float GetAudioDelay() = 0;

	//Default is 0
	//in seconds
	//To let subtitle faster or slower(subtitle rendering pts = subtitle own pts - (-fdelay) * TIME_BASE)
	//This change works immediately when playing back.
	virtual void  SetSubTitleDelay(float fdelay) = 0;
	virtual float GetSubTitleDelay() = 0;

	//Default is true. But after OpenFile, plcore may change it automatically
	//This change works immediately when playing back.
	virtual void SetSubtitleOn(bool bOn) = 0;	
	virtual bool GetSubtitleOn() = 0;

	//NOTICE only used by VDPlayer middle layer
	virtual void SetSubtitleOnEx(bool bOn) = 0;
	virtual bool GetSubtitleOnEx() = 0;

	// value's format has to be ARGB
	// Default is white
  //This change works immediately when playing back.
	virtual void SetSubColor(unsigned int color) = 0;
  virtual unsigned int  GetSubColor() = 0;
  //Default is 28
  //[16,74]
  //This change works immediately when playing back.
  virtual bool SetSubtitleSize(int size) = 0;
  virtual  int GetSubtitleSize() = 0;
	// yPos valid when align is VD_SUBTITLE_ALIGN_MANUAL
	// yPos [0,1]. 0 means top, 1 means bottom. Default is 1.
	// Default align is VD_SUBTITLE_ALIGN_MANUAL
  //This change works immediately when playing back.
	virtual void  SetSubtitlePos(VD_SubtitleAlign align, float yPos) = 0;
  virtual float GetSubtitlePos() = 0;

  // Default is false
  virtual void SetSubtitleBold(bool bBold) = 0;
  virtual bool GetSubtitleBold() = 0;
  // Default is false
  virtual void SetSubtitleItalic(bool bItalic) = 0;
  virtual bool GetSubtitleItalic() = 0;
	//Default is 50
	//0-100
	//This change works immediately when playing back.
	virtual void SetBrightness(float fBrightness) = 0;

	//Default is 50
	//0-100
	//This change works immediately when playing back.
	virtual void SetContrast(float fContrast) = 0;

	//0-100
	virtual void SetHue(float fHue) = 0;
	//0-100
	virtual void SetSaturation(float fSaturation) = 0;

	//0-1.0
	//default is 0
	virtual void SetVdpauNoiseRedution(float f) = 0;

	virtual void SetRotationDegree(VD_RotationDegree degree) = 0;

	virtual void SetMirrorStyle(VD_MirrorStyle style) = 0;

	//Default is false
	//for mplayer's "high-quality filter combination"
	//This change works immediately when playing back.
	virtual void SetPostProcessOn(bool on) = 0;

	//Default is false
	//This change works immediately when playing back.
	virtual void SetCropOn(bool on) = 0;

	//Default is VD_STEREOSCOPIC_MODE_AUTO
	//This change works immediately when playing back.
	virtual void SetStereoMode(VD_StereoscopicMode mode) = 0;

	//Default is false
	//This change works immediately when playing back.
	virtual void SetStereoInvert(bool on) = 0;

  // Plcore will use ass subtitle original settings to display ass subtitle if bOver is false, otherwise use client's settings.
  // Default is false.
  // This change works immediately when playing back.
  virtual void SetWhetherOverAssOrigSettings(bool bOver) = 0;
 
  virtual bool IsDisplay3DSupported() = 0;
 
  virtual void SetDisplayMode(VD_DisplayMode mode) = 0;

  virtual void SetSubtitleStyle(int nStyle) = 0;

  virtual int  GetSubtitleStyle() = 0;

  virtual void SetDefaultSubtitleProperty(unsigned int nSubColor,int  nSubSize,float fSubPos,int nStyle,float fdelay) = 0;

#if defined(VDPLAYER_FOR_KODI)
	// Kodi client has to call this in its render thread before call Init.
  virtual void BindRenderThreadID(){}

	//kodi client call this to render in its render thread
	virtual void RenderManagerRender(bool clear, unsigned int flags = 0, unsigned int alpha = 255){}

	virtual void RenderManagerFlush() {}

  virtual void RenderManagerFrameMove(){}

	virtual bool RenderManagerIsStarted(){ return false;}

	virtual void RenderManagerFrameFinish(){}

	virtual void RenderManagerUpdate(){}

	virtual VD_Resolution RenderManagerGetResolution(){ return VD_RESOLUTION_INVALID;}

	virtual void RenderManagerSetupScreenshot(){}

	virtual float RenderManagerGetAspectRatio(){return 0;}

	virtual bool RenderManagerSupports(VD_DeinterlaceMode mode){return false;}
	virtual bool RenderManagerSupports(VD_InterlaceMethod mode){return false;}
	virtual bool RenderManagerSupports(VD_RenderFeature mode){return false;}
	virtual bool RenderManagerSupports(VD_ScalingMethod mode){return false;}

	virtual void RenderManagerSetViewMode(VD_ViewMode mode){}

	virtual char* RenderManagerGetVSyncState(){return NULL;}
	virtual void RenderManagerReleaseVSyncState(char* p){}

	virtual void RenderManagerFrameWait(int ms){}
	virtual void RenderManagerUpdateResolution(){}
	virtual void RenderManagerManageCaptures(){}

	//TODO: new interfaces of kodi 15
	//virtual bool RenderManagerIsGuiLayer(){}
	//virtual bool RenderManagerIsVideoLayer(){}
	//virtual bool RenderManagerHasFrame(){}

#if defined(HAVE_X11)
	//for kodi Client
	virtual void SetWindow(Window wnd) {}
#endif

	//for kodi Client
	virtual void SetScreen(int screen){}

#if defined(HAS_DX)
	virtual void SetAdapter(unsigned int adapter) {}
	virtual void SetD3DPP(D3DPRESENT_PARAMETERS pp){}
#endif

	virtual void SetStereoMode(VD_RenderStereoMode mode, VD_RenderStereoView view) {}
	virtual void SetWindowResolution(int width, int height) {}

	virtual void SetGraphicContextStereoMode(VD_RenderStereoMode mode) {}
	virtual void SetGraphicContextStereoView(VD_RenderStereoView view) {}
	virtual void SetGraphicContextFullScreenRoot(bool on) {}

	// VDPLAYER_FOR_KODI should call this after CGraphicContext::SetFullScreenVideo finished. 
	virtual void SetGraphicContextFullScreenVideo(bool on) {}
	virtual void SetGraphicContextCalibrating(bool on) {}

	// VDPLAYER_FOR_KODI should call this after CGraphicContext::SetVideoResolutionInternal finished. 
	virtual void SetGraphicContextVideoResolution(VD_Resolution res, bool bForce = true) {}

	virtual void SetGraphicContextVideoRect(float x1, float y1, float x2, float y2) {}
	virtual void SetGraphicContextScreenWidth(int n) {}
	virtual void SetGraphicContextScreenHeight(int n) {}
	virtual void SetGraphicContextScissors(float x1, float y1, float x2, float y2){}

	virtual void SetRenderViewPort(float x1, float y1, float x2, float y2) {}
	virtual void SetMaxTextureSize(unsigned int size) {}

	virtual void AEDeviceChange() {}

#endif // VDPLAYER_FOR_KODI

  // strSaveUrl has to be ended by .jpg.
  // Use render window size in case of ( nWidth <= 0 || nWidth > render window width )
  // Kodi client has to call this in render thread.It is better to call this after RenderManagerRender
  virtual bool CaptureRenderImage(const char* strSaveUrl, int nWidth) = 0;

protected:

};

}

#endif //__IVD_PLAYTOOL_H__
