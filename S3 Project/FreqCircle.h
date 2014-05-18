#pragma once

#include "RenderObject.h"
#include "FrequencyAnalysis.h"
#include "Circle.h"
#include "Settings.h"

#include "InputHandler.h"

//Thread Safety
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>

//Control
#include <SFML\Window.hpp>

/*
This class represents an in-game frequency circle.
Its methods should be used in the order:
	- Update Bars (Sets Bars to frequency spectrum)
	- Game Update (Updates the circle based on input)
	- Check And Resolve Collisions with others (Must be protected higher than this class)
	- Update (Update the model matrices of the object)
	- Render (Render the object)
*/

class FreqCircle : public RenderObject
{
public:
	FreqCircle(int accuracy, Shader* s, const Vector3& position, const float& radius, const int& joystick);
	virtual ~FreqCircle(void);

	virtual void GameUpdate(const float& msec); //TS

	//Call this every thread loop to update buffer
	virtual void Update(float msec); //TS

	virtual void UpdateBars(); //TS

	//This method acquires two locks! protect access to this method at higher level.
	//NOTE: Whichever thread does this method first will "push" the other object!
	void CheckAndResolveCollision(FreqCircle* rhs, float msec); //TS - multiple locks ^

	void CheckAndResolveBounds(const Vector2& lowerBound, const Vector2& upperBound);

	inline float GetHealth() const {
		//Scoped lock on mutex (Cant unlock after a return statement!)
		sharable_lock<interprocess_upgradable_mutex> lock(mutex);
		return health;
	}

	inline Vector3 GetPosition() const {
		sharable_lock<interprocess_upgradable_mutex> lock(mutex);
		return position;
	}

	inline int GetPlayer() const {
		return joystick;
	}

protected:
	FreqCircle(void);

	//Physics Properties
	float radius;
	float angle;
	Vector3 position;

	//Music properties
	int accuracy;
	float* buffer;
	float* specLeft;
	float* specRight;

	//Game properties
	float health;

	int joystick;
};

