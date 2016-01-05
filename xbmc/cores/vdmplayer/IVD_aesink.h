#ifndef __PLCORE_AESINK_H__
#define __PLCORE_AESINK_H__

namespace libvp
{

//==========================================================================================================================//
// IVDPlayer aesink
// 
// Indexs
// A. class IVDPlayer aesink
// B. class IVDPlayer aesink config

class IVDAESinkPlcoreConfig;

//==========================================================================================================================//
// A. class IVDPlayer aesink

class IVDAESinkPlcore
{
public:

  IVDAESinkPlcore();
  virtual ~IVDAESinkPlcore();

protected:

};

//==========================================================================================================================//
// B. class IVDPlayer aesink config

class IVDAESinkPlcoreConfig
{
public:

  IVDAESinkPlcoreConfig() {}
  virtual ~IVDAESinkPlcoreConfig() {}

protected:

};

}

#endif //__PLCORE_AESINK_H__