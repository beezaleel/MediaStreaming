#include "SoundControl.h"

SoundControl::SoundControl(FModManager* fmodManager): fmodManager(fmodManager)
{
}

SoundControl::~SoundControl()
{
}

void SoundControl::Render() {
	ImGui::Begin("Audio Settings");

	FModManager::ChannelGroup* channelGroup;
	if (!fmodManager->FindChannelGroup("master", &channelGroup)) {

	}

	float currentVolume;
	if (!fmodManager->GetChannelGroupVolume("master", &currentVolume)) {

	}
	currentVolume *= 100;
	ImGui::SliderFloat("master volume", &currentVolume, 0.0f, 100.0f, "%.0f");
	currentVolume /= 100;

	if (!fmodManager->SetChannelGroupVolume("master", currentVolume)) {

	}

	bool isMuted;
	if (!fmodManager->GetChannelGroupEnabled("master", &isMuted)) {

	}
	ImGui::SameLine();
	ImGui::Checkbox("###master_volume", &isMuted);
	if (!fmodManager->SetChannelGroupEnabled("master", isMuted)) {

	}

	ImGui::End();
}