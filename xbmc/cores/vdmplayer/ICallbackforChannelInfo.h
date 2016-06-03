#pragma once
struct ChannelInfo
{
  int nCurTime;
  int nTotalTime;
  ChannelInfo()
  {
    nCurTime = 0;
    nTotalTime = 0;
  }
};
class ICallbackforChannelInfo
{
public:
 virtual bool OnStreamConnect( ChannelInfo& channelInfo) = 0;
 virtual bool OnCallBackCacheLevel( int& nCacheLevel) = 0;
};

