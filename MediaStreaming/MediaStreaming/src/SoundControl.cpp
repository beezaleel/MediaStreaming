#include "SoundControl.h"

SoundControl::SoundControl(FModManager* fmodManager): fmodManager(fmodManager)
{
}

SoundControl::~SoundControl()
{
}

void SoundControl::Render() {
	ImGui::Begin("Audio Settings");

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel1", &channelGroup))
		{
			return;
		}

		float currentVolume;
		if (!fmodManager->GetChannelGroupVolume("channel1", &currentVolume)) {
			return;
		}
		currentVolume *= 100;
		ImGui::SliderFloat("channel1 volume (Press `1` to enable)", &currentVolume, 0.0f, 100.0f, "%.0f");
		currentVolume /= 100;

		if (!fmodManager->SetChannelGroupVolume("channel1", currentVolume)) {
			return;
		}

		bool volumeEnabled;
		if (!fmodManager->GetChannelGroupEnabled("channel1", &volumeEnabled)) {
			return;
		}

		ImGui::SameLine();
		ImGui::Checkbox("##channel1_volume", &volumeEnabled);

		if (!fmodManager->SetChannelGroupEnabled("channel1", volumeEnabled)) {
			return;
		}
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel2", &channelGroup))
		{
			return;
		}

		float currentVolume;
		if (!fmodManager->GetChannelGroupVolume("channel2", &currentVolume)) {
			return;
		}
		currentVolume *= 100;
		ImGui::SliderFloat("channel2 volume (Press `2` to enable)", &currentVolume, 0.0f, 100.0f, "%.0f");
		currentVolume /= 100;

		if (!fmodManager->SetChannelGroupVolume("channel2", currentVolume)) {
			return;
		}

		bool volumeEnabled;
		if (!fmodManager->GetChannelGroupEnabled("channel2", &volumeEnabled)) {
			return;
		}

		ImGui::SameLine();
		ImGui::Checkbox("##channel2_volume", &volumeEnabled);

		if (!fmodManager->SetChannelGroupEnabled("channel2", volumeEnabled)) {
			return;
		}
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel3", &channelGroup))
		{
			return;
		}

		float currentVolume;
		if (!fmodManager->GetChannelGroupVolume("channel3", &currentVolume)) {
			return;
		}
		currentVolume *= 100;
		ImGui::SliderFloat("channel3 volume (Press `3` to enable)", &currentVolume, 0.0f, 100.0f, "%.0f");
		currentVolume /= 100;

		if (!fmodManager->SetChannelGroupVolume("channel3", currentVolume)) {
			return;
		}

		bool volumeEnabled;
		if (!fmodManager->GetChannelGroupEnabled("channel3", &volumeEnabled)) {
			return;
		}

		ImGui::SameLine();
		ImGui::Checkbox("##channel3_volume", &volumeEnabled);

		if (!fmodManager->SetChannelGroupEnabled("channel3", volumeEnabled)) {
			return;
		}
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("fx", &channelGroup))
			return;

		float currentVolume;
		if (!fmodManager->GetChannelGroupVolume("fx", &currentVolume))
			return;

		currentVolume *= 100;
		ImGui::SliderFloat("fx volume", &currentVolume, 0.0f, 100.0f, "%.0f");
		currentVolume /= 100;

		if (!fmodManager->SetChannelGroupVolume("fx", currentVolume)) 
			return;

		bool volumeEnabled;
		if (!fmodManager->GetChannelGroupEnabled("fx", &volumeEnabled))
			return;

		ImGui::SameLine();
		ImGui::Checkbox("##fx_volume", &volumeEnabled);

		if (!fmodManager->SetChannelGroupEnabled("fx", volumeEnabled))
			return;
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel1", &channelGroup))
			return;

		ImGui::SliderFloat("channel1 pitch (Press `E` to enable/disable)", &channelGroup->dspPitch, 0.5f, 2.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("pitch", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspPitch);
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel1", &channelGroup))
			return;

		ImGui::SliderFloat("channel1 fader (Press `R` to enable/disable)", &channelGroup->dspFader, -0.80f, 10.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("fader", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspFader);
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel1", &channelGroup))
			return;

		ImGui::SliderFloat("channel1 compressor (Press `T` to enable/disable)", &channelGroup->dspCompressor, 1.00f, 50.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("compressor", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspCompressor);
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel2", &channelGroup))
			return;

		ImGui::SliderFloat("channel2 echo (Press `Y` to enable/disable)", &channelGroup->dspEcho, 10.00f, 5000.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("echo", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspEcho);
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel2", &channelGroup))
			return;

		ImGui::SliderFloat("channel2 oscillator (Press `U` to enable/disable)", &channelGroup->dspOscillator, 0.0f, 5.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("oscillator", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspOscillator);
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel2", &channelGroup))
			return;

		ImGui::SliderFloat("channel2 tremolo (Press `I` to enable/disable)", &channelGroup->dspTremolo, 0.1f, 20.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("tremolo", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspTremolo);
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel3", &channelGroup))
			return;

		ImGui::SliderFloat("channel3 pan (Press `O` to enable/disable)", &channelGroup->dspPan, -1.0f, 1.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("pan", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspPan);
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel3", &channelGroup))
			return;

		ImGui::SliderFloat("channel3 distortion (Press `P` to enable/disable)", &channelGroup->dspDistortion, 0.0f, 1.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("distortion", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspDistortion);
	}

	{
		FModManager::ChannelGroup* channelGroup;
		if (!fmodManager->FindChannelGroup("channel3", &channelGroup))
			return;

		ImGui::SliderFloat("channel3 parameq (Press `D` to enable/disable)", &channelGroup->dspParameq, 20.0f, 22000.0f, "%.2f");
		FMOD::DSP* dsp;
		if (!fmodManager->GetDsp("parameq", &dsp))
			return;
		dsp->setParameterFloat(0, channelGroup->dspParameq);
	}

	ImGui::End();
}
