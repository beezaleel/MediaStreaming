#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream>

#include "FModManager.h"
#include "SoundControl.h"

//GLFWwindow* window;
//Game game;

#define ROBOT_TEXT_COLOR 13
#define USER_TEXT_COLOR 2
#define UMPIRE_TEXT_COLOR 15
#define WRONG 4
#define RIGHT 9
#define TOTAL_ATTEMPS 6

struct Audio {
	std::string audioType;
	std::string path;
	FMOD::Sound* sound;
};

// Stores the secret and the guessed number
struct PlayersInput {
	int secretNo;
	int guessedNo;
};

// Stores the number of attempts and score of each player
struct Result {
	int totalAttempt;
	int score;
};

const std::string UMPIRE_NAME = "Umpire";

std::string player1 = "Jane";
std::string player2;

int secretNumber = 0;
unsigned int microsecond = 9000;

bool player2sTurn = false;
bool gameOver = false;
bool g_isCompressed = true;

std::map<std::string, Audio> backgrounds;
std::map<std::string, Audio> soundEffects;
FModManager fmodManager;
SoundControl soundControl = nullptr;


constexpr int MAX_CHANNELS = 255;
const std::string COMPRESSED = "compressed";

Result player1Result;
Result player2Result;

void SetDspState(bool, std::string, std::string);
PlayersInput GetInputFromComputer();
PlayersInput GetInputFromPlayer();
void Update();
void ProcessUserInput(PlayersInput, bool);
bool Initialize();
bool AddDspEffect();
void LoadAudioFiles(std::string, int, bool isCompressed = true);

/// <summary>
/// Changes the console color for each user. It helps different the users and adds little live to the game.
/// </summary>
/// <param name="color">The color number</param>
/// <param name="message">The string message</param>
void ChangeConsoleColor(int color, std::string message) {
	if (color == ROBOT_TEXT_COLOR)
		fmodManager._PlaySound("keyboard", "fx");
	else if (color == USER_TEXT_COLOR)
		fmodManager._PlaySound("ping", "fx");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	printf("%s\n", message.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), USER_TEXT_COLOR);
}

int main(int argc, char* argv[])
{
	std::string answer;
	printf("Do you want to load uncompressed version of audio files? (Y/N) ");
	std::cin >> answer;
	if (answer == "Y" || answer == "y")
		g_isCompressed = false;

	// Clear the screen
	system("cls");

	Initialize();
	srand((unsigned int)time(NULL));

	std::cout << R"(
                                                                                          
  ____  _   _  _____  ____  ____        _       _   _  _   _  __  __  ____   _____  ____  
 / ___|| | | || ____|/ ___|/ ___|      / \     | \ | || | | ||  \/  || __ ) | ____||  _ \ 
| |  _ | | | ||  _|  \___ \\___ \     / _ \    |  \| || | | || |\/| ||  _ \ |  _|  | |_) |
| |_| || |_| || |___  ___) |___) |   / ___ \   | |\  || |_| || |  | || |_) || |___ |  _ < 
 \____| \___/ |_____||____/|____/   /_/   \_\  |_| \_| \___/ |_|  |_||____/ |_____||_| \_\
                                                                                          
		)" << '\n';

	printf("\n");
	ChangeConsoleColor(ROBOT_TEXT_COLOR, "Hello! My name is Jane. Would you like to play a number guessing game with me? Please enter your name");
	printf("--> ");
	std::cin >> player2;

	ChangeConsoleColor(ROBOT_TEXT_COLOR, player1 + ": Hi " + player2 + " , we will both take turns in guessing a number from 1 - 3, with 3 attempts each.");
	ChangeConsoleColor(ROBOT_TEXT_COLOR, player1 + ": If you fail to guess the number i'm thinking, you get 0, but if you guess right, you get 1");
	ChangeConsoleColor(ROBOT_TEXT_COLOR, player1 + ": If we can guess correctly, then it's 1 point");
	ChangeConsoleColor(ROBOT_TEXT_COLOR, player1 + ": There is an Umpire that keeps record of scores. Let us begin ");
	printf("\n");
	printf("\n");

	Update();

	return 0;
}

void Update() {
	PlayersInput playersInput;

	while (!gameOver) {
		if (!player2sTurn) {
			playersInput = GetInputFromComputer();
		}
		else {
			playersInput = GetInputFromPlayer();
		}

		ProcessUserInput(playersInput, player2sTurn);
		player2sTurn = !player2sTurn;
		printf("\n");

		// Check if both players have attempted 5 times each
		if ((player1Result.totalAttempt + player2Result.totalAttempt) == TOTAL_ATTEMPS) {
			if (player1Result.score > player2Result.score) {
				ChangeConsoleColor(UMPIRE_TEXT_COLOR, UMPIRE_NAME + ": THE WINNER IS " +
					player1 + "!!! WITH A SCORE OF " + std::to_string(player1Result.score) +
					" " + player2 + " scored " + std::to_string(player2Result.score));

				fmodManager._PlaySound("failed", "fx");
				Sleep(5000); // Waiting 2 seconds before annoucing result. 
			}
			else if (player2Result.score > player1Result.score) {
				ChangeConsoleColor(UMPIRE_TEXT_COLOR, UMPIRE_NAME + ": THE WINNER IS " +
					player2 + "!!! WITH A SCORE OF " + std::to_string(player2Result.score) +
					" " + player1 + " scored " + std::to_string(player1Result.score));
				fmodManager._PlaySound("success", "fx");
				Sleep(5000); // Waiting 2 seconds before annoucing result.
			}
			else {
				ChangeConsoleColor(UMPIRE_TEXT_COLOR, UMPIRE_NAME + ": NO WINNER, " +
					player1 + " AND " + player2 + " SCORED THE SAME " + std::to_string(player1Result.score));
				fmodManager._PlaySound("nowinner", "fx");
				Sleep(5000); // Waiting 2 seconds before annoucing result.
			}

			gameOver = true;
		}
	}
}

PlayersInput GetInputFromComputer() {
	int secret, guess;
	PlayersInput input;
	// Create a random generator. Generate an int from 1 to 3
	ChangeConsoleColor(ROBOT_TEXT_COLOR, player1 + ": I'm thinking of a number...");
	fmodManager._PlaySound("clock", "fx");
	secret = rand() % 3 + 1;
	Sleep(microsecond);
	ChangeConsoleColor(ROBOT_TEXT_COLOR, player1 + ": Okay " + player2 + ", can you guess now");
	printf("--> ");
	std::cin >> guess;
	fmodManager._PlaySound("ping", "fx");

	input.secretNo = secret;
	input.guessedNo = guess;

	return input;
}

PlayersInput GetInputFromPlayer() {
	int secret, guess;
	PlayersInput input;
	ChangeConsoleColor(UMPIRE_TEXT_COLOR, UMPIRE_NAME + ": Hi " + player2 + " enter a secret number between 1 and 3. Don't worry, you can trust me.");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), USER_TEXT_COLOR);
	printf("--> ");
	std::cin >> secret;
	fmodManager._PlaySound("ping", "fx");
	ChangeConsoleColor(USER_TEXT_COLOR, player2 + ": Okay " + player1 + ", can you guess now");
	ChangeConsoleColor(ROBOT_TEXT_COLOR, player1 + ": I'm trying to guess . . . ");
	guess = rand() % 3 + 1;
	fmodManager._PlaySound("clock", "fx");
	Sleep(microsecond);
	ChangeConsoleColor(ROBOT_TEXT_COLOR, player1 + ": The number is " + std::to_string(guess));

	input.secretNo = secret;
	input.guessedNo = guess;

	return input;
}

void ProcessUserInput(PlayersInput input, bool player2sTurn) {
	printf("\n");

	Sleep(2000); // Waiting 2 seconds before annoucing result.
	if (!player2sTurn) {
		if (input.guessedNo == input.secretNo) {
			player2Result.score += 1;
			player2Result.totalAttempt += 1;
			fmodManager._PlaySound("correct", "fx");
			ChangeConsoleColor(RIGHT, UMPIRE_NAME + ": You are right " + player2 + " the number is " + std::to_string(input.secretNo));
		}
		else {
			player2Result.totalAttempt += 1;
			fmodManager._PlaySound("wrong", "fx");
			ChangeConsoleColor(WRONG, UMPIRE_NAME + ": Wrong " + player2 + " the number is " + std::to_string(input.secretNo));
		}
	}
	else {
		if (input.guessedNo == input.secretNo) {
			player1Result.score += 1;
			player1Result.totalAttempt += 1;
			fmodManager._PlaySound("correct", "fx");
			ChangeConsoleColor(RIGHT, UMPIRE_NAME + ": You are right " + player1 + " the number is " + std::to_string(input.secretNo));
		}
		else {
			fmodManager._PlaySound("wrong", "fx");
			player1Result.totalAttempt += 1;
			ChangeConsoleColor(WRONG, UMPIRE_NAME + ": Wrong " + player1 + " the number is " + std::to_string(input.secretNo));
		}
	}
	ChangeConsoleColor(UMPIRE_TEXT_COLOR, UMPIRE_NAME + ": Scores - " + player1 + ": " + std::to_string(player1Result.score) + ", " + player2 + ": " + std::to_string(player2Result.score));
}

bool Initialize() {
	// Initialize FMOD
	if (!fmodManager.Initialize(MAX_CHANNELS, FMOD_INIT_NORMAL)) {
		return false;
	}

	// Create 3 channels
	if (!fmodManager.CreateChannelGroup("channel1") ||
		!fmodManager.CreateChannelGroup("channel2") ||
		!fmodManager.CreateChannelGroup("channel3") ||
		!fmodManager.CreateChannelGroup("fx")) {
		return false;
	}

	// Make Master the parent channel
	if (!fmodManager.SetChannelGroupParent("channel2", "channel1") &&
		!fmodManager.SetChannelGroupParent("channel3", "channel1")) {
		return false;
	}

	// Load background music
	LoadAudioFiles("audio.txt", FMOD_LOOP_NORMAL, g_isCompressed);

	// Load FX sounds
	LoadAudioFiles("fx.txt", FMOD_DEFAULT, g_isCompressed);

	if (!fmodManager._PlaySound("adventure", "channel1"))
		return false;

	soundControl = SoundControl(&fmodManager);

	// Add dsp effects
	if (!AddDspEffect()) {
		printf("Adding some dsf effects might have failed!\n");
	}
}


void LoadAudioFiles(std::string filename, int mode, bool isCompressed)
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
			if (!fmodManager.CreateSound(name, path, mode)) {
				return;
			}
		}
		else {
			if (!fmodManager.CreateSound(name, path, mode)) {
				return;
			}
		}

	}
}

/// <summary>
/// Add DSP Effects
/// </summary>
/// <returns></returns>
bool AddDspEffect() {
	if (!fmodManager.CreateDsp("echo", FMOD_DSP_TYPE_ECHO, 800.0f) ||
		!fmodManager.CreateDsp("pitch", FMOD_DSP_TYPE_PITCHSHIFT, 0.7f) ||
		!fmodManager.CreateDsp("fader", FMOD_DSP_TYPE_FADER, -10.0f) ||
		!fmodManager.CreateDsp("compressor", FMOD_DSP_TYPE_COMPRESSOR, -22.0f) ||
		!fmodManager.CreateDsp("oscillator", FMOD_DSP_TYPE_OSCILLATOR, 0.0f) ||
		!fmodManager.CreateDsp("tremolo", FMOD_DSP_TYPE_TREMOLO, 5.0f) ||
		!fmodManager.CreateDsp("pan", FMOD_DSP_TYPE_PAN, -50.0f) ||
		!fmodManager.CreateDsp("distortion", FMOD_DSP_TYPE_DISTORTION, 0.8f) ||
		!fmodManager.CreateDsp("parameq", FMOD_DSP_TYPE_PARAMEQ, 10000.0f))
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
void SetDspState(bool& isEnabled, std::string channelName, std::string name) {
	if (!isEnabled) {
		fmodManager.AddDspEffect(channelName, name);
		isEnabled = true;
	}
	else {
		fmodManager.RemoveDspEffect(channelName, name);
		isEnabled = false;
	}
}

