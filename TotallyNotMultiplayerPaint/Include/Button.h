#pragma once

#include <functional>
#include "SFML/Graphics.hpp"

using sf::RectangleShape;
using sf::Vector2f;
using sf::RenderWindow;
using std::function;
using sf::FloatRect;
using sf::Color;

class Button {
public:
	Button() = default;
	Button(float posX, float posY, float width, float height);
	virtual ~Button();

	void render(RenderWindow& window);

	void setSize(float width, float height);
	void setPosition(float posX, float posY);
	void setColor(const Color& color);

	bool isInBounds(const Vector2f& mousePos);

	void setButtonAction(function<void()> fnc);
	void onClick();

private:
	RectangleShape m_drawableShape;
	function<void()> m_buttonFunction;
	//bool m_hovered;
};
