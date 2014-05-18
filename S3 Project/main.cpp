#pragma once



//Graphical
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

//Threading and safety
#include <boost/thread.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

//Stl containers
#include <list>

//Other classes
#include "Shader.h"
#include "Mesh.h"
#include "RenderObject.h"
#include "SRenderer.h"
#include "FrequencyAnalysis.h"
#include "FreqCircle.h"
#include "Settings.h"
#include "HealthHUD.h"
#include "InputHandler.h"

using std::list;

const Vector2 lowerBound(-8.0f, -5.0f);
const Vector2 upperBound(8.0f, 5.0f);

vector<long> performance;

//Loop flags
bool quit = false;
bool threadQuit = false;

//list of circles in game
list<FreqCircle*> circles;
vector<HealthHUD*> healthBars;

//Circle list protection
interprocess_upgradable_mutex circleMutex;

void threadProcedure(FreqCircle* fq, HealthHUD* hud, SRenderer& r, SRenderer& ortho, int threadNumber){

	sf::Clock c;
	bool dead = false;

	while (!threadQuit && !dead){
		float time = c.restart().asSeconds();

		//Update the bars of the object
		fq->UpdateBars();

		//Move the object around in response to input scheme
		fq->GameUpdate(time);

		//Check the shape has not been moved out of bounds
		fq->CheckAndResolveBounds(lowerBound, upperBound);

		//Protect this list from dual access, as Check and Resolve acquires two locks
		circleMutex.lock_upgradable();
		for (list<FreqCircle*>::const_iterator i = circles.begin(); i != circles.end(); ++i){
			//Dont compare a shape with itself!
			if (*i == fq){
				continue;
			}
			fq->CheckAndResolveCollision(*i, time);
		}

		//Update the healthbar that corresponds with this object
		healthBars[threadNumber-1]->Update(*fq);
		circleMutex.unlock_upgradable();

		//Performance recording (striped vector)
		performance[threadNumber] += 1;

		//Update the graphics of the object and its health bar
		fq->Update(time);

		//If the shape has been destroyed remove it from the game, and stop this thread
		if (fq->GetHealth() < 0.0f){
			dead = true;
			circleMutex.lock();
			circles.remove(fq);
			circleMutex.unlock();
			break;
		}
	}
}


int main( int argc, char* args[] )
{
	Settings settings("settings.txt");

	sf::RenderWindow window;

	//Create our window
	if (Settings::FULLSCREEN){
		window.create(sf::VideoMode(Settings::SCREEN_WIDTH, Settings::SCREEN_HEIGHT), "Freakquency", sf::Style::Fullscreen);
	} else {
		window.create(sf::VideoMode(Settings::SCREEN_WIDTH, Settings::SCREEN_HEIGHT), "Freakquency");
	}

	if (Settings::VSYNC){
		window.setVerticalSyncEnabled(true);
	}

	//Intialize our Renderers (Ortho and Perspective)
	SRenderer r((float) Settings::SCREEN_WIDTH, (float) Settings::SCREEN_HEIGHT, false);
	SRenderer ortho((float) Settings::SCREEN_WIDTH, (float) Settings::SCREEN_HEIGHT, true);

	//Initialize glew
	if (GLEW_OK != glewInit()){
		printf("Error initializing glew!");
		return -1;
	}

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	//Compile our shader
	Shader *s = new Shader("testVert.glsl", "testFrag.glsl");

	//Timing Variables
	sf::Clock c;
	sf::Clock totalTime;
	performance.push_back(0);

	//Start the FMOD Wrapper before threads start (otherwise they create their own individual
	//wrappers)
	FrequencyAnalysis::Create();
	FrequencyAnalysis::Instance().StartSong(settings.SONG_FILE);
	FrequencyAnalysis::Instance().SetSampleSize(settings.SAMPLE_SIZE);

	InputHandler::Create();

	//Create Game objects depending on number of players
	for (int i=0; i<Settings::PLAYERS; ++i){

		FreqCircle* fq;
		
		//Organise players equidistant apart from the center
		if (Settings::PLAYERS != 1){
			fq = new FreqCircle(FrequencyAnalysis::Instance().GetSampleSize(), s,
			Vector3((float) (10/(Settings::PLAYERS-1) * (i) - 5),0,0),
			1.0f,
			i);
		} else {
			fq = new FreqCircle(FrequencyAnalysis::Instance().GetSampleSize(), s,
			Vector3(0,0,0),
			1.0f,
			i);
		}

		//Add circles to the list of circles
		circles.push_back(fq);

		//Add health hud to the list of health huds
		HealthHUD* health = new HealthHUD(s,
			Vector2( (float) (Settings::SCREEN_WIDTH / Settings::PLAYERS * i + (Settings::SCREEN_WIDTH / Settings::PLAYERS) * 0.5), 20),
			Vector2( (float) (Settings::SCREEN_WIDTH / 10), (float) (Settings::SCREEN_HEIGHT /20) ));
		healthBars.push_back(health);

		//If we are running threaded set the threads off!
		if (Settings::THREADED){
			performance.push_back(0);
			boost::thread* threadOne = new boost::thread(threadProcedure, fq, health, r, ortho, i+1);
		}
	}

	//Polgon mode flag
	bool polygon = false;

	//While application is running
	while( !quit )
	{
		//Handle events on queue
		sf::Event event;
		while (window.pollEvent(event))
		{
			window.setActive();
			switch (event.type){
			case sf::Event::Closed:
				threadQuit = true;
				quit = true;
				break;
			case sf::Event::Resized:
				glViewport(0,0,event.size.width, event.size.height);
				break;
			case sf::Event::KeyPressed:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)){
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4)){
						threadQuit = true;
						quit = true;
					}
				}
				break;
			}
		}

		//Debugging Mode
		if (polygon){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float time;

		if (Settings::THREADED){
			time = c.restart().asSeconds();
			performance[0] += 1;

			circleMutex.lock_sharable();
			//Then render all of the circles
			for (list<FreqCircle*>::const_iterator i = circles.begin(); i != circles.end(); ++i){
				if (*i != NULL){
					(*i)->Render(r);
				}
			}

			//Render all of the health bars
			for (vector<HealthHUD*>::const_iterator i = healthBars.begin(); i != healthBars.end(); ++i){
				if (*i != NULL){
					(*i)->Render(ortho);
				}
			}

			//Swap buffers
			ortho.LockRendering();
			r.LockRendering();
			window.display();	//2Phase locking done here.
			r.UnlockRendering();
			ortho.UnlockRendering();
			circleMutex.unlock_sharable();
		} else {
			//Performance counting
			performance[0] += 1;

			time = c.restart().asSeconds();

			//Have each object run its engine counterpart
			for (list<FreqCircle*>::const_iterator i = circles.begin(); i != circles.end(); ++i){

				(*i)->UpdateBars();
				(*i)->GameUpdate(time);
				(*i)->CheckAndResolveBounds(lowerBound, upperBound);
				
				//Bias present here
				for (list<FreqCircle*>::const_iterator j = circles.begin(); j != circles.end(); ++j){
					if ((*i) == (*j)) continue; //Dont compare yourself
					(*i)->CheckAndResolveCollision(*j, time);
				}
			}

			list<FreqCircle*>::iterator i = circles.begin();
			while (i != circles.end()){

				if ( (*i)->GetHealth() < 0){
					circles.erase(i++);
				} else {
					++i;
				}
			}

			//Render the resulting object
			int j = 0;
			for (list<FreqCircle*>::const_iterator i = circles.begin(); i != circles.end(); ++i){
				(*i)->Update(time);
				healthBars[j]->Update(**i);
				(*i)->Render(r);
				healthBars[j]->Render(ortho);
				j++;
			}

			
			window.display();
		}

		if (totalTime.getElapsedTime().asMilliseconds() > FrequencyAnalysis::Instance().GetSongLength()){
			threadQuit = true;
			quit = true;
		}

		//PERFORMANCE TESTING CODE
		/*if (totalTime.getElapsedTime().asSeconds() > 60){
			threadQuit = true;
			quit = true;
		}*/ 

		if (circles.size() == 1){
			threadQuit = true;
			quit = true;
		}
		
	}

	float time = totalTime.restart().asSeconds();

	if (circles.size() == 1){
		std::cout << "The winner was player: " << (*circles.begin())->GetPlayer()+1 << std::endl;
	} else {
		std::cout << "GAME OVER!\n";

		int count = 0;
		int x = 0;
		float maxHealth = 0.0f;

		for (list<FreqCircle*>::iterator i = circles.begin(); i != circles.end(); ++i){
			if ((*i)->GetHealth() > maxHealth){
				maxHealth = (*i)->GetHealth();
				x = count;
			}
			count++;
		}

		std::cout << "The winner was player " << x+1 << std::endl;
	}

	std::cout << "Total time: " << time << std::endl;
	for (unsigned int i = 0; i < performance.size(); ++i){
		if (i < 1) std::cout << "Main thread loops: ";
		else std::cout << "Worker thread loops: ";
		std::cout << performance[i] << std::endl;
		std:: cout << "Per second: " << performance[i] / time << std::endl;
	}

	int x;
	std::cout << "Enter a number to close. ";
	std::cin >> x;

	return 0;
}

/*
What the main loop should look like:

while (!quit){
//Wait the duration between frames.
this->wait(15ms);

//Lock the renderer so no more can be written to the back buffer
renderer.lock();

//Swap the renderer buffers
window.display();

//Unlock the buffers for writing.
render.unlock();
}


*/


//"An Ideal Game loop"
/*while (! done ) {
g.Update ( msec ); // advance your game forward
phys.Update ( msec ); // perform physics calculations
r.RenderScene ();
}
*/

//Game Entity Class
/*
class GameEntity {
virtual void Update ( float msec );
protected :
RenderObject renderer;
PhysicsObject physics;
SoundObject sound;
};*/