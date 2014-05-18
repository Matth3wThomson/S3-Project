#include "InputHandler.h"



bool InputHandler::HandleInput(sf::Event& event){
	switch (event.type){
	case sf::Event::JoystickConnected:
		parseControllers();
		break;
	case sf::Event::JoystickDisconnected:
		parseControllers();
		break;
	}
	return true;
}