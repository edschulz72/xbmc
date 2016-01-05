#ifndef __PLCORE_ADDON_H__
#define __PLCORE_ADDON_H__


#include "VAStdLib.h"
#include "VAAddonUtils.h"

namespace libvp
{

//==========================================================================================================================//
// IVDPlayer addon
// 
// Indexs
// A. class IVDPlayer addontool
// B. class IVDPlayer addontool config

class IVDAddontoolConfig;

//==========================================================================================================================//
// A. class IVDPlayer addontool

class IVDAddontool
{
public:
	// VAStdLib
	virtual VABool VABool_Create(const bool value = false) = 0;
	virtual void VABool_Destroy(VABool& var) = 0;
	virtual VAInt VAInt_Create(const int value = 0) = 0;
	virtual void VAInt_Destroy(VAInt& var) = 0;
	virtual VADouble VADouble_Create(const double value = 0.0) = 0;
	virtual void VADouble_Destroy(VADouble& var) = 0;
	virtual VAString VAString_Create(const char* value = "") = 0;
	virtual void VAString_Destroy(VAString& var) = 0;
	virtual VAStringW VAStringW_Create(const wchar_t* value = L"") = 0;
	virtual void VAStringW_Destroy(VAStringW& var) = 0;
	virtual VAStringVector VAStringVector_Create(void) = 0;
	virtual void VAStringVector_Destroy(VAStringVector& var) = 0;
	virtual VAIntToStr VAIntToStr_Create(void) = 0;
	virtual void VAIntToStr_Destroy(VAIntToStr& var) = 0;
	virtual VAIntToStrVector VAIntToStrVector_Create(void) = 0;
	virtual void VAIntToStrVector_Destroy(VAIntToStrVector& var) = 0;
	virtual VAStrToStr VAStrToStr_Create(void) = 0;
	virtual void VAStrToStr_Destroy(VAStrToStr& var) = 0;
	virtual VAStrToStrVector VAStrToStrVector_Create(void) = 0;
	virtual void VAStrToStrVector_Destroy(VAStrToStrVector& var) = 0;

	// VAAddonUtils struct
	virtual VAAddonDetails VAAddonDetails_Create(void) = 0;
	virtual void VAAddonDetails_Destroy(VAAddonDetails& var) = 0;
	virtual VAFileItem VAFileItem_Create(void) = 0;
	virtual void VAFileItem_Destroy(VAFileItem& var) = 0;
	virtual VAFileItemList VAFileItemList_Create(void) = 0;
	virtual void VAFileItemList_Destroy(VAFileItemList& var) = 0;
	virtual VAAddonUtils VAAddonUtils_Create(void) = 0;
	virtual void VAAddonUtils_Destroy(VAAddonUtils& var) = 0;

	// VALegacyFactory
	virtual VALegacyFactory VAGetLegacyFactory(void) = 0;
	virtual void VASetLegacyFactory(VALegacyFactory factory) = 0;
};

//==========================================================================================================================//
// B. class IVDPlayer addontool config

class IVDAddontoolConfig
{
public:
	// nothing adding for this;

};


class IAddontoolCallback 
{
public:
	// nothing adding for this;

};

}

#endif //__PLCORE_ADDON_H__