#ifndef __IVD_MEDIAINFO_H__
#define __IVD_MEDIAINFO_H__

#include <stdint.h>
#include <stddef.h>

//==========================================================================================================================//
// IVDPlayer mediainfo
//
// Indexs
// A.some basic mediainfo define
// B.stream, playlist class

namespace libvp
{

//==========================================================================================================================//
// A.some basic mediainfo define

enum VD_STREAM_TYPE
{
  VD_STREAM_TYPE_UNDEFINED = -1,
  VD_STREAM_TYPE_VIDEO,
  VD_STREAM_TYPE_AUDIO,
  VD_STEAM_TYPE_DATA,
  VD_STREAM_TYPE_SUBPICTURE,
};

enum VD_VIDEO_RES
{
  VD_VIDEO_RES_UNKNOWN = 0,
  VD_VIDEO_RES_480I    = 1,    // 720 x 480   interlace
  VD_VIDEO_RES_576I    = 2,    // 720 x 576   interlace
  VD_VIDEO_RES_480P    = 3,    // 720 x 480
  VD_VIDEO_RES_1080I    = 4,    // 1920 x 1080 interlace
  VD_VIDEO_RES_720      = 5,    // 1280 x 720
  VD_VIDEO_RES_1080P    = 6,    // 1920 x 1080
  VD_VIDEO_RES_576P    = 7,    // 720 x 576

  VD_VIDEO_RES_704_480,
  VD_VIDEO_RES_704_576,
  VD_VIDEO_RES_352_480,
  VD_VIDEO_RES_352_576,
  VD_VIDEO_RES_352_240,
  VD_VIDEO_RES_352_288,
};

enum VD_VIDEO_AP
{
  VD_ASPECT_UNKNOWN  = 0,
  VD_ASPECT_4X3      = 2,
  VD_ASPECT_16X9      = 3,
  VD_ASPECT_2D35X1    = 4,    //2.35 : 1
  VD_ASPECT_1D66X1    = 5,    //1.66 : 1
  VD_ASPECT_1D85X1    = 6,    //1.85 : 1
  VD_ASPECT_2D20X1    = 7,    //2.20 : 1
};

enum VD_VIDEO_FPS
{
  VD_FPS_UNKNOWN    = 0,
  VD_FPS_23D976    = 1,    // 24000    / 1001
  VD_FPS_24        = 2,    // 24       / 1
  VD_FPS_25        = 3,    // 25       / 1
  VD_FPS_29D97      = 4,    // 30000    / 1001
  VD_FPS_30        = 5,    // 30       / 1
  VD_FPS_50        = 6,    // 50       / 1
  VD_FPS_59D94      = 7,    // 60000    / 1001
  VD_FPS_60        = 8,    // 60       / 1
};

enum VD_AUDIO_SAMPLING
{
  VD_SAMPLING_UNKNOWN    = 0,
  VD_SAMPLING_48KHZ      = 1,
  VD_SAMPLING_96KHZ      = 4,
  VD_SAMPLING_192KHZ      = 5,
  VD_SAMPLING_48_192KHZ  = 12,
  VD_SAMPLING_48_96KHZ    = 14,
};

enum VD_META_KEY_TYPE
{
  VD_META_KEY_UNKNOWN = 0,

  VD_META_KEY_FILEFORMAT,       // { dvd, bluray, mkv, avi, mp4, m2ts, ts .... }
  VD_META_KEY_VIDEOCODEC,       // { h264, avc1, vc1, mpeg2, mpeg4 ....}
  VD_META_KEY_VIDEORES,         // { 1080p, 720p, 480p ...}
  VD_META_KEY_VIDEOSTANDARD,    // { ntsc, pal }

  VD_META_KEY_AUDIOCODEC,       // { dts, ac3, lpcm, dtshd, truehd, eac3, flac, mp3, aac ... }
  VD_META_KEY_AUDIOCHANNEL,     // { 2.0, 5.1, 7.1 .. }
  VD_META_KEY_AUDIOLANGUAGE,

  VD_META_KEY_SUBCODEC,         // { pgs, srt }
  VD_META_KEY_SUBLANGUAGE,

  VD_META_KEY_MENUMODE,        // { hdmv, bdj }
  VD_META_KEY_3DTYPE,          // { mvc, sbs, tb ... }
};

enum VD_PLAY_SOURCE_TYPE
{
  VD_PLAY_SOURCE_MEDIA_UNKNOWN = -1,

  //video and audio container
  VD_PLAY_SOURCE_MEDIA_AVI,    
  VD_PLAY_SOURCE_MEDIA_MKV,  
  VD_PLAY_SOURCE_MEDIA_RMVB,
  VD_PLAY_SOURCE_MEDIA_MP4,
  VD_PLAY_SOURCE_MEDIA_MOV,
  VD_PLAY_SOURCE_MEDIA_WMV,
  VD_PLAY_SOURCE_MEDIA_ASF,
  VD_PLAY_SOURCE_MEDIA_MPEG_PS,
  VD_PLAY_SOURCE_MEDIA_MPEG_TS,
  VD_PLAY_SOURCE_MEDIA_FLV,
  VD_PLAY_SOURCE_MEDIA_3GP,
  VD_PLAY_SOURCE_MEDIA_3GP2,
  VD_PLAY_SOURCE_MEDIA_DPG,

  //audio only container
  VD_PLAY_SOURCE_MEDIA_WAV,
  VD_PLAY_SOURCE_MEDIA_MP3,
  VD_PLAY_SOURCE_MEDIA_WMA,
  VD_PLAY_SOURCE_MEDIA_AAC,
  VD_PLAY_SOURCE_MEDIA_FLAC,
  VD_PLAY_SOURCE_MEDIA_APE,

  //special disc format
  VD_PLAY_SOURCE_MEDIA_DVD,
  VD_PLAY_SOURCE_MEDIA_BDMV,
  VD_PLAY_SOURCE_MEDIA_AVCHD,
  VD_PLAY_SOURCE_MEDIA_BDAV,
  VD_PLAY_SOURCE_MEDIA_SSIF,
  VD_PLAY_SOURCE_MEDIA_VMF,

  //mku format
  VD_PLAY_SOURCE_MEDIA_MKUBD,
  VD_PLAY_SOURCE_MEDIA_MKUDVD,
};

//==========================================================================================================================//
// B.stream, playlist class


class IVDMediaStream
{
public:

  IVDMediaStream(){};
  virtual ~IVDMediaStream(){};

  //stream common attribute

/**
  * get stream type
  * @return enum VD_STREAM_TYPE
  */

  virtual VD_STREAM_TYPE GetStreamType() = 0;

/**
  * get stream language
  * @return standard language string (undefined or English)
  */

  virtual const char* GetLanguage() = 0;        

/**
  * get codec name 
  * @return string (h264, pgs or others), TODO: list more codec example
  */

  virtual const char* GetCodecName() = 0;

/**
  * get AVCodecID
  * @return id in ffmpeg AVCodecID, if not exist return AV_CODEC_ID_NONE = 0
  */

  virtual int  GetAVCodecID() = 0;

/**
  * get stream id, not index
  * @return id in container, in m2ts/vob is pid
  */

  virtual int  GetStreamID() = 0;

/**
  * get average bitrate
  * @return bitrate in bps
  */

  virtual int GetAverageBitrate() = 0;

  //video stream attribute

/**
  * get video width, height, size_res
  * @return width or height
  * @return value in VD_VIDEO_RES
  */

  virtual int GetWidth(){ return 0; }
  virtual int GetHeight(){ return 0; }
  virtual VD_VIDEO_RES GetVideoResFormat(){ return VD_VIDEO_RES_UNKNOWN; }

/**
  * get video aspect ratio
  * @return value in VD_VIDEO_AP
  */

  virtual VD_VIDEO_AP GetVideoARFormat(){ return VD_ASPECT_UNKNOWN; }

/**
  * get video frame_rate
  * @return value in VIDEO_FPS
  * @return fps, fpsscale value
  */

  virtual VD_VIDEO_FPS GetVideoFpsFormat(){ return VD_FPS_UNKNOWN; }
  virtual int GetVideoFps(){ return 1; }
  virtual int GetVideoFpsScale(){ return 1; }

/**
  * get some special video attribute
  * @return value in bool
  */

  virtual bool IsInterlacedFrame(){ return false; }
  virtual bool Is10bitsCodec(){ return false; }


  //audio stream attribute

/**
  * get audio channel
  * @return real channel
  */

  virtual int GetAudioChannels(){ return 0; }

/**
  * get samplerate 
  * @return samplerate value and value in AUDIO_SAMPLING
  */

  virtual VD_AUDIO_SAMPLING GetAudioSampleRateFormat(){ return VD_SAMPLING_UNKNOWN; }
  virtual int GetAudioSampleRate() { return 0; }

  //subtitle stream attribute


  //TODO: more stream information

};

class IVDMediaInfo
{
public:

  IVDMediaInfo(){};
  virtual~IVDMediaInfo(){};

  //public attribute
  virtual int GetCodecType() = 0;
  virtual int GetCodecId() = 0;
  virtual unsigned int GetCodecTag() = 0;
  virtual unsigned int GetIndex() = 0;
  virtual unsigned int GetStreamId() = 0;
  virtual unsigned int GetBitrate() = 0;
  virtual unsigned int GetCrfValue() = 0;
  virtual unsigned int GetBitsPerSample() = 0;
  virtual int GetProfile() = 0;
  virtual int GetLevel() = 0;
  virtual unsigned int GetTimeNum() = 0;
  virtual unsigned int GetTimeDen() = 0;

  //audio attribute
  virtual int GetSampleFmt() = 0;
  virtual unsigned int GetChannels() = 0;
  virtual long long GetChannelMask() = 0;
  virtual unsigned int GetSampleRate() = 0;
  virtual unsigned int GetBlockAlign() = 0;
  virtual unsigned int GetFrameSize() = 0;
  virtual unsigned int GetSamplesPerFrame() = 0;
  virtual unsigned long long* GetChannelOrderMap() = 0;

  //video attribute
  virtual int GetPixFmt() = 0;
  virtual unsigned int GetWidth() = 0;
  virtual unsigned int GetHeignt() = 0;
  virtual unsigned int GetFps() = 0;
  virtual unsigned int GetFpsBase() = 0;
  virtual unsigned int GetSampleAspectRation() = 0;
  virtual unsigned int GetSampleAspectRtionBase() = 0;
  virtual int GetMovRotation() = 0; //mov file rotate angle metadata 0 90 180 270, default 0
  virtual int GetFramePackingArrangementType() = 0;
  virtual int GetElemStreamType() = 0;
  virtual bool GetSupportAnnexb() = 0;

  //attributes                        
  virtual int GetStreamIndex() = 0;   //index in the same streams. only for valid stream.
  virtual long long GetSteamDuration() = 0;
                              
  //decode extradata
  virtual bool GetBExtradata() = 0;
  virtual int GetExtradataSize() = 0;
  virtual unsigned char* GetExtadata() = 0;

  //display information
  virtual const char* GetDisplayExtend() = 0;
  virtual const char* GetLanguageCode() = 0;
  virtual const char* GetLanguageName() = 0;
  virtual unsigned short GetLangCodeId() = 0;
  virtual bool GetExtaAdd() = 0;

  virtual int GetTvStandard() { return -1; }
  virtual bool GetIsIVTC() { return false; }

  //interlace judge
  virtual bool GetBInterlace() { return false; }
  virtual int GetPaletteSize() { return 0; }
  virtual unsigned char* GetPalette() { return NULL; }
  virtual int GetSubMask() { return 0; }

};

class IVDMediaPlaylist
{
public:

  IVDMediaPlaylist(){}
  virtual ~IVDMediaPlaylist(){}

  //streams information 

/**
  * get stream count
  * @return stream count in this playlist
  */

  virtual int GetAllStreamCount() = 0;

  /**
  * get DurationMs duration in millsecond
  * @return 
  */

  virtual int GetDurationMs() = 0;

  /**
  * get the playlist value
  * @return the playlist value
  */

  virtual int GetPlaylistValue() = 0;

  /**
  * get index in all playlist
  * @return the index
  */

  virtual int GetIndex() = 0;

  /**
  * get chapter count 
  * @return the chapter count
  */

  virtual int GetChapterCount() = 0;
  /**
  * get angle count of this playlist
  * @return angle
  */

  virtual int GetAngles() = 0;

  /**
  * get the total size this playlist
  * @return total size
  */
  virtual int64_t GetTotalSize() = 0;

  /**
  * get the average bitrate for this playlist
  * @return average bitrate
  */

  virtual int GetAverageBitrate() = 0;

  /**
  * the playlist is 3D
  * @return bool
  */
  virtual bool Is3D() = 0;

  /**
  * the playlist is MainMovie
  * @return bool
  */

  virtual bool IsMainMovie() = 0;

  /**
  * the playlist is Menu
  * @return bool
  */

  virtual bool IsMenu() = 0;

  /**
  * get count of the selected stream type
  * @param: iStreamType, the type of the stream
  * @return int 
  */

  virtual int GetStreamCount(VD_STREAM_TYPE iStreamType) = 0;

  /**
  * get the selecte stream
  * @param: iStreamType, the type of the stream
  * @param: iIndex,the iIndex of the stream
  * @return IMediaStream
  */

  virtual IVDMediaStream* GetMediaStream(VD_STREAM_TYPE iStreamType, int iIndex) = 0;

  virtual int GetAllMediaInfoCount() = 0;
  virtual int GetMediaInfoCount(VD_STREAM_TYPE iStreamType) = 0;
  virtual IVDMediaInfo* GetMediaInfo(VD_STREAM_TYPE iStreamType, int iIndex) = 0;
};
  
}

#endif // __IVD_MEDIAINFO_H__
