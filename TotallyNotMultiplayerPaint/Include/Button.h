#pragma once

#include <functional>
#include "SFML/Graphics.hpp"

using sf::Sprite;
using sf::IntRect;
using sf::Texture;
using sf::Vector2f;
using sf::Vector2i;
using sf::RenderWindow;
using std::function;
using sf::FloatRect;
using sf::Color;

class Button {
public:
	Button();
	Button(const float& posX, const float& posY, const float& width, const float& height, const Texture& texture);
	virtual ~Button() = default;

	void render(RenderWindow& window);

	void setScale(float width, float height);
	void setPosition(float posX, float posY);
	void setColor(const Color& color);
	void setTexture(const Texture& pTexture);
	void setTextureRect(int posX,int posY, int width, int height);

	bool isInBounds(const Vector2f& mousePos);

	void setButtonAction(function<void()> fnc);
	void onClick();

private:

	Sprite m_drawableShape;
	Texture m_shapeTexture;
	function<void()> m_buttonFunction;
	//bool m_hovered;
};
