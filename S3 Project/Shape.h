#pragma once //This will clean up extra #includes for me, its not supported as standard,
				//however is supported by VS2010 compiler (which the uni use), so 
			//please dont dock me marks for this! :)

#include "..\nclgl\Vector2.h"
#include <string>
#include <vector>
#include "Projection.h"

/**
This class represents any CONVEX shape. 
*/
class Shape
{
public:
	Shape(void);

	//Virtual functions can be overriden, Pure virtual functions MUST 
	//be overriden (=0)
	virtual ~Shape(void) = 0;
	virtual float GetArea() const = 0;

	//This method will rotate the shape about an anchor point when implemented, rather
	//than the origin.
	virtual void Rotate(float angle) = 0;

	//This method cannot be done by value, must supply a pointer (cant instantiate
	// an abstract class)
	virtual bool Overlap(Shape* rhs) const;
	virtual std::vector<Vector2> GetNormals() const;
	virtual Projection Project(Vector2 &axis) const;
	virtual void Translate(float x, float y);
	virtual void Translate(const Vector2& translation);

	//Returns the sum of the vectors in the object / the number of vectors (the center)
	inline virtual Vector2 GetCenter() const {
		Vector2 temp = *vertices[0];
		for (size_t i=1; i<vertices.size(); ++i){
			temp = temp + *vertices[i];
		}
		temp = temp / (float) vertices.size();
		return temp;
	}

	//Returns the vector in the vector at the given array location, Cannot refer
	//to an array of an unistantiatable type!
	inline Vector2* GetVector(int arrayLocation) const{
		arrayLocation %= vertices.size();
		return vertices[arrayLocation];
	}

	//Returns the size of the vector, as you cannot refer to a vector of an 
	//uninstantiable type!
	inline int GetNoVerts() const{
		return vertices.size();
	}

	//Returns a MTV if the shape is out of bounds, else returns null
	virtual Vector2* OutOfBounds(float bound) const;

	//Overrideable print method to be used in the ostream below
	virtual inline void print(std::ostream& where) const{
		where << "Shape: \n";
		for (unsigned int i=0; i<vertices.size(); i++){
			where << "Vertex " << i << *vertices[i] << std::endl;
		}
	};

	//Ostream override for shape.
	inline friend std::ostream& operator<<(std::ostream& o, const Shape& s){
		s.print(o);
		return o;
	}

protected:
	/*
	Vector used to prevent cache misses, despite worse syntax.
	Having all the points stored rather than a single point and a width, means
	a higher memory overhead, but lower performance overhead, as all points need to be
	calculated whenever a collision is being checked.
	Having the array defined here as a vector means it is dynamic, and therefore can take any number
	of vertices, and therefore any shape can be defined from this abstract class.
	This would also be better for a graphics perspective, as meshes are constructed from points,
	which are fed to the gfx card, EVEN THOUGH this is not a graphics module, its worth noting.
	*/
	std::vector<Vector2*> vertices;
};

