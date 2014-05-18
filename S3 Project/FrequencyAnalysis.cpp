#include "FrequencyAnalysis.h"


/**
Default Constructor initializes FMOD and stars default song playing. DO NOT USE WAVS!
*/
FrequencyAnalysis::FrequencyAnalysis(){
	SystemInitialize();
	this->sampleSize = 64;
}

//TODO: Come back to this!
bool FrequencyAnalysis::StartSong(const std::string s){
	std::string temp = (SONG_PATH + s);
	system->createStream(temp.c_str(), FMOD_SOFTWARE, 0, &song);
	system->playSound(FMOD_CHANNEL_FREE, song, false, &channel);
	song->getLength(&SongLength, FMOD_TIMEUNIT_MS);
	song->setLoopCount(0);
	return true;
}


/**
Destructor for the class, make sure no mem leaks!
*/
FrequencyAnalysis::~FrequencyAnalysis(void)
{
	//Release the song, THEN the system
	song->release();
	system->release();
}


/**
This method takes in a pointer (to a float buffer) and fills its with spectrum information.
TODO: THIS NEEDS TO TAKE IN A FLOAT BUFFER AND TWO FLOAT SPECTRUMS.
*/
float* FrequencyAnalysis::Fill(float* buffer, float* specLeft, float* specRight){
	system->update();

	boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
	

	//get dB spectrums for left and right channels
	channel->getSpectrum(specLeft, this->sampleSize, 0, FMOD_DSP_FFT_WINDOW_RECT);
	channel->getSpectrum(specRight, this->sampleSize, 1, FMOD_DSP_FFT_WINDOW_RECT);

	//Get average dBs per channel per frequency
	for(int i = 0; i <this->sampleSize; i++)
		buffer[i] = (specLeft[i] + specRight[i]) / 2;

	//Here do whatever we want with the above obtained data!
	return buffer;
}

/**
This is the required mess of function calls you have to do with fmod before using it,
for neater code a method is provided to do it for me.
*/
void FrequencyAnalysis::SystemInitialize(){
	/*
	Create a System object and initialise
	*/
	result = FMOD::System_Create(&system);
	FmodErrorCheck(result);

	result = system->getVersion(&version);
	FmodErrorCheck(result);

	if (version < FMOD_VERSION)
	{
		//std::cout << "Error! You are using an old version of FMOD " << version << ". This program requires " << FMOD_VERSION << std::endl;
		exit(-1);
	}

	//Get number of sound cards in the system
	result = system->getNumDrivers(&numDrivers);
	FmodErrorCheck(result);

	//No sound cards (disable sound)
	if (numDrivers == 0)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		FmodErrorCheck(result);
	}
	//At least one sound card
	else 
	{
		//Get the capabilities of the default (0) sound card
		result = system->getDriverCaps(0, &caps, 0, &speakerMode);
		FmodErrorCheck(result);

		//Set the speaker mode to match that in control panel
		result = system->setSpeakerMode(speakerMode);
		FmodErrorCheck(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)
		{
			/*
			The user has the 'Acceleration' slider set to off! Bad for latency!
			*/
			result = system->setDSPBufferSize(1024, 10);
			FmodErrorCheck(result);
		}

		result = system->getDriverInfo(0, name, 256, 0);
		FmodErrorCheck(result);

		if (strstr(name, "SigmaTel"))
		{
			/*
			Sigmatel sound cards crackle if format is pcm 16 bit (apparently)
			*/
			result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0,
				FMOD_DSP_RESAMPLER_LINEAR);
			FmodErrorCheck(result);
		}
	}

	result = system->init(100, FMOD_INIT_NORMAL, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		/*
		Ok, the speaker mode selected isn't supported by this soundcard. Switch it
		back to stereo...
		*/
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		FmodErrorCheck(result);

		/*
		... and re-init
		*/
		result = system->init(100, FMOD_INIT_NORMAL, 0);
	}
	FmodErrorCheck(result);
}

/**
This function is required to check that fmod initialization isnt failing at each
stage of systemInitialize.
*/
void FrequencyAnalysis::FmodErrorCheck(FMOD_RESULT result){
	if (result != FMOD_OK)
    {
        //std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
};

void FrequencyAnalysis::SetSampleSize(const int& s){
	this->sampleSize = s;
}

int FrequencyAnalysis::GetSongLength(){
	return SongLength;
}
