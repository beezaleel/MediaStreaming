#pragma once

#include "FModManager.h"
#include "imgui/imgui.h"

class SoundControl
{
public:
	SoundControl(FModManager* fmodManager);
	~SoundControl();
	void Render();

protected:
	FModManager* fmodManager;

};
