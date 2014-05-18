#pragma once //This will clean up extra #includes for me, its not supported as standard,
				//however is supported by VS2010 compiler (which the uni use), so 
			//please dont dock me marks for this! :)

#include <iostream>

/**
A simple class that stores the minimum and maximum values of a shapes
projection onto a 1D plane
*/
class Projection
{
public:

	Projection(void)
	{
		min = 0;
		max = 0;
	}

	/*
	Constructor from basic max and min values
	*/
	Projection(float min, float max){
		this->max = max;
		this->min = min;
	}

	~Projection(void)
	{

	}

	/**
	Because a projection is 1 dimensional, only 2 checks are necessary
	*/
	bool Overlap(Projection &rhs){
		//If eithers min value is between the other shapes min and max, we have a collision
		if ( (rhs.min < this->max) && (rhs.min > this->min) ){
			//rhs min is between this min and max -> overlap
			return true;
		} else if ( (this->min < rhs.max) && (this->min > rhs.min) ){
			//this min is between rhs min and max -> overlap
			return true;
		}
		return false;
	};

	//OStream overloaded for projection
	inline friend std::ostream& operator<<(std::ostream& o, const Projection& p){
		o << "Projection(min: " << p.min << ", max: " << p.max << ")" << std::endl;
		return o;
	}

protected:
	float min, max;
};

