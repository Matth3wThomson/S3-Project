#pragma once //This will clean up extra #includes for me, its not supported as standard,
				//however is supported by VS2010 compiler (which the uni use), so 
			//please dont dock me marks for this! :)

#include "Circle.h"
//#include "Square.h"
//#include "ValueZeroException.h"
#include <iostream>
#include <cmath> //abs

/**
Creates a circle at the origin with a radius of 5
*/
Circle::Circle(void)
{
	radius = 5;
	vertices = std::vector<Vector2*>(1);
	vertices[0] = new Vector2();
	angle = 0;
}

/**
Creates a circle with a given radius and center coordinate.
*/
Circle::Circle(float radius, float x, float y)
{
	vertices = std::vector<Vector2*>(1);
	this->radius = radius;
	vertices[0] = new Vector2(x, y);
	angle = 0;
}

/**
Destructor for Circle
*/
Circle::~Circle(void)
{
	delete vertices[0];
	vertices.~vector();
}

/**
Returns PI r^2
*/
float Circle::GetArea() const
{
	return (float) PI * (radius * radius);
}

/**
Because this method is intended to rotate a shape about an anchor point,
the only anchor point for a circle is its center, and in such a case rotation
about its center is pointless, so this method does nothing.
*/
void Circle::Rotate(float angle){
	this->angle = angle;
}

/**
Override this method as there are too many normals to a circle!
*/
std::vector<Vector2> Circle::GetNormals() const{
	std::vector<Vector2> temp(0);
	return temp;
};

/**
This function takes in a pointer to a shape and runs the correct code
upon the result of a dynamic cast. 
Information on how to do this obtained from:
http://stackoverflow.com/questions/9021935/comparing-derived-classes-in-c-without-dynamic-casting-or-static-downcasting
*/
bool Circle::Overlap(Shape* rhs) const
{
	if (Circle* c = dynamic_cast<Circle*>(rhs)){
		//Code here to handle a circle overlapping a circle. Can only access protected members
		//if a dynamic cast is performed before hand
		if (this->vertices[0]->GetDistance(*c->vertices[0]) < this->radius + c->radius){
			return true;
		}
		return false;
	} else {
		return Shape::Overlap(rhs);
	}
}

/**
This method works out whether the circle is out of a given bound, and if it is
returns the MTV (minimum translation vector) to move it back into the bounds
*/
Vector2* Circle::OutOfBounds(float bound) const {

	float deltaX = bound - abs(vertices[0]->GetX() + radius);
	float deltaY = bound - abs(vertices[0]->GetY() + radius);

	bool outOfRange = false;

	Vector2* mtv = new Vector2();

	if (deltaX < 0){
		outOfRange = true;
		//We are out of range in x
		//Was it in the positive direction?
		if (vertices[0]->GetX() > 0){
			//If so translate the vector in the -ve direction
			mtv->Translate(deltaX, 0);
		} else {
			//If not translate the vector in the +ve direction
			mtv->Translate(-deltaX , 0);
		}
	}
	if (deltaY < 0){
		outOfRange = true;
		//We are out of range in y
		//Was it in the positive direction?
		if (vertices[0]->GetY() > 0){
			//If so translate the vector in the -ve direction
			mtv->Translate(0, deltaY);
		} else {
			//If not translate the vector in the +ve direction
			mtv->Translate(0, -deltaY);
		}
	}

	if (!outOfRange){
		delete mtv;
		return NULL;
	}
	return mtv;

};

/**
This method will project a circle onto a normalized axis.
This axis MUST BE NORMALIZED!
*/
Projection Circle::Project(Vector2& axis) const {
	float min = axis.DotProduct(*vertices[0]);
	float max = min + radius;
	min = min - radius;

	Projection proj(min, max);

	return proj;
}