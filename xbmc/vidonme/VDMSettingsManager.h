
#if defined (HAS_VIDONME)

#pragma once

#include "settings/lib/ISettingCallback.h"
#include "settings/lib/ISettingsHandler.h"
#include "settings/lib/ISubSettings.h"


class CVDMSettingsManager : public ISettingCallback, public ISettingsHandler, public ISubSettings
{
public:
	static CVDMSettingsManager& Get();

	virtual void OnSettingAction(const CSetting *setting);
	virtual bool OnSettingChanging(const CSetting *setting);
	virtual bool OnSettingUpdate(CSetting* &setting, const char *oldSettingId, const TiXmlNode *oldSettingNode);

protected:
	CVDMSettingsManager();
	~CVDMSettingsManager();
};

#endif