
#ifndef __IVD_PLAYTOOL_DECLS_H__
#define __IVD_PLAYTOOL_DECLS_H__

#if defined(HAS_DX)
#include "D3D9.h"
#include "D3DX9.h"
#endif

namespace libvp
{


//==========================================================================================================================//
// 
// Indexs
// ** SetCSettings's id define
// ** enum
// ** struct
// ** callback define
// ** callback interface
// ** interface

//==========================================================================================================================//
// ** SetCSettings's id define
// sub Indexs
// ** audio output
// ** video acceleration
// ** network
// ** video player
// ** VideoScreen
// ** subtitles
// ** dvds
// ** disc

//==========================================================================================================================//
// ** audio output
//
// *  audio passthrough settings guide
//    You have to select one passthrough device and enable passthrough before you enable a specific passthrough.
//    e.g. 
//    Select one passthrough device by set:  VD_CSETTING_ID_AUDIOOUTPUT_PASSTHROUGHDEVICE
//    Enable passthrough by call:            VD_CSETTING_ID_AUDIOOUTPUT_PASSTHROUGH
//    Enable a specific passthrough by call: VD_CSETTING_ID_AUDIOOUTPUT_AC3PASSTHROUGH 
//
// *  You have to call NotifyAudioOutputSettingsChanged method to let those changed audio output settings work.

//Select the device to be used for playback of encoded formats
//value type is char*
#define VD_CSETTING_ID_AUDIOOUTPUT_PASSTHROUGHDEVICE "audiooutput.passthroughdevice"

//Select the device to be used for playback of audio that has been decoded such as mp3
//value type is char*
#define VD_CSETTING_ID_AUDIOOUTPUT_AUDIODEVICE "audiooutput.audiodevice"

//Default is false.
//Set true to enable upmixing of 2 channel audio to the number of audio channels specified by the channel configuration
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_STEREOUPMIX "audiooutput.stereoupmix"

//Default is true.
//Select how audio is downmixed, for example from 5.1 to 2.0: 
//[true] maintains volume level of the original audio source however the dynamic range is compressed. 
//[false] maintains the dynamic range of the original audio source when downmixed however volume will be lower. 
//Note - Dynamic range is the difference between the quietest and loudest sounds in an audio source. Set true if movie dialogues are barely audible
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_MAINTAINORIGINALVOLUME "audiooutput.maintainoriginalvolume"

//Default is VD_AE_QUALITY_MID.
//Select the quality of resampling for cases where the audio output needs to be at a different sampling rate from that used by the source. 
//[VD_AE_QUALITY_LOW] is fast and will have minimal impact on system resources such as the use of the CPU, 
//[VD_AE_QUALITY_MID] & [VD_AE_QUALITY_HIGH] will use progressively more system resources.
//value type is int
#define VD_CSETTING_ID_AUDIOOUTPUT_PROCESSQUALITY "audiooutput.processquality"

//Default is false.
//Select to enable the passthrough audio options for playback of encoded audio such as Dolby Digitals.
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_PASSTHROUGH "audiooutput.passthrough"

//Default is VD_AUDIO_CHANNEL_LAYOUT_2_0.
//Select the number of channels supported by the audio connection, or the number of speakers if connected by analog connections. 
//This setting does not apply to passthrough audio. Note - SPDIF supports 2.0 channels only but can still output multichannel audio using a format supported by passthrough
//value type is int
#define VD_CSETTING_ID_AUDIOOUTPUT_CHANNELS "audiooutput.channels"

//Default is VD_AUDIO_OUTPUT_CONFIG_TYPE_AUTO.
//Select how the properties of the audio output are set.
//value type is int
#define VD_CSETTING_ID_AUDIOOUTPUT_CONFIG "audiooutput.config"

//Default is 48000.
//Maximum sampling rate for spdif or sampling rate for fixed output configuration.
//[44100,48000,88200,96000,192000]
//value type is int
#define VD_CSETTING_ID_AUDIOOUTPUT_SAMPLERATE "audiooutput.samplerate"

//Default is true.
//Set it true to use AC3 passthrough if your receiver is capable of decoding AC3 streams.
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_AC3PASSTHROUGH "audiooutput.ac3passthrough"

//Default is false.
//Set it true if the audio out connection only supports multichannel audio as Dolby Digital 5.1, such as an SPDIF connection.  
//If the system supports LPCM multichannel sound via HDMI, leave this false.
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_AC3TRANSCODE "audiooutput.ac3transcode"

//Default is false.
//Set it true to use E-AC3 passthrough if your receiver is capable of decoding E-AC3 streams.
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_EAC3PASSTHROUGH "audiooutput.eac3passthrough"

//Default is false.
//Set it true to use DTS passthrough if your receiver is capable of decoding DTS streams.
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_DTSPASSTHROUGH "audiooutput.dtspassthrough"

//Default is false.
//Set it true to use TRUE-HD passthrough if your receiver is capable of decoding TRUE-HD streams.
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_TRUEHDPASSTHROUGH "audiooutput.truehdpassthrough"

//Default is false.
//Set it true to use DTS-HD passthrough if your receiver is capable of decoding DTS-HD streams.
//value type is bool
#define VD_CSETTING_ID_AUDIOOUTPUT_DTSHDPASSTHROUGH "audiooutput.dtshdpassthrough"


//==========================================================================================================================//
// ** video acceleration
// Usually those settings would not work immediately while playing back. TODO: to make immediately

enum VD_RENDER_METHODS
{
	VD_RENDER_METHOD_AUTO = 0,
	VD_RENDER_METHOD_ARB,
	VD_RENDER_METHOD_GLSL,
	VD_RENDER_METHOD_SOFTWARE,
	VD_RENDER_METHOD_D3D_PS,
	VD_RENDER_METHOD_DXVA,
	VD_RENDER_METHOD_DXVAHD,
	VD_RENDER_OVERLAYS = 99   // to retain compatibility
};
//default is VD_RENDER_METHOD_AUTO
//Adjust the method used to process and display video.
//This change does not work immediately while palying back.
#define VD_CSETTING_ID_VIDEOPLAYER_RENDERMETHOD "videoplayer.rendermethod"

//Default is 0.
//Use high quality scalers when upscaling a video by at least this percentage.
//[0,100] step 10
//This change does not work immediately while palying back.
//value type is int
#define VD_CSETTING_ID_VIDEOPLAYER_HQSCALERS "videoplayer.hqscalers"

//Default is true.
//Enable decoding of video files using pixel buffer objects.
//This change does not work immediately while palying back.
//value type is bool
#define VD_CSETTING_ID_VIDEOPLAYER_USEPBO "videoplayer.usepbo"

//Default is false.
//Enable upscaling using VDPAU
//value type is bool
#define VD_CSETTING_ID_VIDEOPLAYER_VDPAUUPSCALINGLEVEL "videoplayer.vdpauUpscalingLevel"

//Default is false.
//VDPAU studio level conversion provides a way for advanced applications like Kodi to influence the colour space conversion
//value type is bool
#define VD_CSETTING_ID_VIDEOPLAYER_VDPAU_ALLOW_XRANDR "videoplayer.vdpau_allow_xrandr"

enum VD_DECODING_METHOD
{
	VD_DECODING_METHOD_SOFTWARE = 0,
	VD_DECODING_METHOD_HARDWARE = 1,
};
//Default is VD_DECODING_METHOD_HARDWARE.
//Defines whether video decoding should be performed in software (requires more CPU) or with hardware acceleration where possible
#define VD_CSETTING_ID_VIDEOPLAYER_DECODINGMETHOD "videoplayer.decodingmethod"

//Default is true.
//Enable hardware video decode using AMLogic decoder
//value type is bool
#define VD_CSETTING_ID_VIDEOPLAYER_USEAMCODEC "videoplayer.useamcodec"

//Default is true.
//Enable VDPAU hardware decoding of video files, mainly used for NVIDIA graphics and in some circumstances AMD graphics
//value type is bool
#define VD_CSETTING_ID_VIDEOPLAYER_USEVDPAU "videoplayer.usevdpau"

//Default is true.
//Bypassing VDPAU mixer saves resources on low power systems but slightly reduces picture quality
//value type is bool
#define VD_CSETTING_ID_VIDEOPLAYER_USEVDPAUMIXER "videoplayer.usevdpaumixer"

//Default is true.
//Enable this option to use hardware acceleration for MPEG-(1/2) codecs. 
//If disabled the CPU will be used instead. Older Radeon Cards tend to segfault with this enabled
#define VD_CSETTING_ID_VIDEOPLAYER_USEVDPAUMPEG2 "videoplayer.usevdpaumpeg2"

//Default is flase.
//Enable this option to use hardware acceleration for the MPEG-4 codec. 
//If disabled the CPU will be used instead. Some ION Hardware has problems with this being enabled by default
#define VD_CSETTING_ID_VIDEOPLAYER_USEVDPAUMPEG4 "videoplayer.usevdpaumpeg4"

//Default is true.
//Enable this option to use hardware acceleration for VC-1 based codecs.
//If disabled the CPU will be used instead. AMD Hardware with VDPAU cannot decode VC-1 Simple
#define VD_CSETTING_ID_VIDEOPLAYER_USEVDPAUVC1 "videoplayer.usevdpauvc1"

//Default is true.
//Enable VAAPI hardware decoding of video files, mainly used for Intel graphics and in some circumstances AMD graphics
#define VD_CSETTING_ID_VIDEOPLAYER_USEVAAPI "videoplayer.usevaapi"

//Default is true.
//Enable this option to use hardware acceleration for MPEG-(1/2) codecs.
//If disabled the CPU will be used instead. Some MPEG-2 Videos might have green artifacts
#define VD_CSETTING_ID_VIDEOPLAYER_USEVAAPIMPEG2 "videoplayer.usevaapimpeg2"

//Default is true.
//Enable this option to use hardware acceleration for the MPEG-4 codec. If disabled the CPU will be used instead
#define VD_CSETTING_ID_VIDEOPLAYER_USEVAAPIMPEG4 "videoplayer.usevaapimpeg4"

//Default is true.
//Enable this option to use hardware acceleration for VC-1 based codecs. 
//If disabled the CPU will be used instead. Especially VC-1 Interlaced fails hard on Intel hardware
#define VD_CSETTING_ID_VIDEOPLAYER_USEVAAPIVC1 "videoplayer.usevaapivc1"

//Default is true.
//If enabled VAAPI render method is prefered. This puts less load on the CPU but driver may hang!
#define VD_CSETTING_ID_VIDEOPLAYER_PREFERVAAPIRENDER "videoplayer.prefervaapirender"

// Default is true
// Enable DXVA2 hardware decoding of video files
#define VD_CSETTING_ID_VIDEOPLAYER_USEDXVA2 "videoplayer.usedxva2"

//Default is true
//Use OMXPlayer for decoding of video files
#define VD_CSETTING_ID_VIDEOPLAYER_USEOMXPLAYER "videoplayer.useomxplayer"

//Default is true
//Enable OpenMax hardware decoding of video files
#define VD_CSETTING_ID_VIDEOPLAYER_USEOMX "videoplayer.useomx"

//Default is true
//Enable VideoToolbox hardware decoding of video files
#define VD_CSETTING_ID_VIDEOPLAYER_USEVIDEOTOOLBOX "videoplayer.usevideotoolbox"

//Default is true
//Enable VDA hardware decoding of video files
#define VD_CSETTING_ID_VIDEOPLAYER_USEVDA "videoplayer.usevda"

//Default is true
//Enable hardware decoding of video files
#define VD_CSETTING_ID_VIDEOPLAYER_USEMEDIACODEC "videoplayer.usemediacodec"

//Default is true
#define VD_CSETTING_ID_VIDEOPLAYER_USESTAGEFRIGHT "videoplayer.usestagefright"

//Default is true
//Use DVDPlayer for decoding of video files with MMAL acceleration
#define VD_CSETTING_ID_VIDEOPLAYER_USEMMAL "videoplayer.usemmal"



//==========================================================================================================================//
// ** network

//Default is 0
//[0,102400] step 512
//If you have limited bandwidth available, plcore will try to keep within these limits
#define VD_CSETTING_ID_NETWORK_BANDWIDTH "network.bandwidth"

//Default is false
//If your internet connection uses a proxy, configure it here
#define VD_CSETTING_ID_NETWORK_USEHTTPPROXY "network.usehttpproxy"

//Configure the proxy server address
#define VD_CSETTING_ID_NETWORK_HTTPPROXYSERVER "network.httpproxyserver"

//Default is 8080
//[1,65535] step 1
//Configure the proxy server port
#define VD_CSETTING_ID_NETWORK_HTTPPROXYPORT "network.httpproxyport"

//Default is PLCORE_HTTP_PROXY_TYPE_HTTP
//Configure which proxy type is used
#define VD_CSETTING_ID_NETWORK_HTTPPROXYTYPE "network.httpproxytype"

//Configure the proxy server username
#define VD_CSETTING_ID_NETWORK_HTTPPROXYUSERNAME "network.httpproxyusername"
#define VD_CSETTING_ID_NETWORK_HTTPPROXYPASSWORD "network.httpproxypassword"



//==========================================================================================================================//
// ** video player

//Select the default audio track when different language tracks are available
//This is similar to SetPreferredSubtitleLanguage
//value type is char*
#define VD_CSETTING_ID_LOCALE_AUDIOLANGUAGE "locale.audiolanguage"


enum VD_ADJUST_REFRESH_RATE
{
	VD_ADJUST_REFRESHRATE_OFF = 0,
	VD_ADJUST_REFRESHRATE_ALWAYS,
	VD_ADJUST_REFRESHRATE_ON_STARTSTOP
};
//Default is VD_ADJUST_REFRESHRATE_OFF
//Allow the refresh rate of the display to be changed so that it best matches the video frame rate. This may yield smoother video playback
//This change would work immediately while playing back.
#define VD_CSETTING_ID_VIDEOPLAYER_ADJUSTREFRESHRATE "videoplayer.adjustrefreshrate"

//Default is false
//Synchronise the video to the refresh rate of the monitor
#define VD_CSETTING_ID_VIDEOPLAYER_USEDISPLAYASCLOCK "videoplayer.usedisplayasclock"

//Default is 5.0
//[0.0,10.0] step 0.1
//Maximum video speed adjust to match actual screen refresh rate
#define VD_CSETTING_ID_VIDEOPLAYER_MAXSPEEDADJUST "videoplayer.maxspeedadjust"

//Default is 0
//[0,20] step 1
//Allow video player to ignoring aspect ratio by a certain amount to fill a larger amount of the screen with video
//This change would work immediately while playing back.
#define VD_CSETTING_ID_VIDEOPLAYER_ERRORINASPECT "videoplayer.errorinaspect"

//Default is VD_VIEW_MODE_NORMAL
//Select the zoom level that 4:3 videos are shown on widescreen displays
//Only can be VD_VIEW_MODE_NORMAL or VD_VIEW_MODE_ZOOM or VD_VIEW_MODE_WIDEZOOM or VD_VIEW_MODE_STRETCH16x9
#define VD_CSETTING_ID_VIDEOPLAYER_STRETCH43 "videoplayer.stretch43"

//Default is true
//Enable Teletext when watching a live TV stream
#define VD_CSETTING_ID_VIDEOPLAYER_TELETEXTENABLED "videoplayer.teletextenabled"

//Default is 0
//time base is 100 ms
//Pause for a small amount of time during a refresh rate change
#define VD_CSETTING_ID_VIDEOPLAYER_PAUSEAFTERREFRESHCHANGE "videoplayer.pauseafterrefreshchange"



//==========================================================================================================================//
// ** VideoScreen

//Default is false
//Use limited colour range (16-235) instead of full colour range (0-255). 
//Limited range should be used if your display is a regular HDMI TV and doesn't have a PC or other mode to display full range colour, 
//however if your display is a PC monitor then leave this disabled to get proper blacks
#define VD_CSETTING_ID_VIDEOSCREEN_LIMITEDRANGE "videoscreen.limitedrange"

enum VD_VSYNC
{
	VD_VSYNC_DISABLED = 0,
	VD_VSYNC_VIDEO = 1,
	VD_VSYNC_ALWAYS = 2,
	VD_VSYNC_DRIVER = 3
};
//Default is VD_VSYNC_ALWAYS
//Eliminate vertical tearing
#define VD_CSETTING_ID_VIDEOSCREEN_VSYNC "videoscreen.vsync"

//Default is false
//In a multi-screen configuration, the screens where player is not displayed are blacked out
#define VD_CSETTING_ID_VIDEOSCREEN_BLANKDISPLAYS "videoscreen.blankdisplays"

//For now only KODI use
#define VD_CSETTING_ID_VIDEOSCREEN_SCREEN "videoscreen.screen"

//default is "DESKTOP"
//Can be "DESKTOP","WINDOW"
//value type is char*
#define VD_CSETTING_ID_VIDEOSCREEN_SCREENMODE "videoscreen.screenmode"

//For now only KODI use
#define VD_CSETTING_ID_VIDEOSCREEN_MONITOR "videoscreen.monitor"

//==========================================================================================================================//
// ** subtitles

//To let playercore know what the CharacterSet that current external subtitle file used is.  Default is CP1252.
//Playercore will convert any other coding to UTF-8. 
//Some coding can be detected by playercore from sub file. If can not, then will use this user specified CharSet.
//CharSet name format is same as iconv used. e.g. "ISO-8859-1", "GBK", "SHIFT_JIS"... Studing iconv usage at internet to know more.
//value type is char*
#define VD_CSETTING_ID_SUBTITLES_CHARSET "subtitles.charset"

//When open a media file, plcore will search external subtitle files in current media file directory.
//And it will also search them in custom path if client called this method.
//value type is char*
#define VD_CSETTING_ID_SUBTITLES_CUSTOMPATH "subtitles.custompath"

// When open a media file, plcore will choose a subtitle as default.
// This method will help plcore predicate which sub should be default.
// Default language is 'original' that means no preferred one.
// value format refers to the content of KODI's UI setting control 'Preferred subtitle language'.
//
// The predicate algorithm as follows
//          A subtitle lh is 'better than' a subtitle rh (in evaluation order) if
//          - lh was previously selected, or
//          - lh is an external sub and rh not, or
//          - lh is a forced sub and ("original stream's language" was selected or subtitles are off) and rh not, or
//          - lh is an external sub and its language matches the preferred subtitle's language (unequal to "original stream's language") and rh not, or
//          - lh is language matches the preferred subtitle's language (unequal to "original stream's language") and rh not, or
//          - lh is a default sub and rh not
//value type is char*
#define VD_CSETTING_ID_LOCALE_SUBTITLELANGUAGE "locale.subtitlelanguage"

//Default is VD_SUBTITLE_STYLE_BOLD
#define VD_CSETTING_ID_SUBTITLES_STYLE "subtitles.style"

//Default is false
#define VD_CSETTING_ID_SUBTITLES_OVERRIDEASSFONTS "subtitles.overrideassfonts"


//Default is 0
//[0,10] step 1
//Sets the visual depth of subtitles for stereoscopic 3D videos. The higher the value, the closer the subtitles will appear to the viewer
#define VD_CSETTING_ID_SUBTITLES_STEREOSCOPICDEPTH "subtitles.stereoscopicdepth"

//Default is false
//Enable to parse for CC in video stream. Puts slightly more load on the CPU
#define VD_CSETTING_ID_SUBTITLES_PARSECAPTIONS "subtitles.parsecaptions"


//==========================================================================================================================//
// ** dvds

//Default is 0
//[0,8] step 1
//Force a region for DVD playback
#define VD_CSETTING_ID_PLAYERREGION "playerregion"


//Default is false
//Attempt to skip 'unskippable' introductions before DVD menu
#define VD_CSETTING_ID_DVDS_AUTOMENU "dvds.automenu"

//==========================================================================================================================//
// ** disc

//Default is VD_BD_PLAY_MODE_SHOW_SIMPLIFIED_MENU
//Specifies how Blu-rays should be opened/played back. Disc menus are not fully supported yet and may cause problems
#define VD_CSETTING_ID_DISC_PLAYBACK "disc.playback"

// ** end of SetCSettings's id define




//==========================================================================================================================//
// ** enum

enum VD_RotationDegree
{
	VD_ROTATION_DEGREE_ZERO,
	VD_ROTATION_DEGREE_NINETY,
	VD_ROTATION_DEGREE_ONE_HUNDRED_EIGHTY,
	VD_ROTATION_DEGREE_TWO_HUNDRED_SEVENTY,
};

enum VD_MirrorStyle
{
	VD_MIRROR_STYLE_NORMAL,
	VD_MIRROR_STYLE_HORIZONTAL,
	VD_MIRROR_STYLE_VERTICAL,
	VD_MIRROR_STYLE_HORIZONTAL_AND_VERTICAL,
};

enum VD_RenderFeature
{
	VD_RENDER_FEATURE_GAMMA,
	VD_RENDER_FEATURE_BRIGHTNESS,
	VD_RENDER_FEATURE_CONTRAST,
	VD_RENDER_FEATURE_NOISE,
	VD_RENDER_FEATURE_SHARPNESS,
	VD_RENDER_FEATURE_NONLINSTRETCH,
	VD_RENDER_FEATURE_ROTATION,
	VD_RENDER_FEATURE_STRETCH,
	VD_RENDER_FEATURE_CROP,
	VD_RENDER_FEATURE_ZOOM,
	VD_RENDER_FEATURE_VERTICAL_SHIFT,
	VD_RENDER_FEATURE_PIXEL_RATIO,
	VD_RENDER_FEATURE_POSTPROCESS
};

enum VD_DeinterlaceMode
{
	VD_DEINTERLACE_MODE_OFF = 0,
	VD_DEINTERLACE_MODE_AUTO = 1,
	VD_DEINTERLACE_MODE_FORCE = 2
};

enum VD_InterlaceMethod 
{
	VD_INTERLACE_METHOD_NONE = 0, // Legacy
	VD_INTERLACE_METHOD_NONE_AUTO = 1,
	VD_INTERLACE_METHOD_NONE_RENDER_BLEND = 2,

	VD_INTERLACE_METHOD_NONE_RENDER_WEAVE_INVERTED = 3,
	VD_INTERLACE_METHOD_NONE_RENDER_WEAVE = 4,

	VD_INTERLACE_METHOD_NONE_RENDER_BOB_INVERTED = 5,
	VD_INTERLACE_METHOD_NONE_RENDER_BOB = 6,

	VD_INTERLACE_METHOD_NONE_DEINTERLACE = 7,

	VD_INTERLACE_METHOD_NONE_VDPAU_BOB = 8,
	VD_INTERLACE_METHOD_NONE_INVERSE_TELECINE = 9,

	VD_INTERLACE_METHOD_NONE_VDPAU_INVERSE_TELECINE = 11,
	VD_INTERLACE_METHOD_NONE_VDPAU_TEMPORAL = 12,
	VD_INTERLACE_METHOD_NONE_VDPAU_TEMPORAL_HALF = 13,
	VD_INTERLACE_METHOD_NONE_VDPAU_TEMPORAL_SPATIAL = 14,
	VD_INTERLACE_METHOD_NONE_VDPAU_TEMPORAL_SPATIAL_HALF = 15,
	VD_INTERLACE_METHOD_NONE_DEINTERLACE_HALF = 16,
	VD_INTERLACE_METHOD_NONE_DXVA_BOB = 17,
	VD_INTERLACE_METHOD_NONE_DXVA_BEST = 18,
	// VS_INTERLACEMETHOD_DXVA_ANY = 19, Legacy

	VD_INTERLACE_METHOD_NONE_SW_BLEND = 20,
	VD_INTERLACE_METHOD_NONE_AUTO_ION = 21,

	VD_INTERLACE_METHOD_NONE_VAAPI_BOB = 22,
	VD_INTERLACE_METHOD_NONE_VAAPI_MADI = 23,
	VD_INTERLACE_METHOD_NONE_VAAPI_MACI = 24,

	VD_INTERLACE_METHOD_NONE_MMAL_ADVANCED = 25,
	VD_INTERLACE_METHOD_NONE_MMAL_ADVANCED_HALF = 26,
	VD_INTERLACE_METHOD_NONE_MMAL_BOB = 27,
	VD_INTERLACE_METHOD_NONE_MMAL_BOB_HALF = 28,

	VD_INTERLACE_METHOD_NONE_MAX // do not use and keep as last enum value.
};

enum VD_ScalingMethod 
{
	VD_SCALING_METHOD_NEAREST = 0,
	VD_SCALING_METHOD_LINEAR,

	VD_SCALING_METHOD_CUBIC,
	VD_SCALING_METHOD_LANCZOS2,
	VD_SCALING_METHOD_LANCZOS3_FAST,
	VD_SCALING_METHOD_LANCZOS3,
	VD_SCALING_METHOD_SINC8,
	VD_SCALING_METHOD_NEDI,

	VD_SCALING_METHOD_BICUBIC_SOFTWARE,
	VD_SCALING_METHOD_LANCZOS_SOFTWARE,
	VD_SCALING_METHOD_SINC_SOFTWARE,
	VD_SCALING_METHOD_VDPAU_HARDWARE,
	VD_SCALING_METHOD_DXVA_HARDWARE,

	VD_SCALING_METHOD_AUTO,

	VD_SCALING_METHOD_SPLINE36_FAST,
	VD_SCALING_METHOD_SPLINE36,

	VD_SCALING_METHOD_MAX // do not use and keep as last enum value.
};

enum VD_ViewMode 
{
	VD_VIEW_MODE_NORMAL = 0,
	VD_VIEW_MODE_ZOOM,
	VD_VIEW_MODE_STRETCH4x3,
	VD_VIEW_MODE_WIDEZOOM,
	VD_VIEW_MODE_STRETCH16x9,
	VD_VIEW_MODE_ORIGINAL,
	VD_VIEW_MODE_CUSTOM
} ;


enum VD_SubtitleStyle 
{
	VD_SUBTITLE_STYLE_NORMAL,
	VD_SUBTITLE_STYLE_BOLD,
	VD_SUBTITLE_STYLE_ITALICS,
	VD_SUBTITLE_STYLE_BOLD_ITALICS,
};

enum VD_SubtitleAlign 
{
	VD_SUBTITLE_ALIGN_MANUAL = 0,
	VD_SUBTITLE_ALIGN_BOTTOM_INSIDE,
	VD_SUBTITLE_ALIGN_BOTTOM_OUTSIDE,
	VD_SUBTITLE_ALIGN_TOP_INSIDE,
	VD_SUBTITLE_ALIGN_TOP_OUTSIDE,
};

enum VD_AudioChannelLayout 
{
	VD_AUDIO_CHANNEL_LAYOUT_INVALID = -1,

	VD_AUDIO_CHANNEL_LAYOUT_1_0 = 0,
	VD_AUDIO_CHANNEL_LAYOUT_2_0,
	VD_AUDIO_CHANNEL_LAYOUT_2_1,
	VD_AUDIO_CHANNEL_LAYOUT_3_0,
	VD_AUDIO_CHANNEL_LAYOUT_3_1,
	VD_AUDIO_CHANNEL_LAYOUT_4_0,
	VD_AUDIO_CHANNEL_LAYOUT_4_1,
	VD_AUDIO_CHANNEL_LAYOUT_5_0,
	VD_AUDIO_CHANNEL_LAYOUT_5_1,
	VD_AUDIO_CHANNEL_LAYOUT_7_0,
	VD_AUDIO_CHANNEL_LAYOUT_7_1,

	VD_AUDIO_CHANNEL_LAYOUT_MAX
};

enum VD_AudioOutputConfigType 
{
	VD_AUDIO_OUTPUT_CONFIG_TYPE_FIXED = 1,//output properties are set to the specified sampling rate & speaker configuration at all times
	VD_AUDIO_OUTPUT_CONFIG_TYPE_AUTO = 2,//output properties are set at the start of playback and will not change if the properties of the source changes
	VD_AUDIO_OUTPUT_CONFIG_TYPE_MATCH = 3,//output properties are set to always be as close a match to the source properties as possible
};

enum VD_BDPlayMode 
{
	VD_BD_PLAY_MODE_SHOW_SIMPLIFIED_MENU = 0,
	VD_BD_PLAY_MODE_SHOW_DISC_MENU = 1,
	VD_BD_PLAY_MODE_PLAY_MAIN_MOVIE = 2,
};

enum VD_AudioQuality 
{
	VD_AE_QUALITY_UNKNOWN = -1, // Unset, unknown or incorrect quality level
	VD_AE_QUALITY_DEFAULT = 0, // Engine's default quality level 

	// Basic quality levels 
	VD_AE_QUALITY_LOW = 20, // Low quality level
	VD_AE_QUALITY_MID = 30, // Standard quality level 
	VD_AE_QUALITY_HIGH = 50, // Best sound processing quality 

	// Optional quality levels
	VD_AE_QUALITY_REALLYHIGH = 100, // Uncompromised optional quality level,
															       //usually with unmeasurable and unnoticeable improvement 
	VD_AE_QUALITY_GPU = 101, // GPU acceleration
};


enum VD_StereoscopicMode 
{
	VD_STEREOSCOPIC_MODE_AUTO = 0,
	VD_STEREOSCOPIC_MODE_OVER_UNDER = 1,
	VD_STEREOSCOPIC_MODE_SIDE_BY_SIDE = 2,
};


enum VD_ShaderMethod 
{
	VD_SHADER_METHOD_DEFAULT,
	VD_SHADER_METHOD_TEXTURE,
	VD_SHADER_METHOD_MULTI,
	VD_SHADER_METHOD_FONTS,
	VD_SHADER_METHOD_TEXTURE_NOBLEND,
	VD_SHADER_METHOD_MULTI_BLENDCOLOR,
	VD_SHADER_METHOD_TEXTURE_RGBA,
	VD_SHADER_METHOD_TEXTURE_RGBA_OES,
	VD_SHADER_METHOD_TEXTURE_RGBA_BLENDCOLOR,
	VD_SHADER_METHOD_ESHADERCOUNT
};

enum VD_RenderStereoMode 
{
	VD_RENDER_STEREO_MODE_OFF,
	VD_RENDER_STEREO_MODE_SPLIT_HORIZONTAL,
	VD_RENDER_STEREO_MODE_SPLIT_VERTICAL,
	VD_RENDER_STEREO_MODE_ANAGLYPH_RED_CYAN,
	VD_RENDER_STEREO_MODE_ANAGLYPH_GREEN_MAGENTA,
	VD_RENDER_STEREO_MODE_INTERLACED,
	VD_RENDER_STEREO_MODE_HARDWAREBASED,
	VD_RENDER_STEREO_MODE_MONO,
	VD_RENDER_STEREO_MODE_COUNT,

	// psuevdo modes
	VD_RENDER_STEREO_MODE_AUTO = 100,
};

enum VD_RenderStereoView 
{
	VD_RENDER_STEREO_VIEW_OFF,
	VD_RENDER_STEREO_VIEW_LEFT,
	VD_RENDER_STEREO_VIEW_RIGHT,
};

enum VD_Resolution  
{
	VD_RESOLUTION_INVALID = -1,
	VD_RESOLUTION_HDTV_1080i = 0,
	VD_RESOLUTION_HDTV_720pSBS = 1,
	VD_RESOLUTION_HDTV_720pTB = 2,
	VD_RESOLUTION_HDTV_1080pSBS = 3,
	VD_RESOLUTION_HDTV_1080pTB = 4,
	VD_RESOLUTION_HDTV_720p = 5,
	VD_RESOLUTION_HDTV_480p_4x3 = 6,
	VD_RESOLUTION_HDTV_480p_16x9 = 7,
	VD_RESOLUTION_NTSC_4x3 = 8,
	VD_RESOLUTION_NTSC_16x9 = 9,
	VD_RESOLUTION_PAL_4x3 = 10,
	VD_RESOLUTION_PAL_16x9 = 11,
	VD_RESOLUTION_PAL60_4x3 = 12,
	VD_RESOLUTION_PAL60_16x9 = 13,
	VD_RESOLUTION_AUTORES = 14,
	VD_RESOLUTION_WINDOW = 15,
	VD_RESOLUTION_DESKTOP = 16,          // Desktop resolution for primary screen
	VD_RESOLUTION_CUSTOM = 16 + 1,      // Desktop resolution for screen #2
	//                     ...
	//                     12 + N - 1   // Desktop resolution for screen N
	//                     12 + N       // First additional resolution, in a N screen configuration.
	//                     12 + N + ... // Other resolutions, in any order
};

enum VD_DisplayMode
{
    VD_DISPLAY_MODE_2D_ORIGINAL = 0,
    VD_DISPLAY_MODE_2D_LEFT_HALF = 1,
    VD_DISPLAY_MODE_2D_UPPER_HALF = 2,
    VD_DISPLAY_MODE_3D_LEFT_RIGHT = 3,
    VD_DISPLAY_MODE_3D_TOM_BOTTOM = 4,
    VD_DISPLAY_MODE_3D_INTERLACING = 5,
    VD_DISPLAY_MODE_3D_LINE_INTERLACING = 6,
    VD_DISPLAY_MODE_3D_COLUMN_INTERLACING = 7,
    VD_DISPLAY_MODE_MAX
};


//==========================================================================================================================//
// ** struct

struct VD_AudioOutputDeviceNode
{
	char* m_strDisplayName;
	char* m_strName; 
	VD_AudioOutputDeviceNode* next;

	VD_AudioOutputDeviceNode()
	{
		m_strDisplayName = NULL;
		m_strName = NULL;
		next = NULL;
	}
	~VD_AudioOutputDeviceNode()
	{
		delete[] m_strDisplayName;
		delete[] m_strName;
		delete next;
	}
};

struct VD_ThreadMessageCallback
{
	void(*callback)(void *userptr);
	void *userptr;
};

//==========================================================================================================================//
// ** callback interface


class IVDPlaytoolCallback
{
public:
	IVDPlaytoolCallback() {};
	virtual ~IVDPlaytoolCallback() {};

#ifdef VDPLAYER_FOR_KODI
	// Inside this, 
	//-----------------------------------------------------------------
	// kodi client has to add following code
	//
	// ThreadMessageCallback callbackData;
	// callbackData.callback = cb.callback;
	// callbackData.userptr = cb.userptr;

	// ThreadMessage msg;
	// msg.dwMessage = TMSG_CALLBACK;
	// msg.lpVoid = (void*)&callbackData;

	// wait for it.
	// CApplicationMessenger::Get().SendMessage(msg, bWaitBeforeDone);
	virtual void OnThreadMessageCallback(VD_ThreadMessageCallback& cb, bool bWaitBeforeDone ) = 0;


	// Inside this, 
	//-----------------------------------------------------------------
	// kodi client has to add following code
	//
	// ThreadMessage msg = {TMSG_RENDERER_FLUSH};
	// CApplicationMessenger::Get().SendMessage(msg, false);
	virtual void OnRenderFlush() = 0;

	//To deal with "stack playback"'s seek
	//Kodi client should call g_application.SeekTime() inside this.
	virtual void OnStackSeek(double time) = 0;

	//To get "stack playback"'s time
	//Kodi client should call g_application.GetTime() inside this.
	virtual double OnGetStackTime() = 0;

	// Inside this Kodi client should call "CApplicationMessenger::Get().SendAction(CAction(ACTION_TRIGGER_OSD), WINDOW_INVALID, false);".
	virtual void OnActionTriggerOsd() = 0;

	//Kodi client should call CApplicationMessenger::Get().SwitchToFullscreen() inside this method.
	virtual void OnSwitchToFullscreen() = 0;

	//Kodi client should call g_Windowing.SetViewPort() inside this method.
	virtual void OnSetViewPort(float x1, float y1, float x2, float y2) = 0;
	
	//Kodi client should call g_graphicsContext.SetVideoResolution() in its render thread.
	virtual void OnSetVideoResolution(VD_Resolution res, bool forceUpdate) = 0;

	//Kodi client should call g_graphicsContext.SetFullScreenVideo() in its render thread.
	virtual void OnSetFullScreenVideo(bool bFull) = 0;
	

#if defined(HAS_GLES)
	//Kodi client should call g_Windowing.EnableGUIShader() inside this method.
	virtual void OnEnableGUIShader(VD_ShaderMethod method) = 0;
	//Kodi client should call g_Windowing.DisableGUIShader() inside this method.
	virtual void OnDisableGUIShader() = 0;
	//Kodi client should call g_Windowing.GUIShaderGetPos() inside this method.
	virtual int OnGUIShaderGetPos() = 0;
	//Kodi client should call g_Windowing.GUIShaderGetCol() inside this method.
	virtual int OnGUIShaderGetCol() = 0;
	//Kodi client should call g_Windowing.GUIShaderGetCoord0() inside this method.
	virtual int OnGUIShaderGetCoord0() = 0;
	//Kodi client should call g_Windowing.GUIShaderGetUniCol() inside this method.
	virtual int OnGUIShaderGetUniCol() = 0;
	//Kodi client should call g_Windowing.GUIShaderGetCoord1() inside this method.
	virtual int OnGUIShaderGetCoord1()= 0;
	//Kodi client should call g_Windowing.GUIShaderGetCoord0Matrix() inside this method.
	virtual int OnGUIShaderGetCoord0Matrix()= 0;

#endif
#endif //VDPLAYER_FOR_KODI
};

//==========================================================================================================================//
// ** interface
#if defined(HAS_EGL)
#include <EGL/egl.h>
#endif


class IVDPlayToolConfig
{
public:

	IVDPlayToolConfig() {}
	virtual ~IVDPlayToolConfig() {}

	virtual int GetRenderWindowInitWidth() const = 0;

	virtual int GetRenderWindowInitHeight() const = 0;
#if !defined(VDPLAYER_FOR_KODI)
	// Return true means default resolution is desktop
	virtual bool GetIsDesktop() { return false; };

	// e.g return &m_hwnd;
	// m_hwnd's type cound be HWND or NSView* or NativeWindow* or UIView*
	virtual void** GetRenderWindowHandle() const = 0;


#else 

#if defined(HAS_DX)

	virtual LPDIRECT3D9 GetD3d9() const = 0;

	virtual LPDIRECT3DDEVICE9 GetD3d9Device() const = 0;

	virtual const D3DADAPTER_IDENTIFIER9& GetAIdentifier() const = 0;

	virtual DWORD GetDefaultD3DUsage() const = 0;

	virtual D3DPOOL GetDefaultD3DPool() const = 0;

	virtual D3DPRESENT_PARAMETERS GetD3DPP() const = 0;
#endif

	virtual unsigned int GetRenderCaps() const = 0;

	virtual unsigned int GetMaxTextureSize() const = 0;

	virtual int GetScreen() const = 0;

#if defined(HAS_GL)

	virtual unsigned int GetRenderQuirks() const = 0;
#endif


	virtual const char* GetRenderVendor() const = 0;

	virtual const char* GetRenderRenderer() const = 0;

	virtual void GetRenderVersion(unsigned int& major, unsigned int& minor) const = 0;

#if defined(TARGET_DARWIN_OSX)

	virtual void* GetGlContext() const = 0;
#endif

#if defined(HAS_GL) || defined(HAS_GLES)

	virtual const char* GetRenderExtensions() const = 0;
#endif

#if defined(HAS_EGL)

	virtual EGLDisplay GetEGLDisplay() = 0;

	virtual EGLContext GetEGLContext() = 0;
#endif

#endif // VDPLAYER_FOR_KODI
	
protected:

};

}

#endif //__IVD_PLAYTOOL_DECLS_H__
