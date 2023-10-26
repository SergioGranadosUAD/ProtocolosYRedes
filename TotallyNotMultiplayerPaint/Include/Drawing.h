//#pragma once

#include <SFML/Graphics.hpp>

using sf::Shape;
using sf::RectangleShape;
using sf::CircleShape;
using sf::RenderWindow;
using sf::Vector2f;
using sf::Color;
using sf::Angle;
using sf::radians;

enum SHAPE_TYPE {
	LINE = 0,
	RECTANGLE,
	CIRCLE,
	FREEDRAW,
	TEXT
};

class Drawing
{
public:
	Drawing();
	virtual ~Drawing();

	void Update(const Vector2f& actualPos);
	void Render(RenderWindow& window);
	void Clear();
	void Reset();
	void CreateShape(const Vector2f& startingPos, const SHAPE_TYPE& eShapeType);

	inline Shape* GetShapePtr() { return m_pBaseShape; };

	void SetColor(Color setColor);
private:
	Shape* m_pBaseShape;
	SHAPE_TYPE m_ShapeType;
	Vector2f m_StartingPosition;
	Vector2f m_FinalPosition;

};

