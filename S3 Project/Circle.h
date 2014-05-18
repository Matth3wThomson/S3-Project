#pragma once //This will clean up extra #includes for me, its not supported as standard,
				//however is supported by VS2010 compiler (which the uni use), so 
			//please dont dock me marks for this! :)

#include "shape.h"
#include <stdlib.h>

/**
A class to store a circle.
vertices[0] is the center of the circle, and the radius is stored as a sperate float
The rotate method for this class is pointless as it is a perfect circle, and therefore does
nothing.
*/
class Circle : public Shape
{
public:
	//Constructors
	Circle(void);
	Circle(float radius, float x, float y);

	//Destructor
	virtual ~Circle(void);

	//Getters
	virtual float GetArea() const;
	inline float GetRadius() const { return radius; } 
	virtual Vector2 GetCenter() const {return *vertices[0]; }

	//Method to translate and move the object
	virtual void Rotate(float angle);

	//Checks whether this CIRCLE has overlapped another shape
	virtual bool Overlap(Shape* rhs) const;

	//Returns a vector of the normals to this circle
	virtual std::vector<Vector2> GetNormals() const;

	//Returns a pointer to a minimum translation vector to get the circle back
	//in bounds
	virtual Vector2* OutOfBounds(float bound) const;

	//Projects the circle onto any NORMALIZED axis
	virtual Projection Project(Vector2& axis) const;

	//Virtual print method
	virtual inline void print(std::ostream& where) const{
		where << "Circle: \nCenter: " << *vertices[0];
		where << "Radius: " << radius;
	};
	
protected:
	/*
	Because we arent actually drawing this circle, the points
	that surround the circle don't need to be stored. (Would easily be
	implemented though)
	*/
	float radius;
	float angle;
};

