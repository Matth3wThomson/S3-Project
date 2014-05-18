#include "FreqCircle.h"

float clamp(float x, float a, float b){
	//return x < a ? a : (x > b ? b : x);

	if (x < a){
		return a;
	} else if (x > b){
		return b;
	} else {
		return x;
	}
}

FreqCircle::FreqCircle(int accuracy, Shader* s, const Vector3& position, const float& radius, const int& joystick)
	: RenderObject(Mesh::GenerateCircle(accuracy, radius), s)
{
	this->joystick = joystick;
	this->radius = radius;
	this->position = position;
	this->angle = 0.0f;
	this->health = Settings::MAX_HEALTH;

	this->accuracy = accuracy;
	buffer = new float[accuracy];
	specLeft = new float[accuracy];
	specRight = new float[accuracy];

	//Create Bars to surround this circle, and add them as child. Declaring as stack
	//caused a memory access violation!?
	for (int i=0; i<accuracy; ++i){
		Mesh* m = Mesh::GenerateBar(i, accuracy, this->radius);

		RenderObject* r = new RenderObject(m, this->shader);
		this->AddChild(r);
	}
}

void FreqCircle::GameUpdate(const float& msec){
	float x, y, u;

	if (InputHandler::Instance().GetJoystick(joystick) != -1){

		//Poll the controller Axis positions
		x = sf::Joystick::getAxisPosition(InputHandler::Instance().GetJoystick(joystick), sf::Joystick::X);
		y = sf::Joystick::getAxisPosition(InputHandler::Instance().GetJoystick(joystick), sf::Joystick::Y);
		u = sf::Joystick::getAxisPosition(InputHandler::Instance().GetJoystick(joystick), sf::Joystick::U);
	}

	//Apply Tolerance transforms
	if (abs(x) < Settings::JOYSTICK_TOLERANCE){
		x = 0;
	}

	if (abs(y) < Settings::JOYSTICK_TOLERANCE){
		y = 0;
	}

	if (abs(u) < Settings::JOYSTICK_TOLERANCE){
		u = 0;
	}

	if (joystick == 0){
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			y = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			y = 100.0f;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
			x = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
			x = 100.0f;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
			u = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
			u = 100.0f;
		}
	} else if (joystick == 1){
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::I)){
			y = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)){
			y = 100.0f;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)){
			x = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)){
			x = 100.0f;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Comma)){
			u = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period)){
			u = 100.0f;
		}
	} else if (joystick == 2){
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
			y = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
			y = 100.0f;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
			x = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
			x = 100.0f;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)){
			u = -100.0f;
		} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)){
			u = 100.0f;
		}
	}

	x = x * Settings::AXIS_SENSITIVITY * msec;
	y = y * Settings::AXIS_SENSITIVITY * msec;
	u = u * Settings::AXIS_SENSITIVITY * 20.0f * msec;

	//Exclusive lock here!
	Lock();

	this->position  += Vector3(x, -y, 0);

	//Adjust Angle
	this->angle -= u;

	//Make sure the angle doesnt go higher than 360 degrees and lower than zero!
	if (this->angle > 360.0f) this->angle -= 360.0f;

	if (this->angle < 0) this->angle = 360 - this->angle;

	Unlock();
}

void FreqCircle::Update(float msec){

	//SetModelMatrix method is threadsafe, no lock necessary
	this->SetModelMatrix(Matrix4::Translation(position) 
		* Matrix4::Rotation(angle, Vector3(0,0,1)));

	//RenderObject update is threadsafe too.
	RenderObject::Update(msec);
};

void FreqCircle::UpdateBars(){
	//Exclusive Lock this object while bars are updated.
	//Fill our buffer with frequency information
	Lock();
	FrequencyAnalysis::Instance().Fill(buffer, specLeft, specRight);

	//Apply the correct transformations to the children (the bars)
	for (unsigned int i = 0; i< children.size(); ++i){

		float angle = ((float) i / accuracy) * 360;
		float modification = (float) i+1.0f;

		float value = clamp(buffer[i] * modification , 0.0f, 1.0f);

		//This method is threadsafe, no children locks necessary
		children[i]->SetModelMatrix(
			Matrix4::Translation(Vector3(
			value * cos(DegToRad( angle )),
			value * sin(DegToRad( angle )), -0.05f)));
	}
	Unlock();
}

FreqCircle::~FreqCircle(void)
{
	delete[] buffer;
	delete[] specLeft;
	delete[] specRight;
}

void FreqCircle::CheckAndResolveCollision(FreqCircle* rhs, float msec){
	//Acquire read lock on our cricle and an upgradeable lock on the rhs
	this->ReadLock();
	rhs->LockUpgradable();

	//Distance is a Non-square rooted distance
	float distance = this->position.GetDistanceNSq(rhs->position);

	if (distance < pow((2 * this->radius) + rhs->radius, 2)){
		//The other shape is now in range of this shapes bars

		//Upon collision, we calculate distance more accurately for further checks.
		distance = this->position.GetDistance(rhs->position);

		if (distance < this->radius + rhs->radius){
			//The other shape is now intersecting this shape.

			//Calculate the resolution
			Vector3 conNormal = (this->position - rhs->position).GetNormalised();
			float penDepth = this->radius + rhs->radius - distance;

			rhs->UpgradeLock();
			rhs->position -= conNormal * penDepth; //Resolve between locks
			rhs->DowngradeLock();
		}

		//Now check to see how much damage is done to the other shape.

		//First find angle between this shape and other shape
		float xDiff = rhs->position.x - position.x;
		float yDiff = rhs->position.y - position.y;
		float angleBetween = (float) RadToDeg(atan2(yDiff, xDiff));

		//Add an adjustment for the angle our shape is at.
		angleBetween -= this->angle;

		//Find the bar at that angle
		int bar = static_cast<int>(angleBetween * (static_cast<float>(accuracy) / 360));

		//Correction for bar less than zero
		if (bar < 0) bar = (accuracy+bar);

		//Calculate the damage to be done to opposing shape
		float damageToDo = 0.0f;

		//Work out if most optimal bar actually intersects the circle
		if (distance < this->radius + rhs->radius + (this->radius * (buffer[bar] * (bar+1)) )){
			damageToDo += 0.3f;
		}

		//Check the bar to its right
		int tempBar = bar-1;
		if (tempBar < 0) tempBar = accuracy-1;

		if (distance < this->radius + rhs->radius + (this->radius * (buffer[tempBar] * (tempBar+1)) )){
			//TODO: Do we want damage as a constant?
			damageToDo += 0.3f;
		}

		//Check bar to its left
		tempBar = bar+1;
		if (tempBar > accuracy-1) tempBar = 0;

		if (distance < this->radius + rhs->radius + (this->radius * (buffer[tempBar] * (tempBar+1)) )){
			damageToDo += 0.3f;
		}

		//Check if damage needs to be done
		if (damageToDo > 0.01f){
			//Dps management alterations (Max dps)
			damageToDo = damageToDo * Settings::MAX_DPS * msec;

			rhs->UpgradeLock();
			//rhs->health -= damageToDo; //Apply damage between locks
			rhs->DowngradeLock();
		}

	};

	this->ReadUnlock();
	rhs->UnlockUpgradable();

};

void FreqCircle::CheckAndResolveBounds(const Vector2& lowerBound, const Vector2& upperBound){
	float x = 0.0f, y = 0.0f;

	this->LockUpgradable();

	//Check X coordinate isnt out of bounds
	if (this->position.x + this->radius > upperBound.x){
		x = upperBound.x - this->radius;
	} else if (this->position.x - this->radius < lowerBound.x){
		x = lowerBound.x  + this->radius;
	}

	//Check Y coordinate with bounds
	if (this->position.y + this->radius > upperBound.y){
		y = upperBound.y - this->radius;
	} else if (this->position.y - this->radius < lowerBound.y){
		y = lowerBound.y + this->radius;
	}

	//Only lock if there is a need too, then translate by required amount
	if (abs(x) > 0.01f || abs(y) > 0.01f){
		this->UpgradeLock();
		if (abs(x) > 0.01f) this->position.x = x;
		if (abs(y) > 0.01f) this->position.y = y;
		this->DowngradeLock();
	}

	this->UnlockUpgradable();
};