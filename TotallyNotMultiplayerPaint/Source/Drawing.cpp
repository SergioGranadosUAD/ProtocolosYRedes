#include "../Include/Application.h"

Drawing::Drawing() :
	m_pBaseShape(nullptr),
	m_StartingPosition(Vector2f(0,0)),
	m_ShapeType(SHAPE_TYPE::LINE),
	m_pFreeLineShape(nullptr)
{

}

Drawing::~Drawing() 
{
	//delete m_pBaseShape;
}

void Drawing::Update(const Vector2f& actualPos)
{
	if (m_ShapeType == SHAPE_TYPE::LINE) {
		RectangleShape* pTempShape = static_cast<RectangleShape*>(m_pBaseShape);
		Vector2f shapeSize = actualPos - m_StartingPosition;
		float lineSize = sqrt((shapeSize.x * shapeSize.x) + (shapeSize.y * shapeSize.y));
		Angle lineAngle = radians(atan2(actualPos.y - m_StartingPosition.y, actualPos.x - m_StartingPosition.x));
		pTempShape->setSize(Vector2f(lineSize, 0));
		pTempShape->setRotation(lineAngle);
	}
	else if (m_ShapeType == SHAPE_TYPE::RECTANGLE) {
		RectangleShape* pTempShape = static_cast<RectangleShape*>(m_pBaseShape);
		Vector2f shapeSize = actualPos - m_StartingPosition;
		pTempShape->setSize(shapeSize);
	}
	else if (m_ShapeType == SHAPE_TYPE::CIRCLE) {
		CircleShape* pTempShape = static_cast<CircleShape*>(m_pBaseShape);
		float shapeRadius = actualPos.x * .5f - m_StartingPosition.x * .5f;
		float shapeScaleY = (actualPos.y - m_StartingPosition.y) / (actualPos.x - m_StartingPosition.x);
		pTempShape->setRadius(shapeRadius);
		pTempShape->setScale(Vector2f(1, shapeScaleY));
	}
	else if (m_ShapeType == SHAPE_TYPE::FREEDRAW) 
	{
		if (m_StartingPosition == actualPos) 
		{
			return;
		}

		m_pFreeLineShape->append(actualPos);
		size_t vertexCount = m_pFreeLineShape->getVertexCount() - 1;
		(*m_pFreeLineShape)[vertexCount].color = sf::Color::Red;
	}
}

void Drawing::Render(RenderWindow& window)
{
	if (m_pFreeLineShape != nullptr) 
	{
		window.draw(*m_pFreeLineShape);
		return;
	}

		window.draw(*m_pBaseShape);
}

void Drawing::Clear()
{
	m_ShapeCreated = false;
	if (m_pFreeLineShape != nullptr) 
	{
		delete m_pFreeLineShape;
		m_pFreeLineShape = nullptr;
		return;
	}

	if(m_pBaseShape != nullptr)
	{
		delete m_pBaseShape;
		m_pBaseShape = nullptr;
	}
}

void Drawing::Reset()
{
	m_ShapeCreated = false;
	if (m_pFreeLineShape != nullptr) 
	{
		m_pFreeLineShape = nullptr;
		return;
	}

	if (m_pBaseShape != nullptr)
	{
		m_pBaseShape = nullptr;
	}
}

void Drawing::CreateShape(const Vector2f& startingPos, const SHAPE_TYPE& eShapeType)
{
	m_ShapeCreated = true;
	m_StartingPosition = startingPos;
	m_ShapeType = eShapeType;

	if (m_ShapeType == SHAPE_TYPE::FREEDRAW) 
	{
		m_pFreeLineShape = new VertexArray(sf::PrimitiveType::LineStrip, 0);
		m_pFreeLineShape->append(startingPos);
		(*m_pFreeLineShape)[0].color = sf::Color::Red;
		return;
	}

	switch (m_ShapeType) 
	{
	case SHAPE_TYPE::LINE:
		m_pBaseShape = new RectangleShape(Vector2f(0, 0));
		break;
	case SHAPE_TYPE::RECTANGLE:
		m_pBaseShape = new RectangleShape(Vector2f(0, 0));
		break;
	case SHAPE_TYPE::CIRCLE:
		m_pBaseShape = new CircleShape(0);
		break;
	}

	m_pBaseShape->setPosition(m_StartingPosition);
	m_pBaseShape->setFillColor(sf::Color::Transparent);
	m_pBaseShape->setOutlineThickness(5.0f);
	m_pBaseShape->setOutlineColor(sf::Color::Red);
}

void Drawing::SetColor(Color setColor)
{
	if (m_ShapeType == SHAPE_TYPE::FREEDRAW) 
	{
		size_t vertexCount = m_pFreeLineShape->getVertexCount() - 1;
		for (int i = 0; i < vertexCount; ++i) 
		{
			(*m_pFreeLineShape)[i].color = setColor;
		}
		return;
	}

	m_pBaseShape->setOutlineColor(setColor);
}

