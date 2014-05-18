#pragma once

#include <string>
#include <fstream>

#define SETTINGS_PATH "Resources\\"

//512 and the bars disappear... TODO: FIX! less than 16 and disappear?


class Settings {
public:

	Settings(){ SetDefaultSettings(); }
	Settings(const std::string& file){
		SetDefaultSettings();
		LoadSettings(file);	
	};
	~Settings(){};

	int LoadSettings(const std::string& file);

	inline void SetDefaultSettings(){
		SAMPLE_SIZE = 64;
		SONG_FILE = "flumeSong.mp3";
	}

	int SAMPLE_SIZE;
	std::string SONG_FILE;

	static float JOYSTICK_TOLERANCE;
	static float AXIS_SENSITIVITY;

	static float MAX_DPS;
	static float MAX_HEALTH;

	static int PLAYERS;

	static bool THREADED;

	static int SCREEN_WIDTH;
	static int SCREEN_HEIGHT;

	static bool FULLSCREEN;
	static bool VSYNC;

};



//static void LoadSettings(const string& filename){
//	//HAVE A LOAD SETTINGS HERE?
//	//IS IT SAFE TO HAVE THESE AS NON CONST?
//}