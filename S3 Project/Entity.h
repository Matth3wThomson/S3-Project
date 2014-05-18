#pragma once
#include "..\nclgl\Vector3.h"
#include "RenderObject.h"


class Entity
{
public:
	Entity(void);
	~Entity(void);

protected:
	Vector3 position;
	RenderObject rObj;
};

