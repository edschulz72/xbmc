#ifndef __IVD_MEDIATOOL_H__
#define __IVD_MEDIATOOL_H__

#include "IVD_mediainfo.h"
#include <vector>
#include <map>
//==========================================================================================================================//
// IVDPlayer mediatool

namespace libvp
{
#define MAINMOVIE_PLAYLIST 0xFFFFF

//==========================================================================================================================//
// A. class IVDPlayer mediatool config

class IVDMediatoolConfig
{
public:

  IVDMediatoolConfig() {}
  virtual ~IVDMediatoolConfig() {}

/**
  * get source path 
  * @return source path string,
  */

  virtual const char* GetSourcePath() = 0;

/**
  * get source type
  * @return IPLCORE_SOURCE_TYPE_XXXX enum value
  */

  virtual int GetSourceType() = 0;

/**
  * check need cancel parser or other working process
  * @return true if client need mediatool to cancel current working
  */

  virtual bool IsCancel() = 0;

/**
  * get thumbnail saved path
  * @param: plcore tell the client, which playlist is be decode, if common file, playlist = -1
  * @return thumbnail path string
  */

  virtual const char* GetThumbnailPath(int playlist = -1) = 0;

  /**
  * get the movie poster saved path
  * @return movie poster path string
  */

  virtual const char* GetMovieImagePath() = 0;

/**
  * get Muti thumbnail saved path
  * @param: nSeekTo,the time to get image
  * @param: nIntervalTime,the interval time between two images,the nSeekTo is second
  * @param: plcore tell the client, which playlist is be decode, if common file, playlist = -1,the nIntervalTime is second
  * @return thumbnail path string
  */

  virtual const char* GetMultiThumbnailPath(int nSeekTo, int& nIntervalTime, int playlist = -1) = 0;

/**
  * get percent time of thumbnail
  * @return percent of thumbnail time in (0-100)%
  */

  virtual float  GetThumbnailPercentTime() = 0;

/**
  * get size of thumbnail
  * @param: width, height
  * @return void
  */

  virtual void GetThumbnailSize(int& iWidth, int& iHeight ) = 0;

  /**
  * get the Min time length to praser the playlist
  * @return int
  */
  virtual int GetMinTimeLengthOfPlaylist() = 0;

#ifdef TARGET_WINDOWS
  virtual bool IsNeedSaveBMP(){ return false; }

  virtual void SetThumbnailBMP(uint32_t pBMP) {}

#endif


};

//==========================================================================================================================//
// B. class IVDPlayer media input stream

class IVDMediaInputStream
{
public:

  IVDMediaInputStream(){};
  virtual ~IVDMediaInputStream(){};

  //IDisplayTime interface

/**
  * GetTotalTime() if stream is supported
  * @return totaltime in millsecond, if invalid return -1
  */

  virtual int64_t GetTotalTime() = 0;

/**
  * GetTime() if stream is supported
  * @return current playing time, if invalid return -1
  */
  virtual int64_t GetTime() = 0;

  //ISeekTime interface

/**
  * SeekTime() if stream is supported
  * @return true if success
  */

  virtual bool SeekTime(int millsecond) = 0;

  //IChapter interface

/**
  * chapter function
  * @param ch = 1 ---> ChapterCount
  * @return, return 0 if no chapter
  */

  virtual int  GetChapter() = 0;
  virtual int  GetChapterCount() = 0;
  virtual const char* GetChapterName(int ch) = 0;
  virtual bool SeekChapter(int ch) = 0;

  //InputStream interface

/** Open(), the path should be set in construct
  * @param content, example = "application/vnd.apple.mpegurl"
  */

  virtual bool Open(const char* pszContent) = 0;
  virtual void Close() = 0;
  virtual int Read(unsigned char* buf, int buf_size) = 0;

/**
  * seek function
  * @param: 
  * standard seek flag : SEEK_CUR, SEEK_END, SEEK_SET
  * custom seek flag: SEEK_POSSIBLE, using for check seek function is can be used
  * for some live stream, seek is impossible, if can seek return 1 else return 0
  * @return standard result
  */

  virtual int64_t Seek(int64_t offset, int whence) = 0;
  
/**
  * Pause in specify time
  * @param pause time in millsecond
  * @return, standard result
  */

  virtual bool Pause(double dTime) = 0;
  virtual bool IsEOF() = 0;
  virtual int64_t GetLength() = 0;


/**
  * read block / align size
  * @return, if not need align, default value = 1
  */

  virtual int GetBlockSize() = 0;

  //TODO, need explore more attribute(from DVDInputStream)

};

//==========================================================================================================================//
// C. class IVDPlayer mediatool

class IVDMediatool
{
public:

  IVDMediatool(IVDMediatoolConfig* pConfig) {}
  virtual ~IVDMediatool() {}

/**
  * parser media
  * @param IVDMediatoolConfig, all user param will be send by config
  * @return bool,
  */

  virtual bool Parser() = 0;

/**
  * get specify source thumbnail
  * @param: playlist, for file it's not useful
  *          for DVD/Bluray, if playlist = -1, then try to get the mainmovie thumbnail
  * @return bool if get thumbnail successful
  */

  virtual bool GetThumbnail(int playlist = MAINMOVIE_PLAYLIST) = 0;

/**
  * get Multi thumbnail
  * @param: playlist, for file it's not useful
  *          for DVD/Bluray, if playlist = MAINMOVIE_PLAYLIST, then try to get the mainmovie thumbnail
  * @return bool if get Multi thumbnail successful
  */

  virtual bool GetMultiThumnail(int playlist = MAINMOVIE_PLAYLIST) = 0;



  /**
  * get playlist count
  * @return the playlist count
  */

  virtual int GetPlaylistCount() = 0;


  /**
  * get playlist by index
  * @return the IVDMediaPlaylist*
  */

  virtual IVDMediaPlaylist* GetMediaPlaylistByIndex(int nIndex) = 0;

  /**
  * get IVDMediaPlaylist
  * @param: playlist, for file it's not useful
  *          for DVD/Bluray, if playlist = MAINMOVIE_PLAYLIST, then try to get the mainmovie thumbnail
  * @return IVDMediaPlaylist
  */
  virtual IVDMediaPlaylist* GetMediaPlaylistByPlaylist(int playlist = MAINMOVIE_PLAYLIST) = 0;


  /**
  * get the meta value int the metamap
  * @param: iType, the type key in the meamap
  * @return const char*
  */

  virtual const char* GetMetaValue(VD_META_KEY_TYPE iType) = 0;


/**
  * create IVDMediaInputStream object
  * release IVDMediaInputStream
  * mediatool provide input(read, source) stream to outer, client can use this stream as input stream, and provide
  * to demux, mediatool can wrap file(http,smb,dvd,bluray ...)
  * @param: playlist, for file it's not useful
  *          for DVD/Bluray, if playlist = MAINMOVIE_PLAYLIST,
  * 
  */
  
  virtual IVDMediaInputStream* CreateMediaInputStream(int playlist = MAINMOVIE_PLAYLIST) = 0;
  virtual void ReleaseMediaInputStream(IVDMediaInputStream* pStream) = 0;

  /**
  * get the IVDMediaInfo
  * @param: playlist, the playlist to parser
  * @return bool
  */

  virtual bool ParserMediaInfo(int playlist) = 0;

  /**
  * get the VD_PLAY_SOURCE_TYPE
  * @return VD_PLAY_SOURCE_TYPE
  */

  virtual VD_PLAY_SOURCE_TYPE GetPlaySourceType() = 0;

  /**
  * get the bd movie name ,didn't need parser
  * @return the bd movie name
  */

  virtual const char* GetBDMovieName() = 0;
    
    /**
     * get the main playlist during
     * @return the during in ms
     */
    
  virtual int GetMainPlaylistDuring() = 0;

};

}

#endif //__IVD_MEDIATOOL_H__