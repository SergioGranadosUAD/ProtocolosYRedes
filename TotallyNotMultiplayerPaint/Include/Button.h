#pragma once

#include "SFML/Graphics.hpp"

using sf::RectangleShape;

class Button {
public:
	Button() = default;
	Button(float posX, float posY, float width, float height);
	~Button();

private:
	RectangleShape m_drawableShape;
};
