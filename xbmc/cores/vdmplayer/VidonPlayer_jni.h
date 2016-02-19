#pragma once
#include <stdlib.h>
#include <jni.h>
#include "android/log.h"
#include "threads/SharedSection.h"
#include "threads/Thread.h"

#include "../VidonCommon.h"
#include "VidonPlayer_cfg.h"
#include "../VidonDownLoadInterface.h"

#ifndef NDEBUG
#define NDEBUG
#endif

#ifdef NDEBUG
#ifdef _DEBUG
#undef _DEBUG
#endif
#endif

#define LOG_TAG "VidonPlayer"
#ifdef _DEBUG
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#else
#define LOGE(...)
#endif

enum media_event_type
{
  MEDIA_NOP               = 0, // interface test message
  MEDIA_PREPARED          = 1,
  MEDIA_PLAYBACK_COMPLETE = 2,
  MEDIA_BUFFERING_UPDATE  = 3,
  MEDIA_SEEK_COMPLETE     = 4,
  MEDIA_SET_VIDEO_SIZE    = 5,
  MEDIA_PLAYBACK_START    = 6,
  MEDIA_PLAYBACK_PAUSED   = 7,
  MEDIA_PLAYBACK_RESUME   = 8,
  MEDIA_PLAYBACK_STOPED   = 9,
  MEDIA_UPDATE_SUBTITLE   = 10,
  MEDIA_RESIZE_VIDEO      = 11,
  MEDIA_VIDEOCODED_OMX    = 12,
  MEDIA_CAPTURE_END       = 13,
  MEDIA_TIMED_TEXT        = 99,
  MEDIA_ERROR             = 100,
  BDJ_EVENT_KEYBOARD      = 200,
};

struct SPlayerConfig
{
  // player type
  VD_PlayerType ePlayerType;

  //plcore render
  void        **handle;
  int         nWidth;
  int         nHeight;

  //plcore config 
  std::string strFontName;
  std::string strFontPath;
  std::string strLogPath;
  std::string strRuntimePath;
  std::string strProfileFolderPath;
  
  std::string strCharset;

  //plcore playback
  int bdPlayMode;
  int playlist;
  std::string localfilePath;
  //reserve
  float starttime;
  int restoreAudioIndex;
  int restoreSubtitleIndex;
  std::string restoreSubtitlePath;
  
  //aesink setting 
  bool isPassthroughOn;
  std::string audioDevice;  

  //vmf config
  vidonPlayerParam playerParam;
};

class AndroidRuntime
{
public:
  AndroidRuntime();
  ~AndroidRuntime();
  JavaVM *m_jvm;
  JNIEnv *m_env;
  jobject m_objSurface;
  jobject m_objVidonplayer;
  jobject m_objDispManager;
  jclass m_clsVidonplayer;
  jclass m_clsVidonDownload;
  jmethodID m_midNativecrash;
  struct SPlayerConfig m_sConfig;
  struct ANativeWindow* m_pNativewindow;
  vidonPlayerParam      m_DownloadParam;
  
  IVDPlayer* m_pVDPlayer;
  CVDPlcoreCallback* m_pVDPlcoreCallback;
  CVDPlayerConfig* m_pVDPlayerConfig;
  IVDPlayTool* m_pVDPlayTool;
  CVDPlayToolConfig* m_pVDPlayToolConfig;
  CVDPlaytoolCallback* m_pVDPlaytoolCallback;
  IVDCorePlayer* m_pVDCorePlayer;
  CVDPlayerCallback* m_pVDPlayerCallback;
  CVDPlcoreCallback* m_pPlcoreCallback;
  bool m_bInitPlayerCore;
  IVidonDownloadInterface  *m_pDownload;
  IDownloadCallback        *m_pDownloadCallback;
  bool                      m_bAudioPassthough;
  int                       m_nAudioPassthoughInfo;
  static AndroidRuntime& Get();
};


bool setup_listener(JNIEnv* env, jobject thiz);

#define JNIAttachCurrentThread(env) \
  bool attached = false; \
  switch (AndroidRuntime::Get().m_jvm ->GetEnv((void**)&env, JNI_VERSION_1_6)) \
{ \
  case JNI_OK: \
  break; \
  case JNI_EDETACHED: \
  if (AndroidRuntime::Get().m_jvm->AttachCurrentThread(&env, NULL)) \
{ \
  LOGE("Could not attach current thread"); \
} \
  attached = true; \
  break; \
}

#define JNIDetachCurrentThread() \
  if (attached) \
  AndroidRuntime::Get().m_jvm->DetachCurrentThread();

#define JNU_ThrowException(env, ex) \
  { \
    jclass clsIllegalState = env->FindClass("java/lang/IllegalStateException"); \
    jclass clsIO = env->FindClass("java/io/IOException"); \
    jclass clsIllegalArgument = env->FindClass("java/lang/IllegalArgumentException"); \
    jclass clsSecurity = env->FindClass("java/lang/SecurityException"); \
    \
    if(clsIllegalState && env->IsInstanceOf(ex, clsIllegalState)) \
    { \
      env->ThrowNew(clsIllegalState, "IllegalStateException throw from JNI code"); \
      LOGE( "IllegalStateException throw from JNI code..."); \
    } \
    else if(clsIO && env->IsInstanceOf(ex, clsIO)) \
    { \
      env->ThrowNew(clsIO, "IOException throw from JNI code"); \
      LOGE( "IOException throw from JNI code"); \
    } \
    else if(clsIllegalArgument && env->IsInstanceOf(ex, clsIllegalArgument)) \
    { \
      env->ThrowNew(clsIllegalArgument, "IllegalArgumentException throw from JNI code"); \
      LOGE( "IllegalArgumentException throw from JNI code"); \
    } \
    else if(clsSecurity && env->IsInstanceOf(ex, clsSecurity)) \
    { \
      env->ThrowNew(clsSecurity, "SecurityException throw from JNI code"); \
      LOGE("SecurityException throw from JNI code"); \
    } \
  }

#define  JNU_ExceptionOccurred(env) \
  { \
    jthrowable ex; \
    ex = env->ExceptionOccurred(); \
    if(ex != NULL) \
    { \
      env->ExceptionDescribe(); \
      env->ExceptionClear(); \
      JNU_ThrowException(env, ex) \
    } \
    else \
    { \
      LOGE("JNU_ExceptionOccurred nothing, from JNI code"); \
    } \
  }

