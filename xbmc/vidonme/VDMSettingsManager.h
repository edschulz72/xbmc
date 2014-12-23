
#if defined (HAS_VIDONME)

#pragma once

#include "settings/lib/ISettingCallback.h"
#include "settings/lib/ISettingsHandler.h"


class CVDMSettingsManager : public ISettingCallback, public ISettingsHandler
{
public:
	static CVDMSettingsManager& Get();

	virtual void OnSettingAction(const CSetting *setting);
	virtual bool OnSettingChanging(const CSetting *setting);

protected:
	CVDMSettingsManager();
	~CVDMSettingsManager();
};

#endif