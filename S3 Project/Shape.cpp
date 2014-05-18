#pragma once //This will clean up extra #includes for me, its not supported as standard,
				//however is supported by VS2010 compiler (which the uni use), so 
			//please dont dock me marks for this! :)

#include "Shape.h"


Shape::Shape(void)
{
	
}


Shape::~Shape(void)
{
}

/**
Method to translate the shape by a given x and y coordinate.
*/
void Shape::Translate(float x, float y){
	for (size_t i=0; i<vertices.size(); ++i){
		vertices[i]->Translate(x, y);
	}
}

/*
Method to translate the shape by a given Vector.
Passed by reference to save making unnecessary copies
*/
void Shape::Translate(const Vector2& rhs){
	for (size_t i=0; i<vertices.size(); ++i){
		vertices[i]->Translate(rhs);
	}
}

/**
Implementing the seperate Axis theorem for all shapes.
	ASSUMPTIONS:
		-All shapes are convex!

This Theorem is good as the calculations of a shape colliding can exit as soon
as one test fails, making the algorithm fast for non colliding shapes. It is ideal for when
a lot of shapes need to be checked

*/
bool Shape::Overlap(Shape *rhs) const{

	//The shape supplied is the current shape, cant overlap yourself!
	if (this == rhs){
		return false;
	}

	//Step 1. Get all the axes of both shapes to test.
	std::vector<Vector2> axes1 = this->GetNormals();
	std::vector<Vector2> axes2 = rhs->GetNormals();

	//Step 2. Loop over axes 1
	for (size_t i = 0; i < axes1.size(); i++){
		Vector2 axis = axes1[i];

		//In case this is a circle, we must normalise the axis
		axis = axis.GetNormalized();

		//Project both shapes onto the axis
		Projection p1 = this->Project(axis);
		Projection p2 = rhs->Project(axis);

		//Do the projections overlap?
		if (!p1.Overlap(p2)){
			return false;
		} //If they do we need to check by how much!
	}

	//Step 3. over axes 2
	for (size_t i=0; i < axes2.size(); i++){
		Vector2 axis = axes2[i];

		//In case this is a circle, we must normalise the axis
		axis = axis.GetNormalized();

		//Project both shapes onto the axis
		Projection p1 = this->Project(axis);
		Projection p2 = rhs->Project(axis);

		//Do the projections overlap?
		if (!p1.Overlap(p2)){
			return false;
		}
	}

	//Step 4.
	//Check the axes found between the center of one shape,
	//and the center of the other (mainly used for circles, but an extra 
	//check on other shapes wont hurt)
	Vector2 rhsCenter = rhs->GetCenter();

	//Find the normal to the resultant vector of the center of this shape
	//and the center of the other shape
	Vector2 axis = this->GetCenter() - rhsCenter;

	//We must normalize the axis, as it could be a circle we're projecting
	axis = axis.GetNormalized();

	//Project the circle and the shape onto the found axis
	Projection p1 = rhs->Project(axis);
	Projection p2 = this->Project(axis);

	//See if the projections overlap?
	if (!p1.Overlap(p2)){
		return false;
	}

	//If we reach here, the shapes must be colliding!
	return true;
}

/**
Method returns a vector of vector2s that are the normals of every side of the given
shape.
*/
std::vector<Vector2> Shape::GetNormals() const{
	std::vector<Vector2> axes(this->vertices.size());

	for (size_t i=0; i < this->vertices.size(); ++i){
		//Dont call constructor of temp
		Vector2 temp;

		//Get the current Vertex
		Vector2 v1 = *this->vertices[i];

		//Get the next vertex
		Vector2 v2 = *this->vertices[i + 1 == this->vertices.size() ? 0 : i + 1];

		//Subtract the two to get the edge vector
		temp = v1 - v2;

		//Calculate the perpendicular vector
		temp = temp.perpVec();

		//Add it to the vector of vectors of a shapes normals
		axes[i] = temp;
	}

	return axes;
}

/**
Projects an object of "shape" (Although abstract) on to
a 1D plane defined by the line axis
*/
Projection Shape::Project(Vector2& axis) const{
	//Have a start value you can move away from (if necessary)

	float min = axis.DotProduct(*this->vertices[0]);
	float max = min;

	//For the rest of the vertices in the shape
	for (size_t i=1; i<this->vertices.size(); ++i){
		//Do the dot product with the axis being tested and the vertex tested
		float temp = axis.DotProduct(*this->vertices[i]);

		//If the value is smaller put it in min, else if bigger than max put it in max
		if (temp < min){
			min = temp;
		} else if (temp > max){
			max = temp;
		}
	}

	//Create a new projection, min and max
	Projection proj(min, max);

	//return the new projection
	return proj;
}

/**
This method will return a vector containing the minimum translation vector
of the current shape given a bound. If the shape is not out of bounds,
it will return a null pointer
*/
Vector2* Shape::OutOfBounds(float bound) const{
	float deltaX = 0;
	float deltaY = 0;
	int arrLocX;
	int arrLocY;

	bool outOfRange = false;

	//For every point in our shape
	for (size_t i=0; i<vertices.size(); ++i){
		
		//Check if the x coord is out of bounds
		if (bound - abs(vertices[i]->GetX()) < deltaX){
			deltaX = bound - abs(vertices[i]->GetX());
			arrLocX = i;
		}

		//Check if the y coord is out of bounds
		if (bound - abs(vertices[i]->GetY()) < deltaY){
			deltaY = bound - abs(vertices[i]->GetY());
			arrLocY = i;
		}
	}

	Vector2* mtv = new Vector2();

	//if Delta X and delta Y are now less than zero, we must translate the
	//shape by delta X and delta Y
	if (deltaX < 0){
		outOfRange = true;
		//We are out of range in x
		//Was it in the +ve direction?
		if (vertices[arrLocX]->GetX() > 0){
			//If so translate in -ve direction
			mtv->Translate(deltaX, 0);
		} else {
			//If not translate in +ve direction
			mtv->Translate(-deltaX, 0);
		}
	}

	if (deltaY < 0){
		outOfRange = true;
		//We are out of range in y
		//Was it in the +ve direction?
		if (vertices[arrLocY]->GetY() > 0){
			//If so translate in -ve direction
			mtv->Translate(0, deltaY);
		} else {
			//If not translate in +ve direction
			mtv->Translate(0, -deltaY);
		}

	}

	if (!outOfRange){
		delete mtv;
		return NULL;
	}
	return mtv;
}