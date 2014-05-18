#pragma once

#include <SFML\Graphics\Text.hpp>
#include <SFML\Window.hpp>
#include <SFML\Graphics\RenderWindow.hpp>

#include <iostream>
#include <string>
#include <sstream>

#include "../nclgl/Vector3.h"
#include "../nclgl/Vector2.h"
#include "FreqCircle.h"
#include "RenderObject.h"

#define FONT_PATH "Resources\\Fonts\\"

using std::string;

class HealthHUD
{
public:
	HealthHUD(Shader* shader, const Vector2& position, const Vector2& size);
	~HealthHUD(void);

	void Update(const FreqCircle& fq);
	void Render(const SRenderer& ortho);

protected:
	Matrix4 position;
	Vector2 size;

	RenderObject* redBar;
	RenderObject* greenBar;
};

