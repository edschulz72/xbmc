#include "system.h"
#include <string.h>
#include <stdint.h>
#include <string>
#include <stdlib.h>
#include "sys/system_properties.h"
#include "VidonPlayer_jni.h"
#include "VidonPlayer_cfg.h"
#include "jutils/jutils.hpp"

#include "ICallbackforChannelInfo.h"
#include "VMSPlayer.h"
#include "VD_coreplayer_plcore.h"
#include "VD_coreplayer_sys.h"

#include <android/native_window.h>
#include <android/native_window_jni.h>

#ifdef VDPLAYER_FOR_KODI
  #define NFUNC(func) Java_org_xbmc_kodi_VidonPlayer_##func
#else
  #define NFUNC(func) Java_org_vidonme_player_VidonPlayer_##func
#endif

using namespace libvp;
class CVPlayerCore;
extern "C"
{
  jstring NFUNC(getSocName)(JNIEnv* env,jobject thiz);
  void NFUNC(ntSetLogFolder)(JNIEnv* env,jobject thiz,jstring path);
  void NFUNC(ntSetBDCacheFolder)(JNIEnv* env,jobject thiz,jstring path);
  void NFUNC(ntLogOutPut)(JNIEnv* env,jobject thiz,jstring log);
  void NFUNC(ntSetLogEnable)(JNIEnv* env,jobject thiz,jboolean bEnable,jint level);

  void NFUNC(ntInit)(JNIEnv* env,jobject thiz);
  void NFUNC(ntInitPlayerCore)(JNIEnv* env,jobject thiz, jobject owner);
  void NFUNC(ntUnInitPlayerCore)(JNIEnv* env,jobject thiz);
  void NFUNC(ntInitPlayTool)(JNIEnv* env,jobject thiz);
  void NFUNC(ntUnInitPlayTool)(JNIEnv* env,jobject thiz);

  void NFUNC(ntSetup)(JNIEnv* env,jobject thiz);
  void NFUNC(ntConfig)(JNIEnv* env,jobject thiz,jobject owner);
  void NFUNC(ntPrepare)(JNIEnv*env, jobject thiz);
  void NFUNC(ntPrepareAsync)(JNIEnv*env, jobject thiz);
  jboolean NFUNC(ntStart)(JNIEnv* env,jobject thiz);
  void NFUNC(ntStop)(JNIEnv* env,jobject thiz,jboolean bwait);
  void NFUNC(ntPause)(JNIEnv* env,jobject thiz);
  void NFUNC(ntResume)(JNIEnv* env,jobject thiz);
  void NFUNC(ntRelease)(JNIEnv* env,jobject thiz);
  void NFUNC(ntReset)(JNIEnv* env,jobject thiz);
 
  void NFUNC(ntSetVideoSurface)(JNIEnv* env,jobject thiz,jobject jsurface);
  void NFUNC(ntUnSetVideoSurface)(JNIEnv* env,jobject thiz);

  void NFUNC(ntSetDataSource)(JNIEnv* env,jobject thiz,jstring path);
  void NFUNC(ntSetRestoreStream)(JNIEnv* env,jobject thiz,jint asIndex,jint ssIndex,jstring spath);
  void NFUNC(ntSetBdPlayMode)(JNIEnv* env,jobject thiz,jint mode,jint playlist);
 
  jint NFUNC(ntGetFileDuration)(JNIEnv* env,jobject thiz,jstring src);
  jboolean NFUNC(ntIsPlaying)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntIsPaused)(JNIEnv* env,jobject thiz);
  jboolean NFUNC(ntIsCaching)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetCacheLevel)(JNIEnv* env,jobject thiz);

  void NFUNC(ntSetPlaySpeed)(JNIEnv* env,jobject thiz,jfloat iSpeed);
  jfloat NFUNC(ntGetPlaySpeed)(JNIEnv* env,jobject thiz);
  void NFUNC(ntOnAction)(JNIEnv* env,jobject thiz, jint id, jfloat x, jfloat y);

  jint NFUNC(ntGetVideoWidth)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetVideoHeight)(JNIEnv* env,jobject thiz);
  void NFUNC(ntSetViewMode)(JNIEnv* env,jobject thiz,jint viewmode);
  void NFUNC(ntNotifyViewModeChanged)(JNIEnv* env,jobject thiz);

  jint NFUNC(ntGetCurrentPosition)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetDuration)(JNIEnv* env,jobject thiz);
  void NFUNC(ntSeekTo)(JNIEnv* env,jobject thiz, jint msec);

  jint NFUNC(ntGetAudioStreamCount)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetAudioStreamIndex)(JNIEnv* env,jobject thiz);
  jstring NFUNC(ntGetAudioStreamInfoByIndex)(JNIEnv* env,jobject thiz, jint index);
  void NFUNC(ntSetAudioStreamIndex)(JNIEnv* env,jobject thiz,jint index);

  jint NFUNC(ntGetSubtitleCount)(JNIEnv* env,jobject thiz,jboolean bupdate);
  jint NFUNC(ntAddSubtitle)(JNIEnv* env,jobject thiz, jstring subpath);
  void NFUNC(ntSetSubtitleIndex)(JNIEnv* env,jobject thiz, jint index);
  jint NFUNC(ntGetSubtitleStreamIndex)(JNIEnv* env,jobject thiz);
  jstring NFUNC(ntGetSubtitleStreamInfoByIndex)(JNIEnv* env,jobject thiz, jint index);
  jfloat NFUNC(ntGetSubtitleDelay)(JNIEnv* env,jobject thiz);
  void NFUNC(ntSetSubtitleDelay)(JNIEnv* env,jobject thiz, jfloat fValue);
  jboolean NFUNC(ntGetSubtitleVisible)(JNIEnv* env,jobject thiz);
  void NFUNC(ntSetSubtitleVisible)(JNIEnv* env,jobject thiz, jboolean bVisible);

  void NFUNC(ntSetSubtitleColor)(JNIEnv* env,jobject thiz, jint nColor);
  void NFUNC(ntSetSubtitleStyle)(JNIEnv* env,jobject thiz, jint nStyle);
  void NFUNC(ntSetSubtitleSize)(JNIEnv* env,jobject thiz, jint nSize);
  void NFUNC(ntSetSubtitlePos)(JNIEnv* env,jobject thiz, jint nAlign, jfloat yPos);

  jint NFUNC(ntGetSubtitleColor)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetSubtitleStyle)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetSubtitleSize)(JNIEnv* env,jobject thiz);
  jfloat NFUNC(ntGetSubtitlePos)(JNIEnv* env,jobject thiz);
  void NFUNC(ntSetDefaultSubtitleProperty)(JNIEnv* env,jobject thiz,jint nSubColor,jint  nSubSize,jfloat fSubPos,jint nStyle,jfloat fdelay);

  void NFUNC(ntSetWhetherOverAssOrigSettings)(JNIEnv* env,jobject thiz, jboolean bOver);

  jint NFUNC(ntGetChapterCount)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetChapterIndex)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntSeekChapter)(JNIEnv* env,jobject thiz, jint index);

  jboolean NFUNC(ntIsDisplay3DSupported)(JNIEnv* env,jobject thiz);
  void NFUNC(ntSetDisplayMode)(JNIEnv* env,jobject thiz, jint mode);

  jboolean NFUNC(ntIsInMenu)(JNIEnv* env,jobject thiz);
  
  jint NFUNC(ntGetBitrate)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetStarttime)(JNIEnv* env,jobject thiz);
  jstring NFUNC(ntGetOutType)(JNIEnv* env,jobject thiz);
  jstring NFUNC(ntGetResolution)(JNIEnv* env,jobject thiz);
  jint NFUNC(ntGetOutQuality)(JNIEnv* env,jobject thiz);

    //0:software, 1:hardware
  jboolean NFUNC(ntSetDecodingMethod)(JNIEnv* env,jobject thiz,jint method);
  jboolean NFUNC(ntExtractThumbForFile)(JNIEnv* env, jobject thiz, jstring src, jstring dst, jint width, jint height, jint percentTime);
  jboolean NFUNC(ntExtractThumbForPlay)(JNIEnv* env, jobject thiz, jstring dst, jint width);
  void NFUNC(ntExtractMeidaInfo)(JNIEnv* env, jobject thiz, jstring file, jobject info);

  jlong NFUNC(ntReadFileData)(JNIEnv* env, jobject thiz, jstring src, jstring dst,jint startpos, jint datalen);

}
/*
 * internal function
 */

#if defined(USE_BREADPAD)
static google_breakpad::ExceptionHandler *eh=NULL;
static bool DumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
  void* context,
  bool succeeded) {
    JNIEnv* env;
    JNIAttachCurrentThread(env);
    time_t t = time(NULL);
    char tmpc[64] = {0,};  
    strftime(tmpc, sizeof(tmpc), "%Y%m%d%H%M%S.dmp", localtime(&t));

    std::string newpath = descriptor.path();
    int end = newpath.find_last_of('/');
    newpath = StringUtils::Left(newpath, end+1) + tmpc;
    LOGE("Dump file path :%s", newpath.c_str());

    MoveFile(descriptor.path(),newpath.c_str());
    env->CallStaticVoidMethod(AndroidRuntime::Get().m_clsVidonplayer, AndroidRuntime::Get().m_midNativecrash, env->NewStringUTF(newpath.c_str()));
    JNIDetachCurrentThread();
    return succeeded;
}
#else
static struct sigaction oldact;
void p_sigaction(int signum, siginfo_t *info, void *reserved)
{
  JNIEnv* env;
  JNIAttachCurrentThread(env);
  env->CallStaticVoidMethod(AndroidRuntime::Get().m_clsVidonplayer, AndroidRuntime::Get().m_midNativecrash, NULL);
  JNIDetachCurrentThread();
  oldact.sa_sigaction(signum,info,reserved);
}

void redefineSigaction()
{
  struct sigaction handler;
  memset(&handler, 0, sizeof(struct sigaction));
  handler.sa_sigaction = p_sigaction;
  handler.sa_flags = SA_RESETHAND;

#define CATCHSIG(X) sigaction(X, &handler, &oldact)
  CATCHSIG(SIGILL);
  CATCHSIG(SIGABRT);
  CATCHSIG(SIGBUS);
  CATCHSIG(SIGFPE);
  CATCHSIG(SIGSEGV);
  CATCHSIG(SIGSTKFLT);
  CATCHSIG(SIGPIPE);
}
#endif


static void setup_env(JNIEnv* env, jobject owner)
{
  const char *temp ;
  AndroidRuntime::Get().m_midNativecrash = env->GetStaticMethodID(AndroidRuntime::Get().m_clsVidonplayer,"onNativeCrash","(Ljava/lang/String;)V");

#if !defined(USE_BREADPAD)
  redefineSigaction();
#endif

  jobject oActivity = owner;
  jclass cActivity = env->GetObjectClass(oActivity);

//#if defined(__TARGET_ANDROID_ALLWINNER__)
  jmethodID midActivityGetSystemService = env->GetMethodID(cActivity, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
  jstring sDisplayManagerService = env->NewStringUTF("display"); // Display_Manager
  jobject oDisplayManager = env->CallObjectMethod(oActivity, midActivityGetSystemService, sDisplayManagerService);
  AndroidRuntime::Get().m_objDispManager = env->NewGlobalRef(oDisplayManager);
  env->DeleteLocalRef(oDisplayManager);
//#endif

  // get the path to the android system libraries
  jclass cSystem = env->FindClass("java/lang/System");
  jmethodID midSystemGetProperty = env->GetStaticMethodID(cSystem, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
  jstring sJavaLibraryPath = env->NewStringUTF("java.library.path");
  jstring sSystemLibraryPath = (jstring)env->CallStaticObjectMethod(cSystem, midSystemGetProperty, sJavaLibraryPath);
  temp = env->GetStringUTFChars(sSystemLibraryPath, NULL);
  setenv("XBMC_ANDROID_SYSTEM_LIBS", temp, 0);
  env->ReleaseStringUTFChars(sSystemLibraryPath, temp);
  env->DeleteLocalRef(sJavaLibraryPath);
  env->DeleteLocalRef(sSystemLibraryPath);
  env->DeleteLocalRef(cSystem);
  LOGE("XBMC_ANDROID_SYSTEM_LIBS :%s", temp);

  // get the path to XBMC's data directory (usually /data/data/<app-name>)
  jmethodID midActivityGetApplicationInfo = env->GetMethodID(cActivity, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
  jobject oApplicationInfo = env->CallObjectMethod(oActivity, midActivityGetApplicationInfo);
  jclass cApplicationInfo = env->GetObjectClass(oApplicationInfo);
  jfieldID fidApplicationInfoDataDir = env->GetFieldID(cApplicationInfo, "dataDir", "Ljava/lang/String;");
  jstring sDataDir = (jstring)env->GetObjectField(oApplicationInfo, fidApplicationInfoDataDir);
  temp = env->GetStringUTFChars(sDataDir, NULL);
  setenv("XBMC_ANDROID_DATA", temp, 0);
  env->ReleaseStringUTFChars(sDataDir, temp);
  env->DeleteLocalRef(sDataDir);

  LOGE("XBMC_ANDROID_DATA :%s", temp);
  // get the path to where android extracts native libraries to
  jfieldID fidApplicationInfoNativeLibraryDir = env->GetFieldID(cApplicationInfo, "nativeLibraryDir", "Ljava/lang/String;");
  jstring sNativeLibraryDir = (jstring)env->GetObjectField(oApplicationInfo, fidApplicationInfoNativeLibraryDir);
  temp = env->GetStringUTFChars(sNativeLibraryDir, NULL);
  setenv("XBMC_ANDROID_LIBS", temp, 0);
  env->ReleaseStringUTFChars(sNativeLibraryDir, temp);
  env->DeleteLocalRef(sNativeLibraryDir);
  env->DeleteLocalRef(oApplicationInfo);
  env->DeleteLocalRef(cApplicationInfo);

  LOGE("XBMC_ANDROID_LIBS :%s", temp);
  // get the path to the APK
  char apkPath[PATH_MAX] = {0};
  jmethodID midActivityGetPackageResourcePath = env->GetMethodID(cActivity, "getPackageResourcePath", "()Ljava/lang/String;");
  jstring sApkPath = (jstring)env->CallObjectMethod(oActivity, midActivityGetPackageResourcePath);
  temp = env->GetStringUTFChars(sApkPath, NULL);
  strcpy(apkPath, temp);
  setenv("XBMC_ANDROID_APK", apkPath, 0);
  env->ReleaseStringUTFChars(sApkPath, temp);
  env->DeleteLocalRef(sApkPath);

  LOGE("XBMC_ANDROID_APK :%s",apkPath );
  // Get the path to the temp/cache directory
  char cacheDir[PATH_MAX] = {0};
  char tempDir[PATH_MAX] = {0};
  jmethodID midActivityGetCacheDir = env->GetMethodID(cActivity, "getCacheDir", "()Ljava/io/File;");
  jobject oCacheDir = env->CallObjectMethod(oActivity, midActivityGetCacheDir);

  jclass cFile = env->GetObjectClass(oCacheDir);
  jmethodID midFileGetAbsolutePath = env->GetMethodID(cFile, "getAbsolutePath", "()Ljava/lang/String;");
  env->DeleteLocalRef(cFile);

  jstring sCachePath = (jstring)env->CallObjectMethod(oCacheDir, midFileGetAbsolutePath);
  temp = env->GetStringUTFChars(sCachePath, NULL);
  strcpy(cacheDir, temp);
  strcpy(tempDir, temp);
  env->ReleaseStringUTFChars(sCachePath, temp);
  env->DeleteLocalRef(sCachePath);
  env->DeleteLocalRef(oCacheDir);

  //strcat(tempDir, "/temp");
  //setenv("XBMC_TEMP", tempDir, 0);

  //LOGE("XBMC_TEMP :%s", tempDir );
  strcat(cacheDir, "/apk");

  //cache assets from the apk
  //extract_to_cache(apkPath, cacheDir);

  strcat(cacheDir, "/assets");
  setenv("XBMC_BIN_HOME", cacheDir, 0);
  setenv("XBMC_HOME"    , cacheDir, 0);

  LOGE("XBMC_BIN_HOME :%s", cacheDir );
  LOGE("XBMC_HOME :%s", cacheDir );


  // Get the path to the external storage
  // The path would actually be available from activity->externalDataPath (apart from a (known) bug in Android 2.3.x)
  // but calling getExternalFilesDir() will automatically create the necessary directories for us.
  // There might NOT be external storage present so check it before trying to setup.
  char storagePath[PATH_MAX] = {0};
  jmethodID midActivityGetExternalFilesDir = env->GetMethodID(cActivity, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");
  jobject oExternalDir = env->CallObjectMethod(oActivity, midActivityGetExternalFilesDir, (jstring)NULL);
  if (oExternalDir)
  {
    jstring sExternalPath = (jstring)env->CallObjectMethod(oExternalDir, midFileGetAbsolutePath);

    temp = env->GetStringUTFChars(sExternalPath, NULL);
    strcpy(storagePath, temp);
    env->ReleaseStringUTFChars(sExternalPath, temp);
    env->DeleteLocalRef(sExternalPath);
    env->DeleteLocalRef(oExternalDir);
  }

  env->DeleteLocalRef(cActivity);

  // Check if we have a valid home path
  //playercore rename HOME as EXT/se_DATA
  if (strlen(storagePath) > 0)
  {
    setenv("XBMC_EXT_DATA", storagePath, 0);
    LOGE("XBMC_EXT_DATA(storagePath) :%s", storagePath );
  }
  else
  {
    setenv("XBMC_EXT_DATA", getenv("XBMC_TEMP"), 0);

    LOGE("XBMC_EXT_DATA(XBMC_TEMP) :%s",  getenv("XBMC_TEMP"));
  }

  std::string strTempDir = getenv("XBMC_EXT_DATA"); strTempDir += "/temp";
  setenv("XBMC_TEMP", strTempDir.c_str(), 0);

  std::string strAssertDir = getenv("XBMC_ANDROID_DATA");strAssertDir += "/files";  
  setenv("XBMC_ASSERT"    , strAssertDir.c_str(), 0);

  std::string strFontDir = getenv("XBMC_ASSERT"); strFontDir += "/media/Fonts";
  setenv("XBMC_FONT"    , strFontDir.c_str(), 0);

  std::string strBDJDir = getenv("XBMC_ASSERT");strBDJDir += "/system";
  setenv("XBMC_BDJ_RESOURCE"    , strBDJDir.c_str(), 0);

  std::string strBDCacheDir = getenv("XBMC_EXT_DATA"); strBDCacheDir += "/cache/";
  setenv("XBMC_BD_CACHE"    , strBDCacheDir.c_str(), 0);

  //set XBMC_LOG in SetLogFolder,path pass form java
  //std::string strLogDir = getenv("XBMC_EXT_DATA");strLogDir += "/log";
  //setenv("XBMC_LOG"    , strLogDir.c_str(), 0);

  std::string strBDLibDir = getenv("XBMC_ANDROID_DATA"); strBDLibDir += "/lib";
  setenv("XBMC_BD_LIB"    , strBDLibDir.c_str(), 0);

  LOGE("XBMC_TEMP :%s", getenv("XBMC_TEMP") );
  LOGE("XBMC_ASSERT :%s",  getenv("XBMC_ASSERT"));
  LOGE("XBMC_FONT :%s",  getenv("XBMC_FONT"));
  LOGE("XBMC_BDJ_RESOURCE :%s",  getenv("XBMC_BDJ_RESOURCE"));
  LOGE("XBMC_BD_CACHE :%s",  getenv("XBMC_BD_CACHE"));
  LOGE("XBMC_LOG :%s",  getenv("XBMC_LOG"));
  LOGE("XBMC_BD_LIB :%s",  getenv("XBMC_BD_LIB"));


}


bool getVidonplayerObject(JNIEnv* env, jobject thiz)
{
  if(AndroidRuntime::Get().m_objVidonplayer)
    env->DeleteGlobalRef(AndroidRuntime::Get().m_objVidonplayer);

  AndroidRuntime::Get().m_objVidonplayer = env->NewGlobalRef(thiz);

  if(AndroidRuntime::Get().m_objVidonplayer == NULL)
    LOGE("AndroidRuntime::Get().m_objVidonplayer is NULL");

  LOGE("AndroidRuntime::Get().m_objVidonplayer successful");

  return true;
}

static void SetLogFolder(const char* path)
{
  setenv("XBMC_LOG",path,0);
  LOGE("XBMC_LOG :%s",  getenv("XBMC_LOG"));
#if defined(USE_BREADPAD)
  if(eh)
    delete eh;
  google_breakpad::MinidumpDescriptor descriptor(path);
  eh = new google_breakpad::ExceptionHandler(descriptor,NULL,DumpCallback,NULL,true,-1);
#endif
}

static void JavaLogOutPut(const char* log)
{
  LOGE("%s",log);
}

static VD_Action App2VDAction(int key)
{
  VD_Action plAction;
  switch(key)
  {
  case 1:
    plAction = VD_ACTION_MOVE_LEFT;
    break;
  case 2:
    plAction = VD_ACTION_MOVE_RIGHT;
    break;
  case 3:
    plAction = VD_ACTION_MOVE_UP;
    break;
  case 4:
    plAction = VD_ACTION_MOVE_DOWN;
    break;
  case 5:
    plAction = VD_ACTION_SELECT_ITEM;
    break;
  case 6:
    plAction = VD_ACTION_MOUSE_MOVE;
    break;
  case 7:
    plAction = VD_ACTION_MOUSE_LEFT_CLICK;
    break;
  case 8:
    plAction = VD_ACTION_NEXT_ITEM;
    break;
  case 9:
    plAction = VD_ACTION_PREV_ITEM;
    break;
  case 10:
    plAction = VD_ACTION_NONE;
    break;
  case 11:
    plAction = VD_ACTION_NONE;
    break;
  case 12:
    plAction = VD_ACTION_SHOW_VIDEOMENU;
    break;
  default:
    plAction = VD_ACTION_NONE;
  }
  return plAction;
}

/*
 * JNI porting 
 */
jstring NFUNC(getSocName)(JNIEnv* env,jobject thiz)
{
  char name[64] = {0,};
  __system_property_get("ro.board.platform",name);
  LOGE("getSocName :%s", name);
  return env->NewStringUTF(name);
}

void NFUNC(ntSetLogEnable)(JNIEnv* env,jobject thiz,jboolean bEnable,jint level)
{
  LOGE("SetLogEnable bEnable: %d, level: %d", bEnable, level);
}

void NFUNC(ntSetLogFolder)(JNIEnv* env,jobject thiz,jstring path)
{
  if(!path)
    return;
  const char* cpath = env->GetStringUTFChars(path,NULL);
  if(!cpath)
    return;
  LOGE("ntSetLogFolder path: %s",cpath);
  SetLogFolder(cpath);
  AndroidRuntime::Get().m_sConfig.strLogPath = cpath;
  env->ReleaseStringUTFChars(path, cpath);
}

void NFUNC(ntSetBDCacheFolder)(JNIEnv* env,jobject thiz,jstring path)
{
  if(!path)
    return;
  const char* cpath = env->GetStringUTFChars(path,NULL);
  if(!cpath)
    return;
  LOGE("ntSetBDCacheFolder path: %s",cpath);
  
  setenv("XBMC_BD_CACHE"    , cpath, 0);
  LOGE("XBMC_BD_CACHE :%s",  getenv("XBMC_BD_CACHE"));
#if defined(USE_BREADPAD)
  if(eh)
    delete eh;
  google_breakpad::MinidumpDescriptor descriptor(cpath);
  eh = new google_breakpad::ExceptionHandler(descriptor,NULL,DumpCallback,NULL,true,-1);
#endif
  env->ReleaseStringUTFChars(path, cpath);
}

void NFUNC(ntLogOutPut)(JNIEnv* env,jobject thiz,jstring log)
{
  if(!log)
    return;
  const char* clog = env->GetStringUTFChars(log,NULL);
  if(!clog)
    return;
 
  JavaLogOutPut(clog);
  env->ReleaseStringUTFChars(log, clog);
}


void NFUNC(ntInitPlayerCore)(JNIEnv* env,jobject thiz,jobject owner)
{
  LOGE("ntInitPlayerCore m_bInitPlayerCore :%d", AndroidRuntime::Get().m_bInitPlayerCore);
  setup_env(env, owner);

  if( false == AndroidRuntime::Get().m_bInitPlayerCore )
  {
    AndroidRuntime::Get().m_pVDPlayer = CreateVDPlayer();

    if (AndroidRuntime::Get().m_pVDPlayer)
    {
      AndroidRuntime::Get().m_pVDPlayerConfig = new CVDPlayerConfig();
      AndroidRuntime::Get().m_pVDPlayerConfig->m_strFontName = "arial.ttf";
      AndroidRuntime::Get().m_pVDPlayerConfig->m_strFontPath = getenv("XBMC_FONT");
      AndroidRuntime::Get().m_pVDPlayerConfig->m_strLogPath = AndroidRuntime::Get().m_sConfig.strLogPath;
      AndroidRuntime::Get().m_pVDPlayerConfig->m_strPlcoreRuntimePath = getenv("XBMC_ASSERT");
      AndroidRuntime::Get().m_pVDPlayerConfig->m_strProfileFolderPath = getenv("XBMC_ASSERT");

      AndroidRuntime::Get().m_pVDPlcoreCallback = new CVDPlcoreCallback();

      if(!AndroidRuntime::Get().m_pVDPlayer->Initialize(AndroidRuntime::Get().m_pVDPlayerConfig, AndroidRuntime::Get().m_pVDPlcoreCallback))
      {
        LOGE("m_pVDPlayer->Initialize failed");
        return;
      }
      AndroidRuntime::Get().m_bInitPlayerCore = true;
    }
    LOGE("playercore init done");
  }
  else
    LOGE("playercore already init");

}
void NFUNC(ntUnInitPlayerCore)(JNIEnv* env,jobject thiz)
{
  if( true == AndroidRuntime::Get().m_bInitPlayerCore )
  {
    LOGE("playercore uninit");

    ReleaseVDPlayer(AndroidRuntime::Get().m_pVDPlayer);  
    AndroidRuntime::Get().m_pVDPlayer = NULL;
    SAFE_DELETE(AndroidRuntime::Get().m_pVDPlayerConfig);
  }

}

void NFUNC(ntInitPlayTool)(JNIEnv* env,jobject thiz)
{
  LOGE("playtool init");

#if !defined(VDPLAYER_FOR_KODI)
  if(AndroidRuntime::Get().m_objSurface)
  {
    AndroidRuntime::Get().m_pNativewindow = ANativeWindow_fromSurface(env, AndroidRuntime::Get().m_objSurface);
  }
  else
  {
    LOGE("get NativeWindow failed form surface!!!!!! ");
    AndroidRuntime::Get().m_pNativewindow = NULL;
  }
  AndroidRuntime::Get().m_sConfig.handle = (void**)&( AndroidRuntime::Get().m_pNativewindow);
#endif //VDPLAYER_FOR_KODI
  AndroidRuntime::Get().m_sConfig.nWidth = AndroidRuntime::Get().m_pNativewindow?ANativeWindow_getWidth(AndroidRuntime::Get().m_pNativewindow):0;
  AndroidRuntime::Get().m_sConfig.nHeight = AndroidRuntime::Get().m_pNativewindow?ANativeWindow_getHeight(AndroidRuntime::Get().m_pNativewindow):0;
  
  //patch for android phone from home page playback
  if(AndroidRuntime::Get().m_sConfig.nHeight > AndroidRuntime::Get().m_sConfig.nWidth)
  {
#define SWAP(x,y)\
    { \
      int temp = x; \
      x = y; \
      y = temp; \
    }
    SWAP(AndroidRuntime::Get().m_sConfig.nWidth, AndroidRuntime::Get().m_sConfig.nHeight);
  }

  LOGE("get NativeWindow width :%d", AndroidRuntime::Get().m_sConfig.nWidth );
  LOGE("get NativeWindow height :%d", AndroidRuntime::Get().m_sConfig.nHeight);

  AndroidRuntime::Get().m_pVDPlayToolConfig = new CVDPlayToolConfig();
  AndroidRuntime::Get().m_pVDPlayToolConfig->SetRenderWindowHandle( AndroidRuntime::Get().m_sConfig.handle);
  AndroidRuntime::Get().m_pVDPlayToolConfig->SetRenderWindowInitWidth( AndroidRuntime::Get().m_sConfig.nWidth);
  AndroidRuntime::Get().m_pVDPlayToolConfig->SetRenderWindowInitHeight(AndroidRuntime::Get().m_sConfig.nHeight);

  AndroidRuntime::Get().m_pVDPlaytoolCallback = new CVDPlaytoolCallback();

  AndroidRuntime::Get().m_pVDPlayTool = AndroidRuntime::Get().m_pVDPlayer->CreatePlayTool(
    AndroidRuntime::Get().m_sConfig.ePlayerType,
    AndroidRuntime::Get().m_pVDPlayToolConfig, 
    AndroidRuntime::Get().m_pVDPlaytoolCallback);

  if (!AndroidRuntime::Get().m_pVDPlayTool)
  {
    LOGE("creat m_pVDPlayTool failed");
    return;
  }

  //apply charset
  AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_SUBTITLES_CHARSET, AndroidRuntime::Get().m_sConfig.strCharset.c_str());
  AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_LOCALE_SUBTITLELANGUAGE, AndroidRuntime::Get().m_sConfig.playerParam.settingParam.szSubtitleLanguage);
  AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_LOCALE_AUDIOLANGUAGE, AndroidRuntime::Get().m_sConfig.playerParam.settingParam.szAudioLanguage);

  // apply audioOutput
  bool isPassthroughOn = !!AndroidRuntime::Get().m_sConfig.isPassthroughOn;
  std::string strAudioDevice = AndroidRuntime::Get().m_sConfig.audioDevice;
  LOGE("playtool init ,audiodevice :%s, passthrough :%d", strAudioDevice.c_str(), isPassthroughOn);

  AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_AUDIOOUTPUT_PASSTHROUGH, isPassthroughOn);

  if(isPassthroughOn)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_AUDIOOUTPUT_PASSTHROUGHDEVICE, strAudioDevice.c_str());

    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_AUDIOOUTPUT_AC3PASSTHROUGH, true);
    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_AUDIOOUTPUT_EAC3PASSTHROUGH, true);
    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_AUDIOOUTPUT_DTSPASSTHROUGH, true);
    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_AUDIOOUTPUT_TRUEHDPASSTHROUGH, true);
    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_AUDIOOUTPUT_DTSHDPASSTHROUGH, true);
  }
  else
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_AUDIOOUTPUT_AUDIODEVICE, strAudioDevice.c_str());
  }

  AndroidRuntime::Get().m_pVDPlayTool->NotifyAudioOutputSettingsChanged();

 if(!AndroidRuntime::Get().m_pVDPlayTool->Init()) 
 {
   LOGE("m_pVDPlayTool->Init failed");
 }
}
void NFUNC(ntUnInitPlayTool)(JNIEnv* env,jobject thiz)
{
  LOGE("playtool uninit");

  if(AndroidRuntime::Get().m_pVDPlayTool && AndroidRuntime::Get().m_pVDPlayer)
  {
    AndroidRuntime::Get().m_pVDPlayTool->UnInit();
    AndroidRuntime::Get().m_pVDPlayer->ReleasePlayTool(AndroidRuntime::Get().m_pVDPlayTool);
    AndroidRuntime::Get().m_pVDPlayTool = NULL;
  }
  else
    LOGE("release m_pVDPlayTool failed");

  SAFE_DELETE(AndroidRuntime::Get().m_pVDPlayToolConfig);
  SAFE_DELETE(AndroidRuntime::Get().m_pVDPlaytoolCallback); 

}

void NFUNC(ntInit)( JNIEnv* env,jobject thiz)
{
  //
  LOGE("plcore init start");
#if !defined(VDPLAYER_FOR_KODI)
  if(AndroidRuntime::Get().m_objSurface)
  {
    AndroidRuntime::Get().m_pNativewindow = ANativeWindow_fromSurface(env, AndroidRuntime::Get().m_objSurface);
  }
  else
  {
    LOGE("get NativeWindow failed form surface!!!!!! ");
    AndroidRuntime::Get().m_pNativewindow = NULL;
  }
  AndroidRuntime::Get().m_sConfig.handle = (void**)&( AndroidRuntime::Get().m_pNativewindow);
#endif //VDPLAYER_FOR_KODI
  AndroidRuntime::Get().m_sConfig.nWidth = AndroidRuntime::Get().m_pNativewindow?ANativeWindow_getWidth(AndroidRuntime::Get().m_pNativewindow):0;
  AndroidRuntime::Get().m_sConfig.nHeight = AndroidRuntime::Get().m_pNativewindow?ANativeWindow_getHeight(AndroidRuntime::Get().m_pNativewindow):0;

  LOGE("get NativeWindow width :%d", AndroidRuntime::Get().m_sConfig.nWidth );
  LOGE("get NativeWindow height :%d", AndroidRuntime::Get().m_sConfig.nHeight);

  AndroidRuntime::Get().m_pVDPlayer = CreateVDPlayer();
  if (AndroidRuntime::Get().m_pVDPlayer)
  {
    AndroidRuntime::Get().m_pVDPlayerConfig = new CVDPlayerConfig();
    AndroidRuntime::Get().m_pVDPlayerConfig->m_strFontName = AndroidRuntime::Get().m_sConfig.strFontName;
    AndroidRuntime::Get().m_pVDPlayerConfig->m_strFontPath = AndroidRuntime::Get().m_sConfig.strFontPath;
    AndroidRuntime::Get().m_pVDPlayerConfig->m_strLogPath = AndroidRuntime::Get().m_sConfig.strLogPath;
    AndroidRuntime::Get().m_pVDPlayerConfig->m_strPlcoreRuntimePath = AndroidRuntime::Get().m_sConfig.strRuntimePath;
    AndroidRuntime::Get().m_pVDPlayerConfig->m_strProfileFolderPath = AndroidRuntime::Get().m_sConfig.strProfileFolderPath;

    AndroidRuntime::Get().m_pVDPlcoreCallback = new CVDPlcoreCallback();

    if(!AndroidRuntime::Get().m_pVDPlayer->Initialize(AndroidRuntime::Get().m_pVDPlayerConfig, AndroidRuntime::Get().m_pVDPlcoreCallback))
    {
      LOGE("m_pVDPlayer->Initialize failed");
      return;
    }

    AndroidRuntime::Get().m_pVDPlayToolConfig = new CVDPlayToolConfig();
    AndroidRuntime::Get().m_pVDPlayToolConfig->SetRenderWindowHandle( AndroidRuntime::Get().m_sConfig.handle);
    AndroidRuntime::Get().m_pVDPlayToolConfig->SetRenderWindowInitWidth( AndroidRuntime::Get().m_sConfig.nWidth);
    AndroidRuntime::Get().m_pVDPlayToolConfig->SetRenderWindowInitHeight(AndroidRuntime::Get().m_sConfig.nHeight);

    AndroidRuntime::Get().m_pVDPlaytoolCallback = new CVDPlaytoolCallback();

    AndroidRuntime::Get().m_pVDPlayTool = AndroidRuntime::Get().m_pVDPlayer->CreatePlayTool(
      AndroidRuntime::Get().m_sConfig.ePlayerType,
      AndroidRuntime::Get().m_pVDPlayToolConfig, 
      AndroidRuntime::Get().m_pVDPlaytoolCallback);

    if (!AndroidRuntime::Get().m_pVDPlayTool || !AndroidRuntime::Get().m_pVDPlayTool->Init()) 
    {
      LOGE("m_pVDPlayTool->Init failed");
      return;
    }

    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_SUBTITLES_CHARSET, AndroidRuntime::Get().m_sConfig.strCharset.c_str());
    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_LOCALE_SUBTITLELANGUAGE, AndroidRuntime::Get().m_sConfig.playerParam.settingParam.szSubtitleLanguage);
    AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_LOCALE_AUDIOLANGUAGE, AndroidRuntime::Get().m_sConfig.playerParam.settingParam.szAudioLanguage);

    AndroidRuntime::Get().m_pVDPlayerCallback = new CVDPlayerCallback();

    AndroidRuntime::Get().m_pVDCorePlayer = AndroidRuntime::Get().m_pVDPlayTool->GetPlayer(AndroidRuntime::Get().m_pVDPlayerCallback);
    if(!AndroidRuntime::Get().m_pVDCorePlayer)
    {
      LOGE("m_pVDPlayTool->GetPlayer failed");
      return;
    }

    LOGE("ntInit done");
  }
  else
  {
    LOGE("CreateVDPlayer failed");
  }
  LOGE("plcore init end");
}

void NFUNC(ntSetup)(JNIEnv* env,jobject thiz)
{
  getVidonplayerObject(env, thiz);
}

void NFUNC(ntConfig)(JNIEnv* env,jobject thiz, jobject owner)
{
  //TODO. Get Java Setting Path
  //setup_env(env, owner);

  LOGE(" ntConfig...");
#ifdef VDPLAYER_FOR_KODI
  jfieldID fid = env->GetFieldID(AndroidRuntime::Get().m_clsVidonplayer, "mConfig", "Lorg/xbmc/kodi/VidonPlayer$NativeConfigs;");
#else
  jfieldID fid = env->GetFieldID(AndroidRuntime::Get().m_clsVidonplayer, "mConfig", "Lorg/vidonme/player/VidonPlayer$NativeConfigs;");
#endif

  if(!fid)
  {
    LOGE("get filedID failed!!!");
    return;
  }
  jobject confObj =  env->GetObjectField(thiz, fid);
  if (!confObj)
  {
    LOGE(" get configObject failed!!");
    return;
  }
  jclass clazz = env->GetObjectClass(confObj);

  //system setting 
  static std::string strCharset;
  static std::string strLogPath;

  jstring jlogpath;
  if ((jlogpath = JNI::get_jstring_field(env,clazz,confObj,"logPath")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(jlogpath, NULL);
    strLogPath = cstr?cstr:"";
    env->ReleaseStringUTFChars(jlogpath, cstr);
    env->DeleteLocalRef(jlogpath);
    LOGE("Get logpath :%s", strLogPath.c_str());
  }

  jstring js;
  if ((js = JNI::get_jstring_field(env,clazz,confObj,"charSet")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(js, NULL);
    strCharset = cstr?cstr:"";
    env->ReleaseStringUTFChars(js, cstr);
    env->DeleteLocalRef(js);
    LOGE("Get charset :%s", strCharset.c_str());
  }

  jint playlist = JNI::get_jint_field(env,clazz,confObj,"playlist");
  jint bdPlaymode = JNI::get_jint_field(env,clazz,confObj,"bdPlaymode");
  jfloat starttime = JNI::get_jfloat_field(env,clazz,confObj,"starttime");

  LOGE("Get playlist :%d", playlist);
  LOGE("Get bdPlaymode :%d", bdPlaymode);
  LOGE("Get starttime :%f", starttime);
  
  //ae setting
  static std::string straudioDevice;
  jboolean isPassthroughOn = JNI::get_jboolean_field(env,clazz,confObj,"isPassthroughOn");
  jstring audioDevice;
  if ((audioDevice = JNI::get_jstring_field(env,clazz,confObj,"audioDevice")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(audioDevice, NULL);
    straudioDevice = cstr?cstr:"";
    env->ReleaseStringUTFChars(audioDevice, cstr);
    env->DeleteLocalRef(audioDevice);
    LOGE("Get straudioDevice :%s", straudioDevice.c_str());
  }

  //vmf params
  static std::string strserverIP;
  static std::string strvmfPath;  
  static std::string strvmfMode;  

  jint playerType = JNI::get_jint_field(env,clazz,confObj,"playerType");
  jint sourceType = JNI::get_jint_field(env,clazz,confObj,"sourceType");

  LOGE("Get playType :%d", playerType);
  LOGE("Get sourceType :%d", sourceType);

  jint vtxPort = JNI::get_jint_field(env,clazz,confObj,"vtxPort");
  jint tcpPort = JNI::get_jint_field(env,clazz,confObj,"tcpPort");

  LOGE("Get vtxPort :%d", vtxPort);
  LOGE("Get tcpPort :%d", tcpPort);

  jint tokenLocal = JNI::get_jint_field(env,clazz,confObj,"tokenLocal");
  jint tokenRemote = JNI::get_jint_field(env,clazz,confObj,"tokenRemote");
  
  LOGE("Get tokenLocal :%d", tokenLocal );
  LOGE("Get tokenRemote :%d", tokenRemote );

  jstring serverIP;
  if ((serverIP = JNI::get_jstring_field(env,clazz,confObj,"serverIP")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(serverIP, NULL);
    strserverIP = cstr?cstr:"";
    env->ReleaseStringUTFChars(serverIP, cstr);
    env->DeleteLocalRef(serverIP);
    LOGE("Get strserverIP :%s", strserverIP.c_str());
  }

  jstring vmfPath;
  if ((vmfPath = JNI::get_jstring_field(env,clazz,confObj,"vmfPath")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(vmfPath, NULL);
    strvmfPath = cstr?cstr:"";
    env->ReleaseStringUTFChars(vmfPath, cstr);
    env->DeleteLocalRef(vmfPath);
    LOGE("Get strvmfPath :%s", strvmfPath.c_str());
  }

  jstring vmfMode;
  if ((vmfMode = JNI::get_jstring_field(env,clazz,confObj,"vmfMode")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(vmfMode, NULL);
    strvmfMode = cstr?cstr:"";
    env->ReleaseStringUTFChars(vmfMode, cstr);
    env->DeleteLocalRef(vmfMode);
    LOGE("Get strvmfMode :%s", strvmfMode.c_str());
  }


  //vmf setting 
  jfloat fps = JNI::get_jfloat_field(env,clazz,confObj,"fps");
  jint bitrate = JNI::get_jint_field(env,clazz,confObj,"bitrate");
  
  LOGE("Get fps :%f", fps);
  LOGE("Get bitrate :%d", bitrate);
  
  static std::string straudiolanguage;
  jstring audiolanguage;
  if ((audiolanguage = JNI::get_jstring_field(env,clazz,confObj,"langAudio")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(audiolanguage, NULL);
    straudiolanguage = cstr?cstr:"";
    env->ReleaseStringUTFChars(audiolanguage, cstr);
    env->DeleteLocalRef(audiolanguage);
    LOGE("Get straudiolanguage :%s", straudiolanguage.c_str());
  }

  static std::string strsublanguage;
  jstring sublanguage;
  if ((sublanguage = JNI::get_jstring_field(env,clazz,confObj,"langSub")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(sublanguage, NULL);
    strsublanguage = cstr?cstr:"";
    env->ReleaseStringUTFChars(sublanguage, cstr);
    env->DeleteLocalRef(sublanguage);
    LOGE("Get strsublanguage :%s", strsublanguage.c_str());
  }

  static std::string strquality;
  jstring quality;
  if ((quality = JNI::get_jstring_field(env,clazz,confObj,"quality")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(quality, NULL);
    strquality = cstr?cstr:"";
    env->ReleaseStringUTFChars(quality, cstr);
    env->DeleteLocalRef(quality);
    LOGE("Get strquality :%s", strquality.c_str());
  }

  static std::string strdeviceResolution;
  jstring deviceResolution;
  if ((deviceResolution = JNI::get_jstring_field(env,clazz,confObj,"deviceResolution")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(deviceResolution, NULL);
    strdeviceResolution = cstr?cstr:"";
    env->ReleaseStringUTFChars(deviceResolution, cstr);
    env->DeleteLocalRef(deviceResolution);
    LOGE("Get strdeviceResolution :%s", strdeviceResolution.c_str());
  }

  static std::string strnetworkMode;
  jstring networkMode;
  if ((networkMode = JNI::get_jstring_field(env,clazz,confObj,"networkMode")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(networkMode, NULL);
    strnetworkMode = cstr?cstr:"";
    env->ReleaseStringUTFChars(networkMode, cstr);
    env->DeleteLocalRef(networkMode);
    LOGE("Get strnetworkMode :%s", strnetworkMode.c_str());
  }

  static std::string strdeviceName;
  jstring deviceName;
  if ((deviceName = JNI::get_jstring_field(env,clazz,confObj,"deviceName")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(deviceName, NULL);
    strdeviceName = cstr?cstr:"";
    env->ReleaseStringUTFChars(deviceName, cstr);
    env->DeleteLocalRef(deviceName);
    LOGE("Get strdeviceName :%s", strdeviceName.c_str());
  }

  static std::string strdeviceType;
  jstring deviceType;
  if ((deviceType = JNI::get_jstring_field(env,clazz,confObj,"deviceType")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(deviceType, NULL);
    strdeviceType = cstr?cstr:"";
    env->ReleaseStringUTFChars(deviceType, cstr);
    env->DeleteLocalRef(deviceType);
    LOGE("Get strdeviceType :%s", strdeviceType.c_str());
  }

  static std::string strdeviceVersion;
  jstring deviceVersion;
  if ((deviceVersion = JNI::get_jstring_field(env,clazz,confObj,"deviceVersion")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(deviceVersion, NULL);
    strdeviceVersion = cstr?cstr:"";
    env->ReleaseStringUTFChars(deviceVersion, cstr);
    env->DeleteLocalRef(deviceVersion);
    LOGE("Get strdeviceVersion :%s", strdeviceVersion.c_str());
  }
  static std::string strclientName;
  jstring clientName;
  if ((clientName = JNI::get_jstring_field(env,clazz,confObj,"clientName")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(clientName, NULL);
    strclientName = cstr?cstr:"";
    env->ReleaseStringUTFChars(clientName, cstr);
    env->DeleteLocalRef(clientName);
    LOGE("Get strclientName :%s", strclientName.c_str());
  }
  static std::string strclientVersion;
  jstring clientVersion;
  if ((clientVersion = JNI::get_jstring_field(env,clazz,confObj,"clientVersion")) != NULL)
  {
    const char* cstr = env->GetStringUTFChars(clientVersion, NULL);
    strclientVersion = cstr?cstr:"";
    env->ReleaseStringUTFChars(clientVersion, cstr);
    env->DeleteLocalRef(clientVersion);
    LOGE("Get strclientVersion :%s", strclientVersion.c_str());
  }

  jboolean isAD = JNI::get_jboolean_field(env,clazz,confObj,"IsAD");

  env->DeleteLocalRef(clazz);

  //plcore config
  AndroidRuntime::Get().m_sConfig.ePlayerType = (VD_PlayerType)playerType;//VD_PLAYER_SYS;//VD_PLAYER_PLCORE;
  AndroidRuntime::Get().m_sConfig.strFontName = "arial.ttf";
  AndroidRuntime::Get().m_sConfig.strFontPath = getenv("XBMC_FONT");
  AndroidRuntime::Get().m_sConfig.strRuntimePath = getenv("XBMC_ASSERT");
  AndroidRuntime::Get().m_sConfig.strProfileFolderPath = getenv("XBMC_ASSERT");
  AndroidRuntime::Get().m_sConfig.strLogPath = strLogPath;
  AndroidRuntime::Get().m_sConfig.strCharset = strCharset; 

  //plcore playback
  AndroidRuntime::Get().m_sConfig.bdPlayMode = bdPlaymode;
  AndroidRuntime::Get().m_sConfig.playlist = (bdPlaymode==0)?-1:(bdPlaymode==1)?0xfffff:(bdPlaymode==2)?playlist:0xfffff;
  LOGE("Get m_sConfig.playlist :%d", AndroidRuntime::Get().m_sConfig.playlist);

  //aesing setting
  AndroidRuntime::Get().m_sConfig.isPassthroughOn = isPassthroughOn;
  AndroidRuntime::Get().m_sConfig.audioDevice = straudioDevice;
 
  //static char szClientName[]    = "vidon";
  //static char szClientVersion[] = "2.0";
  ////static char szDeviceVersion[] = "1.2";
  //static char szSubCodePage[]   = "GB18030";

  vidonPlayerParam params;
  memset(&params, 0, sizeof(vidonPlayerParam));
  params.szIP             = strserverIP.c_str();
  params.nTcpPort         = tcpPort;
  params.nVtxPort         = vtxPort;
  params.tokenLocal       = tokenLocal;
  params.tokenRemote      = tokenRemote;
  params.szMovie          = strvmfPath.c_str();
  params.szMode           = strvmfMode.c_str();
  params.sourceType       = (SOURCE_TYPE)sourceType;
  params.bIsad            = isAD;
  params.nMenuMode        = bdPlaymode;

  params.deviceParam.fFps              = fps;
  params.deviceParam.deviceresolution  = strdeviceResolution.c_str();//"1920x1080";
  params.deviceParam.max_resolution    = strdeviceResolution.c_str();
  params.deviceParam.szClientName      = strclientName.c_str();
  params.deviceParam.szClientVersion   = strclientVersion.c_str();
  params.deviceParam.szDeviceName      = strdeviceName.c_str();//"box";
  params.deviceParam.szDeviceType      = strdeviceType.c_str();//"box";
  params.deviceParam.szDeviceVersion   = strdeviceVersion.c_str();
  params.deviceParam.szNetworkMode     = strnetworkMode.c_str();//"wifi";

  params.settingParam.szAudioLanguage  = straudiolanguage.c_str();//"English";
  params.settingParam.szSubtitleLanguage = strsublanguage.c_str();//"Chinese";
  params.settingParam.szQuality         = strquality.c_str();
  params.settingParam.szOutSubCodePage   = strCharset.c_str();
  params.settingParam.nSubtitleType      = 1;
  params.settingParam.bAudioHD         = isPassthroughOn;
  params.settingParam.nMaxBitrate      = bitrate;

  AndroidRuntime::Get().m_sConfig.playerParam = params;

  LOGE("AndroidRuntime::Get().m_sConfig.ePlayerType :%d", AndroidRuntime::Get().m_sConfig.ePlayerType);
  LOGE("AndroidRuntime::Get().m_sConfig.strFontName :%s", AndroidRuntime::Get().m_sConfig.strFontName.c_str());
  LOGE("AndroidRuntime::Get().m_sConfig.strFontPath :%s", AndroidRuntime::Get().m_sConfig.strFontPath.c_str());
  LOGE("AndroidRuntime::Get().m_sConfig.strRuntimePath :%s", AndroidRuntime::Get().m_sConfig.strRuntimePath.c_str());
  LOGE("AndroidRuntime::Get().m_sConfig.strLogPath :%s", AndroidRuntime::Get().m_sConfig.strLogPath.c_str());

  LOGE("Config done!");
}

void NFUNC(ntPrepare)(JNIEnv*env, jobject thiz)
{
  LOGE("ntPrepare ...");
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    AndroidRuntime::Get().m_pVDCorePlayer->prepare();
  }
  else
  {
    LOGE("ntPrepare failed");
  }
  LOGE("ntprepare done");
}

void NFUNC(ntPrepareAsync)(JNIEnv*env, jobject thiz)
{
  LOGE("ntPrepareAsync ...");
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    AndroidRuntime::Get().m_pVDCorePlayer->prepareAsync();
  }
  else
  {
    LOGE("ntPrepare failed");
  }
  LOGE("ntprepare done");
}


void NFUNC(ntSetVideoSurface)(JNIEnv* env,jobject thiz,jobject jsurface)
{
  if(jsurface==NULL){
    env->ThrowNew(env->GetObjectClass(thiz),"java/IllegalArgumentException");
    return;
  }

  if(AndroidRuntime::Get().m_objSurface)
    env->DeleteGlobalRef(AndroidRuntime::Get().m_objSurface);

  AndroidRuntime::Get().m_objSurface = env->NewGlobalRef(jsurface);

  LOGE("ntsetVideoSurface done");
}

void NFUNC(ntUnSetVideoSurface)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pNativewindow)
  {
    ANativeWindow_release(AndroidRuntime::Get().m_pNativewindow);
    AndroidRuntime::Get().m_pNativewindow = NULL;
  }

  if(AndroidRuntime::Get().m_objSurface)
  {
    env->DeleteGlobalRef(AndroidRuntime::Get().m_objSurface);
    AndroidRuntime::Get().m_objSurface = NULL;
  }
  LOGE("destory AndroidRuntime nativewindow and surface");
}

void NFUNC(ntSetViewMode)(JNIEnv* env,jobject thiz,jint viewmode)
{
  if ( AndroidRuntime::Get().m_pVDPlayTool )
    AndroidRuntime::Get().m_pVDPlayTool->SetViewMode((VD_ViewMode)viewmode);

  LOGE("ntSetViewMode:%d", viewmode);
}

void NFUNC(ntNotifyViewModeChanged)(JNIEnv* env,jobject thiz)
{
  if ( AndroidRuntime::Get().m_pVDPlayTool )
    AndroidRuntime::Get().m_pVDPlayTool->NotifyViewModeChanged();

  LOGE("call ntNotifyViewModeChanged");
}

void NFUNC(ntSetRestoreStream)(JNIEnv* env,jobject thiz,jint asIndex,jint ssIndex,jstring spath)
{
  const char *subpath = NULL;

  if(spath){
    subpath = env->GetStringUTFChars(spath, NULL);
  }

  LOGE("ntSetRestoreStream asIndex:%d,ssIndex:%d,subpath:%s",asIndex,ssIndex,subpath);


  if(subpath)
    env->ReleaseStringUTFChars(spath, subpath);
}

void NFUNC(ntSetBdPlayMode)(JNIEnv* env,jobject thiz,jint mode,jint playlist)
{
  LOGE("ntSetBdPlayMode mode:%d,playlist:%d",mode,playlist);
}

void NFUNC(ntSetDataSource)(JNIEnv* env,jobject thiz,jstring path)
{
  if(!path){
    env->ThrowNew(env->GetObjectClass(thiz),"java/lang/IllegalArgumentException");
    return;
  }

  const char *filepath = env->GetStringUTFChars(path, NULL);
  if (!filepath) 
  {
    return;
  }
  LOGE("ntsetDataSource filepath :%s",filepath );

  AndroidRuntime::Get().m_pVDPlayerCallback = new CVDPlayerCallback();

  AndroidRuntime::Get().m_pVDCorePlayer = AndroidRuntime::Get().m_pVDPlayTool->GetPlayer(AndroidRuntime::Get().m_pVDPlayerCallback);

  if(!AndroidRuntime::Get().m_pVDCorePlayer)
  {
    LOGE("m_pVDPlayTool->GetPlayer failed");
    env->ReleaseStringUTFChars(path, filepath);
    return;
  }

  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    CLocalFileItem item;
    item.m_path = filepath;

    CLocalOptions options;

    //how get these

    //options.m_nStartTime  = ;
    //options.m_nStartPercent =;
    options.m_nPlaylist = AndroidRuntime::Get().m_sConfig.playlist; //by ui setting
    //options.m_nPreSelectedAudioIdx = ;
    //options.m_nPreSelectedSubtitleIdx =;

    SOURCE_TYPE sourceType = AndroidRuntime::Get().m_sConfig.playerParam.sourceType;

    if(!AndroidRuntime::Get().m_pVDCorePlayer->OpenFile(&item, &options,
      (sourceType == SOURCE_NET)? &(AndroidRuntime::Get().m_sConfig.playerParam):NULL))
    {
      LOGE("m_pVDCorePlayer->OpenFile failed");
    }
    else
      LOGE("m_pVDCorePlayer->OpenFile successful");

  }
  env->ReleaseStringUTFChars(path, filepath);
}

jboolean NFUNC(ntStart)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    AndroidRuntime::Get().m_pVDCorePlayer->doPlay();
  }

  LOGE(" ntStart successful!!");
  return true;
}

void NFUNC(ntStop)(JNIEnv* env,jobject thiz,jboolean bwait)
{
  LOGE("ntStop start");
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    AndroidRuntime::Get().m_pVDCorePlayer->CloseFile();
    AndroidRuntime::Get().m_pVDPlayTool->ReleasePlayer(AndroidRuntime::Get().m_pVDCorePlayer);\
    AndroidRuntime::Get().m_pVDCorePlayer = NULL;
    SAFE_DELETE(AndroidRuntime::Get().m_pVDPlayerCallback);
  }
  LOGE(" ntStop successful!!");
}

void NFUNC(ntPause)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    AndroidRuntime::Get().m_pVDCorePlayer->Pause();
  }
}

void NFUNC(ntResume)(JNIEnv* env,jobject thiz)
{
  CVDCorePlayerPlcore *pVDCorePlayerPlcore = dynamic_cast<CVDCorePlayerPlcore*>(AndroidRuntime::Get().m_pVDCorePlayer);
  if(pVDCorePlayerPlcore)
  {
    pVDCorePlayerPlcore->Pause();
    return;
  }

  CVDCorePlayerSys *pVDCorePlayerSys = dynamic_cast<CVDCorePlayerSys*>(AndroidRuntime::Get().m_pVDCorePlayer);
  if(pVDCorePlayerSys)
  {
    pVDCorePlayerSys->doPlay();
    return;
  }
  LOGE("ntResume something wrong ");
}


void NFUNC(ntRelease)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDPlayTool && AndroidRuntime::Get().m_pVDPlayer)
  {
    AndroidRuntime::Get().m_pVDPlayTool->UnInit();
    AndroidRuntime::Get().m_pVDPlayer->ReleasePlayTool(AndroidRuntime::Get().m_pVDPlayTool);
    AndroidRuntime::Get().m_pVDPlayTool = NULL;
  }
  else
    LOGE("release m_pVDPlayTool failed");

  SAFE_DELETE(AndroidRuntime::Get().m_pVDPlayToolConfig);
  SAFE_DELETE(AndroidRuntime::Get().m_pVDPlaytoolCallback); 
  SAFE_DELETE(AndroidRuntime::Get().m_pPlcoreCallback); 

  //now need not release VDPlayer
  //ReleaseVDPlayer(AndroidRuntime::Get().m_pVDPlayer);  
  //AndroidRuntime::Get().m_pVDPlayer = NULL;
  //SAFE_DELETE(AndroidRuntime::Get().m_pVDPlayerConfig);
}



void NFUNC(ntReset)(JNIEnv* env,jobject thiz)
{
  LOGE("ntReseet."); 
  CVDCorePlayerSys *pVDCorePlayerSys = dynamic_cast<CVDCorePlayerSys*>(AndroidRuntime::Get().m_pVDCorePlayer);
  if(pVDCorePlayerSys)
  {
    pVDCorePlayerSys->reset();
    return;
  }

}

jint NFUNC(ntGetFileDuration)(JNIEnv* env,jobject thiz,jstring src)
{
  return 0;
}

jboolean NFUNC(ntIsPlaying)(JNIEnv* env,jobject thiz)
{

  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->IsPlaying();
  }

  return false;
}

jint NFUNC(ntIsPaused)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->IsPaused();
  }

  return false;
}

jboolean NFUNC(ntIsCaching)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->IsCaching();
  }
  return false;

}

jint NFUNC(ntGetCacheLevel)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->GetCacheLevel();
  }
  return 0;
}

void NFUNC(ntSetPlaySpeed)(JNIEnv* env,jobject thiz,jfloat iSpeed)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->ToFFRW(iSpeed);
  }

}

jfloat NFUNC(ntGetPlaySpeed)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->GetPlaySpeed();
  }

  return -1;
}


void NFUNC(ntOnAction)(JNIEnv* env,jobject thiz, jint id, jfloat x, jfloat y)
{

  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    VD_Action action = App2VDAction(id);

    AndroidRuntime::Get().m_pVDCorePlayer->OnAction(action, x, y);
  }
}

jint NFUNC(ntGetVideoWidth)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    VD_PlayerVideoStreamInfo* pVideoStreamInfo = AndroidRuntime::Get().m_pVDCorePlayer->GetVideoStreamInfo();

    if(pVideoStreamInfo)
    {
      int width = pVideoStreamInfo->width;
      AndroidRuntime::Get().m_pVDCorePlayer->ReleaseVideoStreamInfo(pVideoStreamInfo);
      return width;
    }
  }
  return 0;
}
jint NFUNC(ntGetVideoHeight)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    VD_PlayerVideoStreamInfo* pVideoStreamInfo = AndroidRuntime::Get().m_pVDCorePlayer->GetVideoStreamInfo();

    if(pVideoStreamInfo)
    {
      int height = pVideoStreamInfo->height;
      AndroidRuntime::Get().m_pVDCorePlayer->ReleaseVideoStreamInfo(pVideoStreamInfo);
      return height;
    }
  }  return 0;
}

jint NFUNC(ntGetCurrentPosition)(JNIEnv* env,jobject thiz)
{
  //mseconds
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return (AndroidRuntime::Get().m_pVDCorePlayer->GetTime())*1000;
  }
  return -1;

}

jint NFUNC(ntGetDuration)(JNIEnv* env,jobject thiz)
{
  //mseconds
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return (AndroidRuntime::Get().m_pVDCorePlayer->GetTotalTime())*1000;
  }
  return -1;
}

void NFUNC(ntSeekTo)(JNIEnv* env,jobject thiz, jint msec)
{
  //mseconds
  LOGE("ntSeekTo msec: %d", msec);

  CVDCorePlayerPlcore *pVDCorePlayerPlcore = dynamic_cast<CVDCorePlayerPlcore*>(AndroidRuntime::Get().m_pVDCorePlayer);
  if(pVDCorePlayerPlcore)
  {
    pVDCorePlayerPlcore->SeekTime(msec/1000);
    return;
  }

  CVDCorePlayerSys *pVDCorePlayerSys = dynamic_cast<CVDCorePlayerSys*>(AndroidRuntime::Get().m_pVDCorePlayer);
  if(pVDCorePlayerSys)
  {
    pVDCorePlayerSys->SeekTime(msec);
    return;
  }
}

jint NFUNC(ntGetAudioStreamCount)(JNIEnv* env,jobject thiz)
{
  int audioCount = 0;
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    audioCount = AndroidRuntime::Get().m_pVDCorePlayer->GetAudioStreamCount();
  }
  LOGE("ntGetAudioStreamCount audioCount :%d", audioCount);
  return audioCount;
}

jint NFUNC(ntGetAudioStreamIndex)(JNIEnv* env,jobject thiz)
{
  int audioindex = -1;
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    audioindex = AndroidRuntime::Get().m_pVDCorePlayer->GetAudioStream();
  }
  LOGE("ntGetAudioStreamIndex audioindex :%d", audioindex);

  return audioindex;
}

jstring NFUNC(ntGetAudioStreamInfoByIndex)(JNIEnv* env,jobject thiz, jint index)
{
  std::string strAudioStreamInfo;


  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    VD_PlayerAudioStreamInfo* p = AndroidRuntime::Get().m_pVDCorePlayer->GetAudioStreamInfo(index);
    if(p)
    {
      std::string language;
      std::string codec;
      std::string channels;
      if(p->language)
      {
        language = p->language;
        strAudioStreamInfo.append(language);
      }
      if(p->audioCodecName)
      {
        codec = p->audioCodecName;
        if(strAudioStreamInfo.size() && codec.size())
        {
          strAudioStreamInfo.append("-");
        }
        strAudioStreamInfo.append(codec);
      }
      if (p->name)
      {
        channels = p->name;
        if(strAudioStreamInfo.size() && channels.size())
        {
          strAudioStreamInfo.append(" ");
        }
        strAudioStreamInfo.append(channels);
      }
      
      AndroidRuntime::Get().m_pVDCorePlayer->ReleaseAudioStreamInfo(p);
    }
    if(strAudioStreamInfo.empty())
      strAudioStreamInfo = "Unknown";

  }
  LOGE("ntGetAudioStreamInfoByIndex strAudioStreamInfo : %s", strAudioStreamInfo.c_str());

  return env->NewStringUTF(strAudioStreamInfo.c_str());

}

void NFUNC(ntSetAudioStreamIndex)(JNIEnv* env,jobject thiz,jint index)
{
  LOGE("ntSetAudioStreamIndex :%d", index);

  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    AndroidRuntime::Get().m_pVDCorePlayer->SetAudioStream(index);
  }
}

jint NFUNC(ntGetSubtitleCount)(JNIEnv* env,jobject thiz,jboolean bupdate)
{
  int subCount = 0;
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    subCount = AndroidRuntime::Get().m_pVDCorePlayer->GetSubtitleCount(bupdate);
  }
  LOGE("ntGetSubtitleCount subCount :%d", subCount);
  return subCount;
}

jint NFUNC(ntAddSubtitle)(JNIEnv* env,jobject thiz, jstring subpath)
{
  if(!subpath)
    return -1;

  const char* path = env->GetStringUTFChars(subpath,NULL);
  LOGE("ntAddSubtitle path :%s", path);

  if(!path)
    return -1;

  int subindex = -1;
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    subindex = AndroidRuntime::Get().m_pVDCorePlayer->AddSubtitle( path );
  }

  env->ReleaseStringUTFChars(subpath, path);
  return subindex;

}
void NFUNC(ntSetSubtitleIndex)(JNIEnv* env,jobject thiz, jint index)
{
  LOGE("ntSetSubtitleIndex index :%d", index);
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    AndroidRuntime::Get().m_pVDCorePlayer->SetSubtitle(index);
  }
}

jint NFUNC(ntGetSubtitleStreamIndex)(JNIEnv* env,jobject thiz)
{
  int subindex = -1;
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    subindex = AndroidRuntime::Get().m_pVDCorePlayer->GetSubtitle();
  }
  LOGE("ntGetSubtitleStreamIndex subIndex :%d", subindex);
  return subindex;
}

jstring NFUNC(ntGetSubtitleStreamInfoByIndex)(JNIEnv* env,jobject thiz, jint index)
{
  std::string strSubtitleStreamInfo;

  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    VD_PlayerSubtitleStreamInfo* p = AndroidRuntime::Get().m_pVDCorePlayer->GetSubtitleStreamInfo(index);
    if(p)
    {
      std::string language;
      std::string externalSubpath;
      std::string subfilename;
      if(p->language)
      {
        language = p->language;
      }
      if(p->bExternalSub && p->filename)
      {
        externalSubpath = p->filename;
      }
      strSubtitleStreamInfo.append(language);

      if(strSubtitleStreamInfo.size() &&externalSubpath.size() )
      {
        strSubtitleStreamInfo.append("-");
      }
      if(externalSubpath.size())
      {
        std::size_t found = externalSubpath.find_last_of("/\\");
        if(found == std::string::npos)
          subfilename = externalSubpath;
        else
          subfilename = externalSubpath.substr(found+1);

        LOGE("subfilename :%s", subfilename.c_str());
      }
      strSubtitleStreamInfo.append(subfilename);

      AndroidRuntime::Get().m_pVDCorePlayer->ReleaseSubtitleStreamInfo(p);
    }

    if(strSubtitleStreamInfo.empty())
      strSubtitleStreamInfo = "Unknown";

  }
  LOGE("ntGetSubtitleStreamInfoByIndex strSubtitleStreamInfo : %s", strSubtitleStreamInfo.c_str());
  
  return env->NewStringUTF(strSubtitleStreamInfo.c_str());

}

jfloat NFUNC(ntGetSubtitleDelay)(JNIEnv* env,jobject thiz)
{
  float subDelay = 0.0;

  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    subDelay = AndroidRuntime::Get().m_pVDPlayTool->GetSubTitleDelay();
  }
  LOGE("ntGetSubtitleDelay subDelay :%f", subDelay);
  return subDelay;

}
void NFUNC(ntSetSubtitleDelay)(JNIEnv* env,jobject thiz, jfloat fValue)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    return AndroidRuntime::Get().m_pVDPlayTool->SetSubTitleDelay(fValue);
  }
}

jboolean NFUNC(ntGetSubtitleVisible)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    return AndroidRuntime::Get().m_pVDPlayTool->GetSubtitleOn();
  }
  return false;
}
void NFUNC(ntSetSubtitleVisible)(JNIEnv* env,jobject thiz, jboolean bVisible)
{

  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetSubtitleOn(bVisible);
  }
}

void NFUNC(ntSetSubtitleColor)(JNIEnv* env,jobject thiz, jint nColor)
{

  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetSubColor(nColor);
  }

} 
void NFUNC(ntSetDefaultSubtitleProperty)(JNIEnv* env,jobject thiz,jint nSubColor,jint  nSubSize,jfloat fSubPos,jint nStyle,jfloat fdelay)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetDefaultSubtitleProperty(nSubColor,nSubSize,fSubPos,nStyle,fdelay);
  }
}
void NFUNC(ntSetWhetherOverAssOrigSettings)(JNIEnv* env,jobject thiz, jboolean bOver)
{
  LOGE("ntSetWhetherOverAssOrigSettings :%d", bOver );

  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetWhetherOverAssOrigSettings(bOver);
  }
}

void NFUNC(ntSetSubtitleStyle)(JNIEnv* env,jobject thiz, jint nStyle)
{

  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetSubtitleStyle(nStyle);
  }

}
void NFUNC(ntSetSubtitleSize)(JNIEnv* env,jobject thiz, jint nSize)
{

  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetSubtitleSize(nSize);
  }

}
void NFUNC(ntSetSubtitlePos)(JNIEnv* env,jobject thiz, jint nAlign, jfloat yPos)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetSubtitlePos((VD_SubtitleAlign)nAlign, yPos);
  }
}
jint NFUNC(ntGetSubtitleColor)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
   return  AndroidRuntime::Get().m_pVDPlayTool->GetSubColor();
  }
   return -1;
}
jint NFUNC(ntGetSubtitleStyle)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    return AndroidRuntime::Get().m_pVDPlayTool->GetSubtitleStyle();
  }
   return -1;
}
jint NFUNC(ntGetSubtitleSize)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    return AndroidRuntime::Get().m_pVDPlayTool->GetSubtitleSize();
  }
  return -1;
}
jfloat NFUNC(ntGetSubtitlePos)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
   return AndroidRuntime::Get().m_pVDPlayTool->GetSubtitlePos();
  }
   return 0.0;
}
jint NFUNC(ntGetChapterCount)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->GetChapterCount();
  }
  return -1;
}

jint NFUNC(ntGetChapterIndex)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->GetChapter();
  }
  return -1;
}

jint NFUNC(ntSeekChapter)(JNIEnv* env,jobject thiz, jint index)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    AndroidRuntime::Get().m_pVDCorePlayer->SeekChapter(index);
  }
  return -1;
}

jboolean NFUNC(ntIsDisplay3DSupported)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
	return AndroidRuntime::Get().m_pVDPlayTool->IsDisplay3DSupported();
  }
  return false;
}

void NFUNC(ntSetDisplayMode)(JNIEnv* env,jobject thiz,jint mode)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    AndroidRuntime::Get().m_pVDPlayTool->SetDisplayMode((VD_DisplayMode)mode);
  }
}

jboolean NFUNC(ntIsInMenu)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer)
  {
    return AndroidRuntime::Get().m_pVDCorePlayer->IsInMenu();
  }
  return false;
}

jint NFUNC(ntGetBitrate)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer )
  {
    vidonOutInfo *info = AndroidRuntime::Get().m_pVDCorePlayer->GetVMFOutInfo();
    if ( info )
      return info->nBitrate;
  }
  return 0;
}

jint NFUNC(ntGetStarttime)(JNIEnv* env,jobject thiz)
{
  if(AndroidRuntime::Get().m_pVDCorePlayer )
  {
    vidonOutInfo *info = AndroidRuntime::Get().m_pVDCorePlayer->GetVMFOutInfo();
    if ( info )
      return info->nStartTime;
  }
  return 0;

}
jstring NFUNC(ntGetOutType)(JNIEnv* env,jobject thiz)
{
  std::string strOutType;

  if(AndroidRuntime::Get().m_pVDCorePlayer )
  {
    vidonOutInfo *info = AndroidRuntime::Get().m_pVDCorePlayer->GetVMFOutInfo();
    if ( info )
       strOutType = info->szOutType;
  }
  LOGE("ntGetOutType :%s", strOutType.c_str());

  return env->NewStringUTF(strOutType.c_str());
}

jstring NFUNC(ntGetResolution)(JNIEnv* env,jobject thiz)
{
  std::string strResolution;

  if(AndroidRuntime::Get().m_pVDCorePlayer )
  {
    vidonOutInfo *info = AndroidRuntime::Get().m_pVDCorePlayer->GetVMFOutInfo();
    if ( info )
        strResolution = info->szResolution;
  }
  LOGE("ntGetResolution :%s", strResolution.c_str());

  return env->NewStringUTF(strResolution.c_str());
}

jint NFUNC(ntGetOutQuality)(JNIEnv* env,jobject thiz)
{
  int nOutQuality = 0;

  if(AndroidRuntime::Get().m_pVDCorePlayer )
  {
    vidonOutInfo *info = AndroidRuntime::Get().m_pVDCorePlayer->GetVMFOutInfo();
    if ( info )
      nOutQuality = info->nOutQuality;
  }
  LOGE("ntGetResolution :%d", nOutQuality);

  return nOutQuality;
}

jboolean NFUNC(ntSetDecodingMethod)(JNIEnv* env,jobject thiz,jint method)
{
  if(AndroidRuntime::Get().m_pVDPlayTool)
  {
    LOGE("ntSetDecodingMethod :%d", method);
    int decoding_method = method;
    return AndroidRuntime::Get().m_pVDPlayTool->SetCSettings(VD_CSETTING_ID_VIDEOPLAYER_DECODINGMETHOD, decoding_method);
  }

  LOGE("ntSetDecodingMethod :fail");
  return false;
}

jlong NFUNC(ntReadFileData)(JNIEnv* env, jobject thiz, jstring src, jstring dst,jint startpos, jint datalen)
{
  if (src == NULL || dst == NULL)
    return -1;

  const char *srcpath = env->GetStringUTFChars(src, NULL);
  const char *dstpath = env->GetStringUTFChars(dst, NULL);
  LOGE("srcpath :%s", srcpath);
  LOGE("dstpath :%s", dstpath);
  if (srcpath == NULL || dstpath == NULL) 
    return -1;
  int64_t  nReturn = 0;
  if(AndroidRuntime::Get().m_pVDCorePlayer )
  {
   nReturn = AndroidRuntime::Get().m_pVDCorePlayer->ReadFileData(srcpath,dstpath,startpos,datalen);
  }
  env->ReleaseStringUTFChars(src, srcpath);
  env->ReleaseStringUTFChars(dst, dstpath);
  return nReturn;
}
jboolean NFUNC(ntExtractThumbForFile)(JNIEnv* env, jobject thiz, jstring src, jstring dst, jint width, jint height, jint percentTime)
{
  if (src == NULL || dst == NULL)
    return false;

  const char *srcpath = env->GetStringUTFChars(src, NULL);
  const char *dstpath = env->GetStringUTFChars(dst, NULL);

  if (srcpath == NULL || dstpath == NULL) 
    return false;

  if (percentTime <= 0 || percentTime >= 100)
  {
    percentTime = 10;
  }

  bool bOk = false; 
  if (AndroidRuntime::Get().m_pVDPlayer)
  {
    CVDMediatoolConfig* pConfig = new CVDMediatoolConfig;
    pConfig->m_strSourcePath = srcpath;
    pConfig->m_strImgePath = dstpath;
    pConfig->m_nThumbnailWidth = width;
    pConfig->m_nThumbnailHeight = height;
    pConfig->m_nThumbnailPercentTime = percentTime;
  
    IVDMediatool* pVDMediatool = AndroidRuntime::Get().m_pVDPlayer->CreateMediatool(pConfig);
    if (pVDMediatool)
      bOk = pVDMediatool->Parser();
    
    if (pConfig)
      delete pConfig;

    if (pVDMediatool)
      AndroidRuntime::Get().m_pVDPlayer->ReleaseMediatool(pVDMediatool);
  }
  env->ReleaseStringUTFChars(src, srcpath);
  env->ReleaseStringUTFChars(dst, dstpath);
  
  if (bOk)
  {
    LOGE("ntExtractThumbForFile success");
  }
  else
  {
    LOGE("ntExtractThumbForFile fail");
  }
  
  return bOk;
}

jboolean NFUNC(ntExtractThumbForPlay)(JNIEnv* env, jobject thiz, jstring dst, jint width)
{
  const char *dstpath = env->GetStringUTFChars(dst, NULL);

  if (dstpath == NULL || width <= 0)
    return false;

  int bOk = false;
  if (AndroidRuntime::Get().m_pVDPlayer && AndroidRuntime::Get().m_pVDPlayTool)
  {
    bOk = AndroidRuntime::Get().m_pVDPlayTool->CaptureRenderImage(dstpath, width);
  }

  env->ReleaseStringUTFChars(dst, dstpath);

  if (bOk)
  {
    LOGE("ntExtractThumbForPlay success");
  }
  else
  {
    LOGE("ntExtractThumbForPlay fail");
  }
  
  return bOk;
}

void NFUNC(ntExtractMeidaInfo)(JNIEnv* env, jobject thiz, jstring file, jobject info)
{
  if (file == NULL)
    return;

  const char *filepath = env->GetStringUTFChars(file, NULL);
  if (filepath == NULL) 
    return;

  MediaInfo sInfo;
  bool bOk = false;
  if (AndroidRuntime::Get().m_pVDPlayer)
  {
    CVDMediatoolConfig* pConfig = new CVDMediatoolConfig;
    pConfig->m_strSourcePath = filepath;
    pConfig->m_strThumnailPath = "";

    IVDMediatool* pVDMediatool = AndroidRuntime::Get().m_pVDPlayer->CreateMediatool(pConfig);
    if (pVDMediatool && pVDMediatool->Parser())
    {
      IVDMediaPlaylist* pVDPlaylist = pVDMediatool->GetMediaPlaylistByPlaylist();
      if (pVDPlaylist)
      {
        sInfo.filesize = pVDPlaylist->GetTotalSize();
        sInfo.duration = pVDPlaylist->GetDurationMs();
        IVDMediaInfo* pVDInfo = NULL;
       
        pVDInfo = pVDPlaylist->GetMediaInfo(VD_STREAM_TYPE_VIDEO, 0);
        if (pVDInfo)
        {
          sInfo.width = pVDInfo->GetWidth();
          sInfo.height = pVDInfo->GetHeignt();
          bOk = true;
        }
       }
    }
     
    if (pConfig)
      delete pConfig;

    if (pVDMediatool)
      AndroidRuntime::Get().m_pVDPlayer->ReleaseMediatool(pVDMediatool);
  }
  

  if (bOk)
  {
    LOGE("ntExtractMeidaInfo success filepath %s", filepath);
  }
  else
  {
    LOGE("ntExtractMeidaInfo fail filepath %s", filepath);
    env->ReleaseStringUTFChars(file, filepath);
    return;
  }
  env->ReleaseStringUTFChars(file, filepath);
  jclass iclazz = env->GetObjectClass(info);
  jfieldID fid = env->GetFieldID(iclazz, "filesize", "J");
  if (fid != NULL)
    env->SetLongField(info, fid, sInfo.filesize);

  fid = env->GetFieldID(iclazz, "duration", "J");
  if (fid != NULL)
    env->SetLongField(info, fid, sInfo.duration);

  fid = env->GetFieldID(iclazz, "videoWidth", "J");
  if (fid != NULL)
    env->SetLongField(info, fid, sInfo.width);

  fid = env->GetFieldID(iclazz, "videoHeight", "J");
  if (fid != NULL)
    env->SetLongField(info, fid, sInfo.height);

  env->DeleteLocalRef(iclazz);
}



jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
  AndroidRuntime::Get().m_jvm = vm;
  AndroidRuntime::Get().m_env = NULL;
  if (vm->GetEnv((void**) &AndroidRuntime::Get().m_env, JNI_VERSION_1_6) == JNI_OK)
  {
#ifdef VDPLAYER_FOR_KODI
    jclass cls = AndroidRuntime::Get().m_env->FindClass("org/xbmc/kodi/VidonPlayer");
#else
    jclass cls = AndroidRuntime::Get().m_env->FindClass("org/vidonme/player/VidonPlayer");
#endif
    AndroidRuntime::Get().m_clsVidonplayer = (jclass)AndroidRuntime::Get().m_env->NewGlobalRef(cls);
    //AndroidRuntime::Get().m_env->RegisterNatives(AndroidRuntime::Get().m_clsVidonplayer, gMethods, sizeof(gMethods)/sizeof(JNINativeMethod));
    AndroidRuntime::Get().m_env->DeleteLocalRef(cls);

    //download
    jclass clsdownload = AndroidRuntime::Get().m_env->FindClass("org/vidonme/player/Vmfdownload");
    AndroidRuntime::Get().m_clsVidonDownload = (jclass)AndroidRuntime::Get().m_env->NewGlobalRef(clsdownload);
    AndroidRuntime::Get().m_env->DeleteLocalRef(clsdownload);
  }
  LOGE(" load VidonPlayer.");
  return JNI_VERSION_1_6;
}

void JNI_OnUnLoad(JavaVM * vm, void * reserved)
{
  JNIEnv* env = NULL;
  if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) == JNI_OK) 
  {
    env->UnregisterNatives(AndroidRuntime::Get().m_clsVidonplayer);
    env->DeleteGlobalRef(AndroidRuntime::Get().m_clsVidonplayer);
    env->UnregisterNatives(AndroidRuntime::Get().m_clsVidonDownload);
    env->DeleteGlobalRef(AndroidRuntime::Get().m_clsVidonDownload);

  }
  LOGE(" unload VidonPlayer.");
}

//jboolean NFUNC(ntCreatDownload)(JNIEnv* env,jobject thiz, jobject jpcallback)
//{
//   bool bOk = false; 
//
//   AndroidRuntime::Get().m_pDownloadCallback = new DownloadCallback(jpcallback);
//   if (AndroidRuntime::Get().m_pDownloadCallback == NULL)
//   {
//     LOGE(" creat  DownloadCallback error.");
//     return false;
//   }
//   AndroidRuntime::Get().m_pDownload = CreateDownload(AndroidRuntime::Get().m_pDownloadCallback);
//   if (AndroidRuntime::Get().m_pDownload == NULL)
//   {
//     LOGE(" creat  m_pDownload error.");
//     return false;
//   }
//   bOk = AndroidRuntime::Get().m_pDownload->Init(&(AndroidRuntime::Get().m_sConfig.playerParam));
//   return bOk;
//}
//void NFUNC(ntReleaseDownload)(JNIEnv* env,jobject thiz)
//{
//  if (AndroidRuntime::Get().m_pDownload != NULL)
//  {
//    ReleaseDownload(AndroidRuntime::Get().m_pDownload);
//    SAFE_DELETE(AndroidRuntime::Get().m_pDownloadCallback);
//  }
//}
//jboolean NFUNC(ntDownloadStart)(JNIEnv* env,jobject thiz)
//{
//  return AndroidRuntime::Get().m_pDownload->Start();
//}
//void NFUNC(ntDownloadStop)(JNIEnv* env,jobject thiz)
//{
//  return AndroidRuntime::Get().m_pDownload->Stop();
//}
//void     NFUNC(ntDownloadPause)(JNIEnv* env,jobject thiz)
//{
//  AndroidRuntime::Get().m_pDownload->Pause();
//}
//void     NFUNC(ntDownloadResume)(JNIEnv* env,jobject thiz)
//{
//  AndroidRuntime::Get().m_pDownload->Resume();
//}
//jdouble  NFUNC(ntDownloadGetSpeed)(JNIEnv* env,jobject thiz)
//{
//  return  AndroidRuntime::Get().m_pDownload->GetSpeed();
//}
//jdouble  NFUNC(ntDownloadGetRemanenTime)(JNIEnv* env,jobject thiz)
//{
//  return  AndroidRuntime::Get().m_pDownload->GetRemanenTime();
//}
//jlong    NFUNC(ntDownloadGetFileSize)(JNIEnv* env,jobject thiz)
//{
//   int64_t nsize = 0; 
//   AndroidRuntime::Get().m_pDownload->GetFileSize(nsize);
//   return nsize;
//}
//jlong    NFUNC(ntDownloadGetProgress)(JNIEnv* env,jobject thiz)
//{
//  int64_t nDonesize = 0; 
//  AndroidRuntime::Get().m_pDownload->GetProgress(nDonesize);
//  return nDonesize;
//}
//jint     NFUNC(ntDownloadGetConvertpercent)(JNIEnv* env,jobject thiz)
//{
//  int npercent = 0; 
//  AndroidRuntime::Get().m_pDownload->GetConvertpercent(npercent);
//  return npercent;
//}
//jstring  NFUNC(ntDownloadGetExtension)(JNIEnv* env,jobject thiz)
//{
//  const char* pExtension = NULL;
//  AndroidRuntime::Get().m_pDownload->GetExtension(pExtension);
//  LOGE("ntGetOutType :%s", pExtension);
//  return env->NewStringUTF(pExtension);
//}
AndroidRuntime::AndroidRuntime():
m_jvm(NULL),
  m_env(NULL),
  m_objSurface(NULL),
  m_objVidonplayer(NULL),
  m_objDispManager(NULL),
  m_clsVidonplayer(NULL),
  m_midNativecrash(NULL),
  m_pNativewindow(NULL),
  m_pVDPlayer(NULL),
  m_pVDPlayerConfig(NULL),
  m_pVDPlayTool(NULL),
  m_pVDPlayToolConfig(NULL),
  m_pVDPlaytoolCallback(NULL),
  m_pVDCorePlayer(NULL),
  m_pVDPlayerCallback(NULL),
  m_bInitPlayerCore(false),
  m_pPlcoreCallback(NULL),
  m_pDownload(NULL),
  m_pDownloadCallback(NULL)
{
}

AndroidRuntime::~AndroidRuntime()
{
  //TODO
}

AndroidRuntime& AndroidRuntime::Get()
{
  static AndroidRuntime g_androidRuntime;
  return g_androidRuntime;
}
