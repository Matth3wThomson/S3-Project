#pragma once
#include "fmod.hpp"
#include "fmod_errors.h"
#include "Singleton.h"
#include <algorithm> //max_element
#include <iostream>

#include "Settings.h"
#include <Windows.h>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#define SONG_PATH "Resources\\Songs\\"

class FrequencyAnalysis : public Singleton<FrequencyAnalysis>
{
public:
	//Might not be necessary!?
	friend class Singleton<FrequencyAnalysis>;

	bool StartSong(const std::string filename);

	float* Fill(float *buffer, float* specLeft, float* specRight);

	void SetSampleSize(const int& SampleSize);

	int GetSongLength();
	int GetSampleSize(){ return this->sampleSize; }
protected:
	//Protected Constructors and destructors?
	FrequencyAnalysis();
	FrequencyAnalysis(char* songName, int sampleSize);
	~FrequencyAnalysis(void);

private:
	//FMOD
	FMOD::System *system;
	FMOD_RESULT result;
	unsigned int version;
	int numDrivers;
	FMOD_SPEAKERMODE speakerMode;
	FMOD_CAPS caps;
	char name[256];

	//Our song and channel
	FMOD::Sound *song;
	FMOD::Channel *channel;
	unsigned int SongLength;

	//Buffer information!
	float* specLeft;
	float* specRight;

	//Sample size of frequency analysis
	int sampleSize;

	void SystemInitialize();

	void FmodErrorCheck(FMOD_RESULT result);

	boost::interprocess::interprocess_mutex mutex;
};

