
#ifndef __IVD_COREPLAYER_DECLS_H__
#define __IVD_COREPLAYER_DECLS_H__

#include <stddef.h>

// Indexs
// ** defines
// ** enums
// ** structs
// ** interface
// ** tool funcs

namespace libvp
{

#if defined(__GNUC__) && !defined(ANDROID)
// under gcc, inline will only take place if optimizations are applied (-O). this will force inline even whith optimizations.
#define FORCE_INLINE __attribute__((always_inline))
#else
#define FORCE_INLINE
#endif

//==========================================================================================================================//
// ** defines

#define VOLUME_MIN 0.0f        // -60dB
#define VOLUME_MAX 1.0f        // 0dB
#define PLCORE_TIME_BASE 1000000     

//==========================================================================================================================//
// ** enums

enum VD_Action 
{
	VD_ACTION_NONE,
	VD_ACTION_NEXT_ITEM,
	VD_ACTION_MOVE_RIGHT,
	VD_ACTION_MOVE_UP,
	VD_ACTION_SELECT_ITEM,//enter
	VD_ACTION_PREV_ITEM,
	VD_ACTION_SHOW_VIDEOMENU,
	VD_ACTION_PREVIOUS_MENU,
	VD_ACTION_NAV_BACK,
	VD_ACTION_MOVE_LEFT,
	VD_ACTION_MOVE_DOWN,
	VD_ACTION_MOUSE_MOVE,
	VD_ACTION_MOUSE_LEFT_CLICK,
	VD_ACTION_REMOTE_0,
	VD_ACTION_REMOTE_1,
	VD_ACTION_REMOTE_2,
	VD_ACTION_REMOTE_3,
	VD_ACTION_REMOTE_4,
	VD_ACTION_REMOTE_5,
	VD_ACTION_REMOTE_6,
	VD_ACTION_REMOTE_7,
	VD_ACTION_REMOTE_8,
	VD_ACTION_REMOTE_9,
	VD_ACTION_CHANNEL_UP,
	VD_ACTION_CHANNEL_DOWN,
	VD_ACTION_CHANNEL_SWITCH,
};

enum VD_SeekState
{
	VD_SEEK_STATE_NONE,
	VD_SEEK_STATE_CANNOT_SEEK,
	VD_SEEK_STATE_START,
	VD_SEEK_STATE_SUCCESSED,
	VD_SEEK_STATE_FAILED,
	VD_SEEK_STATE_GET_FIRST_AV_PACKET,
	VD_SEEK_STATE_DISPLAY_FIRST_PIC,
};
enum VD_PlayMode
{
  VD_PLAY_MODE_NONE = -1,           // file source
  VD_PLAY_MODE_DVDBD_MENU,          // dvd/bd menu mode , playlist -1
  VD_PLAY_MODE_DVDBD_MAINTITLE,     // dvd/bd main title, playlist 0xfffff
  VD_PLAY_MODE_DVDBD_OTHERTITLE,    // dvd/bd other title, playlist xxx
  VD_PLAY_MODE_VIDON_MENU,          // vidon dvd/bd menu mode, playlist  -1 (use MKUPlayer)
  VD_PLAY_MODE_VIDON_SIMPLE_MOVIES, // vidon dvd/bd simple mode ,movie disk, playlist -1  (use DVDPlayer)
  VD_PLAY_MODE_VIDON_SIMPLE_TV,     // vidon dvd/bd simple mode ,tv disk, playlist xxx  (use MKUPlayer)
};
//==========================================================================================================================//
// ** structs

struct VD_Rect
{
	float x1;
	float x2;
	float y1;
	float y2;
};

struct VD_PlayerVideoStreamInfo
{
	int bitrate;
	float videoAspectRatio;
	int height;
	int width;
	VD_Rect SrcRect;
	VD_Rect DestRect;

	char* language;
	char* name;
	char* videoCodecName;
	char* stereoMode;

	VD_PlayerVideoStreamInfo()
	{
		language = NULL;
		name = NULL;
		videoCodecName = NULL;
		stereoMode = NULL;

		bitrate = 0;
		videoAspectRatio = 1.0f;
		height = 0;
		width = 0;
	}

	~VD_PlayerVideoStreamInfo()
	{
		delete[] language;
		delete[] name;
		delete[] videoCodecName;
		delete[] stereoMode;
	}
};


struct VD_PlayerAudioStreamInfo
{
	int bitrate;
	int channels;
	int samplerate;
	int bitspersample;
	int nPhysicalId;
	char* language;
	char* name;
	char* audioCodecName;

	VD_PlayerAudioStreamInfo()
	{
		language = NULL;
		name = NULL;
		audioCodecName = NULL;

		bitrate = 0;
		channels = 0;
		samplerate = 0;
		bitspersample = 0;
		nPhysicalId = -1;
	}
	~VD_PlayerAudioStreamInfo()
	{
		delete[] language;
		delete[] name;
		delete[] audioCodecName;
	}
};

struct VD_PlayerSubtitleStreamInfo
{
	char* language;
	char* name;
	bool bExternalSub;
	int nPhysicalId; // invalid if bExternalSub == ture;
	char* filename; // invalid if bExternalSub == false;

	VD_PlayerSubtitleStreamInfo()
	{
		language = NULL;
		name = NULL;
    filename = NULL;
    nPhysicalId = -1;
	}
	~VD_PlayerSubtitleStreamInfo()
	{
		delete[] language;
		delete[] name;
		delete[] filename;
	}
};

class IVDPlayOptions
{
public:
	IVDPlayOptions() {};
	virtual ~IVDPlayOptions() {};

	// start time in seconds 
	virtual double GetStartTime() { return 0LL; };

	// start time in percent
	//[0,100]
	virtual double GetStartPercent() { return 0LL; };

	// potential playerstate to restore to
	//Our libdvdnav do not need this?(kodi need this to set libdvdnav state)
	virtual const char* GetState() = 0;

#if defined(VDPLAYER_FOR_KODI)
	// If true, playercore will invoke IVDPlaytoolCallback::OnSwitchToFullscreen() to notify kodi client to make sure no other window upon video screen.
	virtual bool IsFullscreen() { return true; };
#endif

	// player is not allowed to play audio streams, video streams only
	virtual bool IsVideoOnly() { return false; };

	//get playlist default -1
	virtual int GetPlaylist(){ return -1; };

	//get previously selected index
	virtual int GetPreSelectedAudioIdx(){ return -1; };

	//get previously selected index
	virtual int GetPreSelectedSubtitleIdx(){ return -1; };

  virtual bool        GetWhetherScanExternalSubtitle(){ return true; };

  virtual bool IsFromUpperKodi(){ return false; };
  virtual int  GetPlayMode(){ return VD_PLAY_MODE_NONE; };
};

class IVDFileItem
{
public:
	IVDFileItem() {};
	virtual ~IVDFileItem() {};

	//media file path
	virtual const char* GetPath() = 0;
	virtual const char* GetMimeType() = 0;
};


class IVDPlayCallback
{
public:
  IVDPlayCallback() {};
  virtual ~IVDPlayCallback() {};
  
  // ** for IPlayerCallback
  virtual void OnPlayBackEnded() = 0;
  virtual void OnPlayBackStarted() = 0;
  virtual void OnPlayBackPaused() {};
  virtual void OnPlayBackResumed() {};
  virtual void OnPlayBackStopped() = 0;
  virtual void OnQueueNextItem() = 0;
  // This callback means plcore put a client seek require in msg queue. Plcore will do the seek later.
  virtual void OnPlayBackSeek(int iTime, int seekOffset) {};
  virtual void OnPlayBackSeekChapter(int iChapter) {};
  virtual void OnPlayBackSpeedChanged(int iSpeed) {};
  virtual void OnPlayBackFailed() {};
  
  // ** own
  virtual void OnPlayBackSeekState(VD_SeekState state) {};
  virtual void OnPlayBackHwDecodeFailed() {};
  virtual void OnPlayBackDecodeModeNotify(bool Hw) {};
  virtual void OnOpenAC3() {};
  // Current audio stream is DTS
  virtual void OnOpenDTS() {};
};

}

#endif //__IVD_COREPLAYER_DECLS_H__
