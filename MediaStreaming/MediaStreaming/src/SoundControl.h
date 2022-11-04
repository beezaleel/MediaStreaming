#pragma once

#include "FModManager.h"
#include "imgui/imgui.h"

class SoundControl
{
public:
	SoundControl(FModManager* fmodManager);
	~SoundControl();
	void Render();
	void Details(std::string name, FModManager::ChannelGroup* channelGroup);

protected:
	FModManager* fmodManager;

};
