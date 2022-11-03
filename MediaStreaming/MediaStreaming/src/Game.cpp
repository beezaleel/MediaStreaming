#include "Game.h"
#include <fstream>
#include <iostream>
#include <sstream>


#define HEIGHT 800
#define WIDTH 800

constexpr int MAX_CHANNELS = 255;
const std::string COMPRESSED = "compressed";

const int thickness = 15;
const float paddleH = 100.0f;

bool isDspEnabled = false;

Game::Game() :
	mWindow(nullptr),
	mSoundControl(nullptr),
	mRacketDirection(0), 
	mlastTimeFrame(0),
	mdeltaTime(0),
	mTicks(0)
{
}

Game::~Game()
{
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Game* game = (Game*)glfwGetWindowUserPointer(window);
	game->KeycallBack(window, key, scancode, action, mods);
}

void Game::KeycallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// End game
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	
	// Move up
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		mRacketDirection -= 1;
	
	// Move down
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		mRacketDirection += 1;
	
	// Play channel group 1
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		mFmodManager.StopSound();
		if (!mFmodManager._PlaySound("adventure", "channel1"))
			return;
	}
	// Play channel group 2
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		mFmodManager.StopSound();
		if (!mFmodManager._PlaySound("arcade", "channel2"))
			return;
	}
	// Play channel group 3
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		mFmodManager.StopSound();
		if (!mFmodManager._PlaySound("retro", "channel3"))
			return;
	}

	// E to enable/disable pitch dsp
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		SetDspState(isDspEnabled, "channel1", "pitch");

	// R to enable/disable fader dsp
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		SetDspState(isDspEnabled, "channel1", "fader");

	// T to enable/disable compressor dsp
	if (key == GLFW_KEY_T && action == GLFW_PRESS) 
		SetDspState(isDspEnabled, "channel1", "compressor");

	// Y to enable/disable echo dsp
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		SetDspState(isDspEnabled, "channel2", "echo");

	// U to enable/disable oscillator dsp
	if (key == GLFW_KEY_U && action == GLFW_PRESS)
		SetDspState(isDspEnabled, "channel2", "oscillator");

	// I to enable/disable tremolo dsp
	if (key == GLFW_KEY_I && action == GLFW_PRESS)
		SetDspState(isDspEnabled, "channel2", "tremolo");

	// O to enable/disable pan dsp
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
		SetDspState(isDspEnabled, "channel3", "pan"); 

	// P to enable/disable convolution dsp
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		SetDspState(isDspEnabled, "channel3", "distortion");

	// D to enable/disable parameq dsp
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		SetDspState(isDspEnabled, "channel3", "parameq");
}

/// <summary>
/// Game initialization
/// </summary>
/// <returns></returns>
bool Game::Initialize()
{
	mlastTimeFrame = glfwGetTime();
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		glfwTerminate();
		return false;
	}

	mWindow = glfwCreateWindow(HEIGHT, WIDTH, "Simple Game", nullptr, nullptr);

	if (!mWindow) {
		return false;
	}
	glfwMakeContextCurrent(mWindow);
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetKeyCallback(mWindow, key_callback);
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	glClearColor(0.1, 0.0, 0.1, 0.0);
	glfwSwapInterval(1);

	// Initialize FMOD
	if (!mFmodManager.Initialize(MAX_CHANNELS, FMOD_INIT_NORMAL)) {
		return false;
	}

	// Create 3 channels
	if (!mFmodManager.CreateChannelGroup("channel1") ||
		!mFmodManager.CreateChannelGroup("channel2") ||
		!mFmodManager.CreateChannelGroup("channel3")) {
		return false;
	}

	// Make Master the parent channel
	if (!mFmodManager.SetChannelGroupParent("channel2", "channel1") &&
		!mFmodManager.SetChannelGroupParent("channel3", "channel1")) {
		return false;
	}

	// Load background music
	LoadAudioFiles("audio.txt", FMOD_LOOP_NORMAL);

	if (!mFmodManager._PlaySound("adventure", "channel1"))
		return false;

	mSoundControl = SoundControl(&mFmodManager);

	// Add dsp effects
	if (!AddDspEffect()) {
		printf("Adding some dsf effects might have failed!\n");
	}

	mRacketPosition.x = 10.0f;
	mRacketPosition.y = 800.0f / 2.0f;
	mBallPosition.x = 800.0f / 2.0f;
	mBallPosition.y = 800.0f / 2.0f;
	mBallVelocity.x = -200.0f;
	mBallVelocity.y = 235.0f;
	return true;
}

/// <summary>
/// Shutdown glfw
/// </summary>
void Game::Shutdown()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

/// <summary>
/// Game loop
/// </summary>
void Game::Update()
{
	while (!glfwWindowShouldClose(mWindow)) {
		
		float currentTimeFrame = glfwGetTime();

		mdeltaTime = (currentTimeFrame - mlastTimeFrame) - mTicks;
		mlastTimeFrame = currentTimeFrame;
		
		if (mdeltaTime > 0.05f)
		{
			mdeltaTime = 0.05f;
		}

		mTicks = (currentTimeFrame - mlastTimeFrame);

		if (mRacketDirection != 0)
		{
			mRacketPosition.y += mRacketDirection * 300.0f * mdeltaTime;

			if (mRacketPosition.y < (paddleH / 2.0f + thickness))
			{
				mRacketPosition.y = paddleH / 2.0f + thickness;
			}
			else if (mRacketPosition.y > (768.0f - paddleH / 2.0f - thickness))
			{
				mRacketPosition.y = 768.0f - paddleH / 2.0f - thickness;
			}
		}

		mBallPosition.x += mBallVelocity.x * mdeltaTime;
		mBallPosition.y += mBallVelocity.y * mdeltaTime;


		// Bounce if needed
	// Did we intersect with the paddle?
		float diff = mRacketPosition.y - mBallPosition.y;
		// Take absolute value of difference
		diff = (diff > 0.0f) ? diff : -diff;
		if (
			// Our y-difference is small enough
			diff <= paddleH / 2.0f &&
			// We are in the correct x-position
			mBallPosition.x <= 25.0f && mBallPosition.x >= 20.0f &&
			// The ball is moving to the left
			mBallVelocity.x < 0.0f)
		{
			mBallVelocity.x *= -1.0f;
		}
		// Did the ball go off the screen? (if so, end game)
		else if (mBallPosition.x <= 0.0f)
		{
			//mIsRunning = false;
			//Shutdown();
			//return;
		}
		// Did the ball collide with the right wall?
		else if (mBallPosition.x >= (1024.0f - thickness) && mBallVelocity.x > 0.0f)
		{
			mBallVelocity.x *= -1.0f;
		}

		// Did the ball collide with the top wall?
		if (mBallPosition.y <= thickness && mBallVelocity.y < 0.0f)
		{
			mBallVelocity.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (mBallPosition.y >= (768 - thickness) &&
			mBallVelocity.y > 0.0f)
		{
			mBallVelocity.y *= -1;
		}

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, 800, 800);
		//glRectf(-0.50f, 0.50f, 0.05f, 0.40f);
		Draw();
		glfwSwapBuffers(mWindow);
	}
	Shutdown();
}

void Game::Draw() {

}

void Game::LoadAudioFiles(std::string filename, int mode, bool isCompressed)
{
	//std::ifstream infile;
	std::ifstream infile(filename);
	while (infile.is_open()) {
		std::string line, name, audioType, path, channel;

		if (!getline(infile, line))
			break;

		std::stringstream ss((line));
		ss >> name >> audioType >> path >> channel;

		if (isCompressed && audioType == COMPRESSED) {
			if (!mFmodManager.CreateSound(name, path, mode)) {
				return;
			}
		}
		else {
			if (!mFmodManager.CreateSound(name, path, mode)) {
				return;
			}
		}
		
	}
}

/// <summary>
/// Add DSP Effects
/// </summary>
/// <returns></returns>
bool Game::AddDspEffect()
{
	if (!mFmodManager.CreateDsp("echo", FMOD_DSP_TYPE_ECHO, 800.0f) ||
		!mFmodManager.CreateDsp("pitch", FMOD_DSP_TYPE_PITCHSHIFT, 0.7f) ||
		!mFmodManager.CreateDsp("fader", FMOD_DSP_TYPE_FADER, -10.0f)||
		!mFmodManager.CreateDsp("compressor", FMOD_DSP_TYPE_COMPRESSOR, -22.0f) ||
		!mFmodManager.CreateDsp("oscillator", FMOD_DSP_TYPE_OSCILLATOR, 0.0f) ||
		!mFmodManager.CreateDsp("tremolo", FMOD_DSP_TYPE_TREMOLO, 5.0f) ||
		!mFmodManager.CreateDsp("pan", FMOD_DSP_TYPE_PAN, -50.0f) ||
		!mFmodManager.CreateDsp("distortion", FMOD_DSP_TYPE_DISTORTION, 0.8f) ||
		!mFmodManager.CreateDsp("parameq", FMOD_DSP_TYPE_PARAMEQ, 10000.0f))
	{
		return false;
	}
	return true;
}

/// <summary>
/// Enable/Disable DSP
/// </summary>
/// <param name="isEnabled"></param>
/// <param name="channelName"></param>
/// <param name="name"></param>
void Game::SetDspState(bool &isEnabled, std::string channelName, std::string name) {
	if (!isEnabled) {
		mFmodManager.AddDspEffect(channelName, name);
		isEnabled = true;
	}
	else {
		mFmodManager.RemoveDspEffect(channelName, name);
		isEnabled = false;
	}
}
