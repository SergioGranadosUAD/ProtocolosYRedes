//#pragma once

#include <SFML/Graphics.hpp>

using std::vector;
using sf::Vertex;
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
	//inline VertexArray* GetFreeDrawPtr() { return m_pFreeLineShape; };
	inline SHAPE_TYPE GetShapeType() { return m_ShapeType; };
	inline bool IsShapeCreated() { return m_ShapeCreated; };
	inline Vector2f getStartingPos() { return m_StartingPosition; };
	inline Vector2f getFinalPos() { return m_FinalPosition; };
	inline Color getColorID() { return m_colorID; };
	inline unsigned int getShapeID() { return m_shapeID; };
	bool isValidLine(const Vector2f& initialPos, const Vector2f& finalPos);

	void SetColor(Color colorID);
private:
	Shape* m_pBaseShape;
	vector<Vertex>* m_pFreeLineShape;
	SHAPE_TYPE m_ShapeType;
	Vector2f m_StartingPosition;
	Vector2f m_FinalPosition;
	unsigned int m_shapeID;
	Color m_colorID;
	bool m_ShapeCreated;
};

