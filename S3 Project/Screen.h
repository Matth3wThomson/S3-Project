#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "SRenderer.h"

class Screen
{
public:

	Screen(SRenderer& target, bool loadRes, Shader* s) : target(target), s(s){
		if (loadRes){
			LoadResources();
		}
	};

	~Screen(void){ }

	virtual void RenderScreen() = 0;
	virtual void LoadResources() = 0;
	virtual void DeleteResources() = 0;
	virtual void Update() = 0;

protected:
	SRenderer& target;
	Shader* s;

private:
	Screen();
};

