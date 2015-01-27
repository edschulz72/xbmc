#ifndef __LIBVIDONUTILS_VIDONPLAYERMANAGER_H__
#define __LIBVIDONUTILS_VIDONPLAYERMANAGER_H__

#include <vector>
enum
{
  VIDONPLAYER_DEVICE_UNKNOWN = -1,
  VIDONPLAYER_DEVICE_A31,
  VIDONPLAYER_DEVICE_AMLOGIC,
};

class ANativeActivity;
class ANativeWindow;
typedef struct stPlayercoreConfig PlayercoreConfig;
//vidonplayer config
typedef struct stVidonPlayerConfig
{
  stVidonPlayerConfig()
  {
    nDeviceType = VIDONPLAYER_DEVICE_UNKNOWN;
    pNativeActivity = NULL;
    pNativeWindows = NULL;
    pRenderManager = NULL;
    pMatrices = NULL;
    pPlayercoreconfig = NULL;
  }

  int               nDeviceType;
  ANativeActivity*  pNativeActivity;
  ANativeWindow*    pNativeWindows;
  void*             pRenderManager;
  void*             pMatrices;
  PlayercoreConfig* pPlayercoreconfig;
} VidonPlayerConfig;

class VidonPlayer;
struct ICorePlayerCallback;
struct AudioSetting;
struct AppSettings;
namespace XSource
{
  class CPlaylist;
  typedef std::vector<CPlaylist*> PlaylistPtrs;
}
class IGetSourceSettings;
class ISourceDetails;
class CCorePlayer;

class CVidonPlayerManager
{
public:
   CVidonPlayerManager();
   virtual ~CVidonPlayerManager();

   virtual bool Vidonplayer_Init( VidonPlayerConfig *pConfig );
   virtual void Vidonplayer_DeInit();
   virtual CCorePlayer* Vidonplayer_GetCorePlayer( ICorePlayerCallback *pCallback );
   virtual void Vidonplayer_ReleaseCorePlayer( CCorePlayer*& pPlayer );
   virtual void Vidonplayer_GetVersion( int &major,int &minor );
   virtual const char* Vidonplayer_GetVersionInfo();
   virtual void Vidonplayer_RefreshHardwareAccelSetting(int nmode);
   virtual bool Vidonplayer_SetAudioOutputDevice( int deviceIndex );
   virtual bool Vidonplayer_SetAudioDefaultOutputDevice( const char* pszDeviceName, int types);
   virtual bool Vidonplayer_SetAudioOutputUpmixStereo( bool bUpmix );
   virtual bool Vidonplayer_SetAudioOutputSpeakers( int speaker );
   virtual bool Vidonplayer_SetAudioPassthroughCodec( int supportCodec );
   virtual AudioSetting* Vidonplayer_GetAudioSettings();
#if defined(TARGET_VIDONME_BOX)
   virtual bool Vidonplayer_SetAudioHdmiPassthroughType(int mode);
#endif
   virtual bool Vidonplayer_SetAppSettings(AppSettings* pSettings);
   virtual int Vidonplayer_AddPlaySource( const char* pszFile, int type ); 
   virtual const char* Vidonplayer_GetStandardPath(const char* pszFile);
   virtual void Vidonplayer_RemovePlaySource( const char* pszFile, int type );
   virtual void Vidonplayer_RemovePlaySourceAll();
   virtual XSource::PlaylistPtrs& Vidonplayer_GetPlaySourcePlaylist( const char* pszFile );
   virtual const char* Vidonplayer_GetPlaySourceImage( const char* pszFile );
   virtual int Vidonplayer_GetPlaySourcePlaylistCount( const char* pszFile );
   virtual XSource::CPlaylist* Vidonplayer_GetPlaySourcePlaylistByIndex( const char* pszFile, int index );
   virtual XSource::CPlaylist* Vidonplayer_GetPlaySourceMainmoviePlaylist( const char* pszFile );
   virtual const char* Vidonplayer_GetPlaySourceMeta( const char* pszFile, const char* pszKey );
   virtual bool Vidonplayer_GetPlaySourceThumb(const char* src, const char* strTarget, int width, int height, int time, int playlist = -1);
   virtual bool Vidonplayer_RefreshFontSetting(PlayercoreConfig* pConfig);
   virtual void Vidonplayer_SetSettingBool(int key, bool bValue);
   virtual void Vidonplayer_SetSettingString(int key, const char* pszValue);
   virtual void Vidonplayer_SetSettingInt(int key, int iValue);
   virtual void Vidonplayer_SetSettingFloat(int key, float fValue);

   virtual void Vidonplayer_SetBDRegion(int BDRegion);
   virtual bool Vidonplayer_RegisterUserInformation(PlayercoreConfig* pConfig);
   virtual ISourceDetails* Vidonplayer_GetPlaySourceDetails( IGetSourceSettings* pSettings );
   virtual ISourceDetails* Vidonplayer_GetImgPlaylistsThumbnail( IGetSourceSettings* pSettings );
};

#endif //