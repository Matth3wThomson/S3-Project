#pragma once
#include "Singleton.h"
#include <SFML/Window.hpp>

class InputHandler : public Singleton<InputHandler>
{
	friend class Singleton<InputHandler>;

public:

	bool HandleInput(sf::Event& event);

	inline int GetNoOfJoysticks(){
		return joysticks.size();
	}

	inline int GetJoystick(const int& i){
		if (joysticks.size() > i){
			return joysticks[i];
		} else {
			return -1;
		}
	}

	inline int GetPlayerOne(){
		return joysticks.front();
	}

	inline int GetPlayerTwo(){
		return joysticks[1];
	}

protected:
	std::vector<int> joysticks;

	void parseControllers(){
		joysticks.clear();
		
		for (int i=0; i<8; ++i){
			if (sf::Joystick::isConnected(i)){
				joysticks.push_back(i);
			}
		}
	}

	InputHandler(void){
		parseControllers();
	};
	~InputHandler(void){

	};
};

