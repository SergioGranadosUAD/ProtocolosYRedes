//#pragma once

#include <SFML/Graphics.hpp>

using std::vector;
using sf::VertexArray;
using sf::Shape;
using sf::RectangleShape;
using sf::CircleShape;
using sf::RenderWindow;
using sf::Vector2f;
using sf::Color;
using sf::Angle;
using sf::radians;

enum SHAPE_TYPE 
{
	LINE = 0,
	RECTANGLE,
	CIRCLE,
	FREEDRAW,
	TEXT
};

enum SHAPE_COLOR
{
	BLACK = 0,
	WHITE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	MAGENTA,
	CYAN
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
	inline VertexArray* GetFreeDrawPtr() { return m_pFreeLineShape; };
	inline SHAPE_TYPE GetShapeType() { return m_ShapeType; };
	inline bool IsShapeCreated() { return m_ShapeCreated; };
	inline Vector2f getStartingPos() { return m_StartingPosition; };
	inline Vector2f getFinalPos() { return m_FinalPosition; };
	inline unsigned short getColorID() { return m_colorID; };
	vector<float> getFreedrawPositions();

	void SetColor(unsigned int colorID);
private:
	Shape* m_pBaseShape;
	VertexArray* m_pFreeLineShape;
	SHAPE_TYPE m_ShapeType;
	Vector2f m_StartingPosition;
	Vector2f m_FinalPosition;
	unsigned short m_colorID;
	bool m_ShapeCreated;
};

