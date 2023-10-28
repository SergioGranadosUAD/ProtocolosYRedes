#include "../Include/Button.h"

Button::Button(float posX, float posY, float width, float height)
{
	m_drawableShape.setPosition(Vector2f(posX, posY));
	m_drawableShape.setSize(Vector2f(width, height));
}

Button::~Button()
{

}

void Button::render(RenderWindow& window)
{
	window.draw(m_drawableShape);
}

void Button::setSize(float width, float height)
{
	Vector2f size(width, height);
	m_drawableShape.setSize(size);
}

void Button::setPosition(float posX, float posY)
{
	Vector2f position(posX, posY);
	m_drawableShape.setPosition(position);
}

void Button::setColor(const Color& color)
{
	m_drawableShape.setFillColor(color);
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
