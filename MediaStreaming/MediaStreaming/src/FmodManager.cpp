#include "FModManager.h"
#include <stdio.h>

FModManager::FModManager(): last_result_(FMOD_OK), system_(nullptr) {
};

bool FModManager::is_okay(bool show_error) {
	if (last_result_ == FMOD_OK) {
		return true;
	}
	if (show_error) {
		printf("fmod error %d-%s\n", last_result_, FMOD_ErrorString(last_result_));
	}
}

void FModManager::Shutdown() {
	for (auto iterator = dsps.begin(); iterator != dsps.end(); ++iterator)
	{
		iterator->second->release();
	}

	for (auto iterator = sounds.begin(); iterator != sounds.end(); ++iterator) {
		iterator->second->release();
	}
	sounds.clear();

	for (auto iterator = channelGroups.begin(); iterator != channelGroups.end(); ++iterator) {
		iterator->second->groupPtr->release();
	}
	channelGroups.clear();

	if (system_) {
		system_->release();
		system_ = nullptr;
	}
}

bool FModManager::Initialize(const int numberOfChannels, const int systemFlags) {
	last_result_ = System_Create(&system_);
	if (!is_okay()) {
		return false;
	}

	last_result_ = system_->init(numberOfChannels, systemFlags, nullptr);
	if (!is_okay()) {
		return false;
	}

	return true;
}

bool FModManager::CreateChannelGroup(const std::string& name) {
	FMOD::ChannelGroup* channelGroup;
	last_result_ = system_->createChannelGroup(name.c_str(), &channelGroup);
	if (!is_okay()) {
		return false;
	}
	auto* channelGroupWithExtraParams = new ChannelGroup();
	channelGroupWithExtraParams->groupPtr = channelGroup;

	channelGroups.emplace(name, channelGroupWithExtraParams);

	return true;
}

bool FModManager::FindChannelGroup(const std::string& name, ChannelGroup** channelGroup)
{
	const auto iterator = channelGroups.find(name);
	if (iterator == channelGroups.end())
	{
		return false;
	}

	*channelGroup = iterator->second;

	return true;
}

bool FModManager::FindSound(const std::string& name, FMOD::Sound** sound)
{
	const auto iterator = sounds.find(name);
	if (iterator == sounds.end())
	{
		return false;
	}

	*sound = iterator->second;

	return true;
}

bool FModManager::GetChannelGroupVolume(const std::string& name, float* volume)
{
	const auto iterator = channelGroups.find(name);
	if (iterator == channelGroups.end())
	{
		return false;
	}

	return is_okay(iterator->second->groupPtr->getVolume(volume));
}

bool FModManager::SetChannelGroupVolume(const std::string& name, float volume)
{
	const auto iterator = channelGroups.find(name);
	if (iterator == channelGroups.end())
	{
		return false;
	}

	return is_okay(iterator->second->groupPtr->setVolume(volume));
}

bool FModManager::GetChannelGroupEnabled(const std::string& name, bool* enabled)
{
	const auto iterator = channelGroups.find(name);
	if (iterator == channelGroups.end())
	{
		return false;
	}

	if (!is_okay(iterator->second->groupPtr->getMute(enabled))) {
		return false;
	}
	*enabled = !(*enabled);

	return true;
}

bool FModManager::SetChannelGroupEnabled(const std::string& name,const bool enabled)
{
	const auto iterator = channelGroups.find(name);
	if (iterator == channelGroups.end())
	{
		return false;
	}

	if (!is_okay(iterator->second->groupPtr->setMute(!enabled))) {
		return false;
	}

	return true;
}

void FModManager::RemoveChannelGroup(const std::string& name) {
	const auto iterator = channelGroups.find(name);
	if (iterator == channelGroups.end()) {
		return;
	}
	iterator->second->groupPtr->release();
	channelGroups.erase(iterator);
}

bool FModManager::SetChannelGroupParent(const std::string& childName, const std::string& parentName) {
	auto childGroup = channelGroups.find(childName);
	auto parentGroup = channelGroups.find(parentName);

	if (childGroup == channelGroups.end() || parentGroup == channelGroups.end()) {
		return false;
	}

	last_result_ = parentGroup->second->groupPtr->addGroup(childGroup->second->groupPtr);

	return is_okay();
}

bool FModManager::CreateSound(const std::string& name, const std::string& path, const int mode) {
	FMOD::Sound* sound;
	last_result_ = system_->createSound(path.c_str(), mode, nullptr, &sound);
	if (!is_okay()) {
		return false;
	}

	sounds.emplace(name, sound);

	return true;
}

bool FModManager::_PlaySound(const std::string& soundName, const std::string& channelGroupName) {
	const auto soundIterator = sounds.find(soundName);
	const auto channelGroupIterator = channelGroups.find(channelGroupName);

	if (soundIterator == sounds.end() || channelGroupIterator == channelGroups.end()) {
		return false;
	}

	FMOD::Channel* channel;
	last_result_ = system_->playSound(soundIterator->second, channelGroupIterator->second->groupPtr, true, &channel);
	if (!is_okay()) {
		return false;
	}

	last_result_ = (*channel).setPaused(false);
	if (!is_okay()) {
		return false;
	}

	return true;
}

void FModManager::StopSound()
{
	for (auto iterator = channelGroups.begin(); iterator != channelGroups.end(); ++iterator) {
		iterator->second->groupPtr->stop();
	}
}

bool FModManager::GetChannelGroupPan(const std::string& name, float* pan)
{
	const auto iterator = channelGroups.find(name);
	if (iterator == channelGroups.end())
	{
		return false;
	}

	*pan = iterator->second->currentPan;

	return true;
}

bool FModManager::SetChannelGroupPan(const std::string& name, float pan)
{
	const auto iterator = channelGroups.find(name);
	if (iterator == channelGroups.end())
	{
		return false;
	}

	if (!is_okay(iterator->second->groupPtr->setPan(pan)))
	{
		return false;
	}

	return true;
}

bool FModManager::AddDspEffect(const std::string& channelGroupName, const std::string& effectName)
{
	const auto channelGroupIterator = channelGroups.find(channelGroupName);
	const auto dspEffectIterator = dsps.find(effectName);
	if (channelGroupIterator == channelGroups.end() || dspEffectIterator == dsps.end())
	{
		return false;
	}

	int numDsp;
	if (!is_okay(channelGroupIterator->second->groupPtr->getNumDSPs(&numDsp)))
	{
		return false;
	}

	if (!is_okay(channelGroupIterator->second->groupPtr->addDSP(numDsp, dspEffectIterator->second)))
	{
		return false;
	}

	return true;
}

bool FModManager::RemoveDspEffect(const std::string& channelGroupName, const std::string& effectName)
{
	const auto channelGroupIterator = channelGroups.find(channelGroupName);
	const auto dspEffectIterator = dsps.find(effectName);
	if (channelGroupIterator == channelGroups.end() || dspEffectIterator == dsps.end())
	{
		return false;
	}

	if (!is_okay(channelGroupIterator->second->groupPtr->removeDSP(dspEffectIterator->second)))
	{
		return false;
	}

	return true;
}

bool FModManager::CreateDsp(const std::string& name, FMOD_DSP_TYPE dspType, const float value)
{
	FMOD::DSP* dsp;

	if (!is_okay(system_->createDSPByType(dspType, &dsp))) {
		return false;
	}

	if (!is_okay(dsp->setParameterFloat(0, value)))
	{
		return false;
	}

	dsps.try_emplace(name, dsp);
	return true;
}

bool FModManager::GetDsp(const std::string& name, FMOD::DSP** dsp)
{
	const auto dspEffectIterator = dsps.find(name);
	if (dspEffectIterator == dsps.end())
	{
		return false;
	}

	*dsp = dspEffectIterator->second;

	return true;
}


