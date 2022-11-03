#pragma once

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <string>
#include <FMOD/fmod.hpp>
#include "FModManager.h"
#include "SoundControl.h"

struct Vector2
{
	float x;
	float y;
};

struct Audio {
	std::string audioType;
	std::string path;
	FMOD::Sound* sound;
};

class Game
{
public:
	Game();
	~Game();
	bool Initialize();
	void Shutdown();
	void Update();
	void KeycallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
	int mRacketDirection;
	Vector2 mRacketPosition;
	Vector2 mBallPosition;
	Vector2 mBallVelocity;
	float mlastTimeFrame;
	float mdeltaTime;
	uint32_t mTicks;
	std::map<std::string, Audio> mBackgrounds;
	std::map<std::string, Audio> mSoundEffects;
	FModManager mFmodManager;
	SoundControl mSoundControl;
	GLFWwindow* mWindow;
	void Draw();
	void LoadAudioFiles(std::string filename, int mode, bool isCompressed = true);
	bool AddDspEffect();
	void SetDspState(bool &isEnabled, std::string channelName, std::string name);
};

