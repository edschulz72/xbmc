#include "AESinkVidonDummy.h"
#include "utils/log.h"


CAEDeviceInfo CAESinkVidonDummy::m_info_hdmi;
CAEDeviceInfo CAESinkVidonDummy::m_info_spdif;
CAEDeviceInfo CAESinkVidonDummy::m_info_hdmi_pcm;
/*
Enumerate device info, just for UI display
*/
void CAESinkVidonDummy::EnumerateDevicesEx(AEDeviceInfoList &list, bool force)
{
    //vidon hdmi open device passthrough
    m_info_hdmi.m_channels.Reset();
    m_info_hdmi.m_dataFormats.clear();
    m_info_hdmi.m_sampleRates.clear();

    m_info_hdmi.m_deviceType = AE_DEVTYPE_PASSTHROUGH_HDMI;
    m_info_hdmi.m_deviceName = "HDMI";
    m_info_hdmi.m_displayName = "vidon";
    m_info_hdmi.m_displayNameExtra = "hdmi";
    m_info_hdmi.m_channels += AE_CH_FL;
    m_info_hdmi.m_channels += AE_CH_FR;
    m_info_hdmi.m_sampleRates.push_back(48000);
    m_info_hdmi.m_sampleRates.push_back(192000);
    m_info_hdmi.m_dataFormats.push_back(AE_FMT_S16LE);
    m_info_hdmi.m_dataFormats.push_back(AE_FMT_AC3);
    m_info_hdmi.m_dataFormats.push_back(AE_FMT_DTS);
    m_info_hdmi.m_dataFormats.push_back(AE_FMT_TRUEHD);
    m_info_hdmi.m_dataFormats.push_back(AE_FMT_DTSHD);
    m_info_hdmi.m_dataFormats.push_back(AE_FMT_EAC3);
    list.push_back(m_info_hdmi);

    //vidon spdif open device passthrough
    m_info_spdif.m_channels.Reset();
    m_info_spdif.m_dataFormats.clear();
    m_info_spdif.m_sampleRates.clear();

    m_info_spdif.m_deviceType = AE_DEVTYPE_PASSTHROUGH_IEC958;
    m_info_spdif.m_deviceName = "SPDIF";
    m_info_spdif.m_displayName = "vidon";
    m_info_spdif.m_displayNameExtra = "spdif";
    m_info_spdif.m_channels += AE_CH_FL;
    m_info_spdif.m_channels += AE_CH_FR;
    m_info_spdif.m_sampleRates.push_back(48000);
    m_info_spdif.m_dataFormats.push_back(AE_FMT_S16LE);
    m_info_spdif.m_dataFormats.push_back(AE_FMT_AC3);
    m_info_spdif.m_dataFormats.push_back(AE_FMT_DTS);
    list.push_back(m_info_spdif);

    //pcm passthorugh
    m_info_hdmi_pcm.m_channels.Reset();
    m_info_hdmi_pcm.m_dataFormats.clear();
    m_info_hdmi_pcm.m_sampleRates.clear();

    m_info_hdmi_pcm.m_deviceType = AE_DEVTYPE_PCM_HDMI;
    m_info_hdmi_pcm.m_deviceName = "HDMI-PCM";
    m_info_hdmi_pcm.m_displayName = "vidon";
    m_info_hdmi_pcm.m_displayNameExtra = "hdmipcm";
    m_info_hdmi_pcm.m_channels += AE_CH_FL;
    m_info_hdmi_pcm.m_channels += AE_CH_FR;
    m_info_hdmi_pcm.m_sampleRates.push_back(48000);
    m_info_hdmi_pcm.m_dataFormats.push_back(AE_FMT_S32LE);
    list.push_back(m_info_hdmi_pcm);
  //end
}
