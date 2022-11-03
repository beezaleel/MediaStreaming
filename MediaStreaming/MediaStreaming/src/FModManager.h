#pragma once
#include <map>
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>
#include <string>

class FModManager
{
public:
	struct ChannelGroup
	{
		FMOD::ChannelGroup* groupPtr;
		float currentPan;
		float dspPitch;

		ChannelGroup() : groupPtr(nullptr), currentPan(0.0f), dspPitch(1.0f) {}
	};

protected:
	FMOD_RESULT last_result_;
	FMOD::System* system_;
	std::map<std::string, ChannelGroup*> channelGroups;
	std::map<std::string, FMOD::Sound*> sounds;
	std::map<std::string, FMOD::DSP*> dsps;

	bool is_okay(bool show_error = true);

public:
	FModManager();

	bool Initialize(const int numberOfChannels, const int systemFlags);
	void Shutdown();

	bool CreateChannelGroup(const std::string& name);
	void RemoveChannelGroup(const std::string& name);
	bool FindChannelGroup(const std::string& name, ChannelGroup** channelGroup);

	bool GetChannelGroupVolume(const std::string& name, float* volume);
	bool SetChannelGroupVolume(const std::string& name, float volume);

	bool GetChannelGroupEnabled(const std::string& name, bool* enabled);
	bool SetChannelGroupEnabled(const std::string& name, const bool enabled);

	bool SetChannelGroupParent(const std::string& childName, const std::string& parentName);

	bool CreateSound(const std::string& name, const std::string& path, const int mode);
	bool _PlaySound(const std::string& name, const std::string& channelGroup);
	void StopSound();

	bool GetChannelGroupPan(const std::string& name, float* pan);
	bool SetChannelGroupPan(const std::string& name, float pan);

	bool AddDspEffect(const std::string& channel_group_name, const std::string& effectName);
	bool RemoveDspEffect(const std::string& channel_group_name, const std::string& effectName);

	bool CreateDsp(const std::string& name, FMOD_DSP_TYPE dspType, const float value);
	bool GetDsp(const std::string& name, FMOD::DSP** dsp);
};
