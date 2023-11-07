#include "../Include/Button.h"

Button::Button() :
	m_drawableShape(m_shapeTexture)
{

}

Button::Button(const float& posX, const float& posY, const float& width, const float& height, const Texture& texture):
	m_drawableShape(texture)
{
	m_drawableShape.setPosition(Vector2f(posX, posY));
	m_drawableShape.setScale(Vector2f(width, height));
}

void Button::render(RenderWindow& window)
{
	window.draw(m_drawableShape);
}

void Button::setScale(float width, float height)
{
	Vector2f size(width, height);
	m_drawableShape.setScale(size);
}

void Button::setPosition(float posX, float posY)
{
	Vector2f position(posX, posY);
	m_drawableShape.setPosition(position);
}

void Button::setColor(const Color& color)
{
	m_drawableShape.setColor(color);
}

void Button::setTexture(const Texture& pTexture)
{
	m_drawableShape.setTexture(pTexture);
}

void Button::setTextureRect(int posX, int posY, int width, int height)
{
	Vector2i position(posX, posY);
	Vector2i size(width, height);
	m_drawableShape.setTextureRect(IntRect(position, size));
}

bool Button::isInBounds(const Vector2f& mousePos)
{
	FloatRect buttonBounds = m_drawableShape.getGlobalBounds();
	return buttonBounds.contains(mousePos);
}

void Button::onClick()
{
	if (m_buttonFunction) {
		m_buttonFunction();
	}
}

void Button::setButtonAction(function<void()> fnc)
{
	m_buttonFunction = fnc;
}
