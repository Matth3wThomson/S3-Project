#include "Settings.h"

float Settings::JOYSTICK_TOLERANCE = 20;
float Settings::AXIS_SENSITIVITY = 0.1f;

float Settings::MAX_DPS = 10;
float Settings::MAX_HEALTH = 100;

int Settings::PLAYERS = 3;

bool Settings::THREADED = false;

int Settings::SCREEN_WIDTH = 1280;
int Settings::SCREEN_HEIGHT = 720;

bool Settings::FULLSCREEN = false;
bool Settings::VSYNC = false;

//int Settings::LoadSettings(const std::string& file){
//
//	std::string filePath = SETTINGS_PATH + file;
//
//	FILE* f = fopen(filePath.c_str(), "r");
//
//	if (f == NULL){
//		printf("Settings file not found!");
//		return false;
//	}
//
//	//Load the first line from the file
//	char lineHeader[128];
//	int res = fscanf(f, "%s", lineHeader);
//
//	//Loop through the lines
//	while (res != EOF){
//
//		//Set the according setting
//		if (strcmp(lineHeader, "samplesize") == 0 ){
//			fscanf(f, "%i\n", &SAMPLE_SIZE);
//		} else if (strcmp(lineHeader, "players") == 0){
//			fscanf(f, "%i\n", &PLAYERS);
//		} else if (strcmp(lineHeader, "threaded") == 0){
//			fscanf(f, "%i\n", &THREADED);
//		} else if (strcmp(lineHeader, "resolution") == 0){
//			fscanf(f, "%i %i\n", &SCREEN_WIDTH, &SCREEN_HEIGHT);
//		} else if (strcmp(lineHeader, "fullscreen") == 0){
//			fscanf(f, "%i\n", &FULLSCREEN);
//		} else if (strcmp(lineHeader, "song") == 0){
//			fscanf(f, "%s", &SONG_FILE);
//		} else if (strcmp(lineHeader, "vsync") == 0){
//			fscanf(f, "%i\n", &VSYNC);
//		}
//
//		//Load the next line
//		res = fscanf(f, "%s", lineHeader);
//	}
//
//	return true;
//}

int Settings::LoadSettings(const std::string& filename){

		std::ifstream f((SETTINGS_PATH + filename).c_str());

		if (!f.is_open()){
			printf("Settings file not found!");
			return false;
		}

		while (!f.eof()){
			std::string temp;
			f >> temp;

			if (temp == "players"){
				f >> PLAYERS;
			} else if (temp == "threaded"){
				f >> THREADED;
			}else if (temp == "samplesize"){
				f >> SAMPLE_SIZE;
			} else if (temp == "resolution"){
				f >> SCREEN_WIDTH;
				f >> SCREEN_HEIGHT;
			} else if (temp == "fullscreen"){
				f >> FULLSCREEN;
			} else if (temp == "vsync"){
				f >> VSYNC;
			} else if (temp == "song"){
				f >> SONG_FILE;
			}
		}

		return true;
	}