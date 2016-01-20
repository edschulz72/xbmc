#ifndef __IVD_COREPLAYER_H__
#define __IVD_COREPLAYER_H__

#include "IVD_coreplayer_decls.h"
#include "IVD_playtool_decls.h"
//==========================================================================================================================//
// ** class IVDCorePlayer

namespace libvp
{

class IVDCorePlayer
{
public:

	IVDCorePlayer(IVDPlayCallback*){}
	virtual ~IVDCorePlayer(){}

	//only for android system player
	virtual void prepare() {}
	//only for android system player
	virtual void prepareAsync() {}
	//only for android system player
	virtual void doPlay(){}
	//only for android system player
	virtual void reset(){}

	// Client can delete pPlcoreFileItem and pPlcorePlayerOptions after this call done.
	//Ignore pPlcoreFileItem and pPlcorePlayerOptions if you want to playback vms source
	// Ignore pVmsParams if you want to playback local source
	virtual bool OpenFile(IVDFileItem* pPlcoreFileItem, IVDPlayOptions* pPlcorePlayerOptions,libvp::vidonPlayerParam* pVmsParams) = 0;
	virtual void CloseFile() = 0;

	virtual VD_PlayerVideoStreamInfo* GetVideoStreamInfo() = 0;
	virtual void ReleaseVideoStreamInfo(VD_PlayerVideoStreamInfo* pInfo) = 0;

	virtual VD_PlayerAudioStreamInfo* GetAudioStreamInfo(int streamIndex) = 0;
	virtual void ReleaseAudioStreamInfo(VD_PlayerAudioStreamInfo* pInfo) = 0;

	virtual VD_PlayerSubtitleStreamInfo* GetSubtitleStreamInfo(int streamIndex) = 0;
	virtual void ReleaseSubtitleStreamInfo(VD_PlayerSubtitleStreamInfo* pInfo) = 0;

	virtual void Pause() = 0;
	virtual bool IsPlaying() const = 0;
	virtual bool IsPaused() const = 0;

	virtual bool HasVideo() const = 0;
	virtual bool HasAudio() const = 0;

	//total time in seconds
	virtual double GetTotalTime() = 0;

  //current time in seconds
	virtual double GetTime() = 0;

	//Pausing player before seek can make seek faster.
	//There are two situation:
	//1,Single seek.You should pause player before seek and resume player after seek.
	//2,Continuous seek.You should pause player before the first seek and resume after the last seek.
	//Seeking will faster if you set bAccurate to false, but that seeking is not accurate.
	//time in seconds.
	virtual void SeekTime(double time, bool bAccurate = true) = 0;

	virtual bool ControlsVolume() = 0;

	virtual int  GetAudioStreamCount() = 0;
	//Get current stream index( normal index >= 0)
	virtual int  GetAudioStream() = 0;
	virtual void SetAudioStream(int iStream) = 0;

	virtual int  GetSubtitleCount(bool bUpdate = false) = 0;
	virtual int  GetSubtitle() = 0;
	virtual void SetSubtitle(int iStream) = 0;

	//To detemine whether current subtitle support subtitle settings change (e.g. font size, color...)
	virtual bool IsCurrentSubtitleSupportUserSettings() = 0;

	//Set playback speed
	//iSpeed = 1 means normal speed, = 0 means pause
	virtual void ToFFRW(float iSpeed = 0) = 0;
	virtual float GetPlaySpeed() = 0;

	virtual int  GetChapterCount() = 0;
	virtual int  GetChapter() = 0;
	virtual char* GetChapterName() = 0;
	virtual void ReleaseChapterName(char* chapterName) = 0;
	virtual int  SeekChapter(int iChapter) = 0;

	//amount1,amount2 mean mouse location(x,y) when mouse action
	//amount1 means channel when CHANNEL_SWITCH action
	//return false to inform the client we didn't handle the action
	virtual bool OnAction(VD_Action id, float amount1 = 1.0F, float amount2 = 0.0F) = 0;

	//Return true means menu mode and in menu right now
	virtual bool IsInMenu() const  = 0;
	//Return true just means menu mode but not sure if in menu right now
	virtual bool HasMenu() = 0;

	//For non-kodi user: ask jun.jiang about the format info
	virtual char* GetAudioInfo() = 0;
	virtual void ReleaseAudioInfo(char* pAudioInfo) = 0;
	virtual char* GetVideoInfo() = 0;
	virtual void ReleaseVideoInfo(char* pVideoInfo) = 0;
	virtual char* GetGeneralInfo() = 0;
	virtual void ReleaseGeneralInfo(char* pGeneralInfo) = 0;

	virtual void Seek(bool bPlus = true, bool bLargeStep = false, bool bChapterOverride = false) = 0;

	//dvd's state
	//returns a state that is needed for resuming from a specific time
	virtual char* GetPlayerState() = 0;
	virtual void ReleasePlayerState(char* pPlayerState) = 0;
	virtual bool SetPlayerState(const char* state) = 0;

	//Returns true if not playback (paused or stopped beeing filled)
	virtual bool IsCaching() const = 0;
	//Cache filled in Percent
	virtual int GetCacheLevel() const = 0;

	virtual float GetCachePercentage() = 0;

	//strSubPath is external sub file path
	//return index(-1 means failed)
	virtual int AddSubtitle(const char* strSubPath) = 0;

	virtual bool CanPause() = 0;
	virtual bool IsPassthrough() const = 0;
	virtual bool CanSeek() = 0;
	virtual bool SeekScene(bool bPlus = true) = 0;
	//fPercent [0,100]
	virtual void SeekPercentage(float fPercent = 0, bool bAccurate = true) = 0;
	virtual float GetPercentage() = 0;

	virtual bool CanRecord() = 0;
	virtual bool IsRecording() = 0;
	virtual bool Record(bool bOnOff) = 0;

	virtual char* GetPlayingTitle() = 0;
	virtual void ReleasePlayingTitle(char* p) = 0;

  virtual libvp::vidonOutInfo*  GetVMFOutInfo() = 0;
  virtual int64_t ReadFileData( const char* pReadFileName,const char* pSaveFileName, int64_t nStartPos, int64_t nLenght ) = 0;
  virtual bool GetTitleInfo(int &iTitleNum, bool &bMainTitle) = 0;
  virtual int  GetErrorNum() = 0;
protected:

};

}

#endif //__IVD_COREPLAYER_H__
