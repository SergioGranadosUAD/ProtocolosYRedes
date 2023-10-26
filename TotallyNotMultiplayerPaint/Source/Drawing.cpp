#include "../Include/Application.h"

Drawing::Drawing() :
	m_pBaseShape(nullptr),
	m_StartingPosition(Vector2f(0,0)),
	m_ShapeType(SHAPE_TYPE::LINE),
	m_LineVertexCount(0)
{

}

Drawing::~Drawing() 
{
	//delete m_pBaseShape;
}

void Drawing::Update(const Vector2f& actualPos)
{
	if (m_ShapeType == SHAPE_TYPE::LINE) {
		RectangleShape* pTempShape = reinterpret_cast<RectangleShape*>(m_pBaseShape);
		Vector2f shapeSize = actualPos - m_StartingPosition;
		float lineSize = sqrt((shapeSize.x * shapeSize.x) + (shapeSize.y * shapeSize.y));
		Angle lineAngle = radians(atan2(actualPos.y - m_StartingPosition.y, actualPos.x - m_StartingPosition.x));
		pTempShape->setSize(Vector2f(lineSize, 0));
		pTempShape->setRotation(lineAngle);
	}
	else if (m_ShapeType == SHAPE_TYPE::RECTANGLE) {
		RectangleShape* pTempShape = reinterpret_cast<RectangleShape*>(m_pBaseShape);
		Vector2f shapeSize = actualPos - m_StartingPosition;
		pTempShape->setSize(shapeSize);
	}
	else if (m_ShapeType == SHAPE_TYPE::CIRCLE) {
		CircleShape* pTempShape = reinterpret_cast<CircleShape*>(m_pBaseShape);
		float shapeRadius = actualPos.x * .5f - m_StartingPosition.x * .5f;
		float shapeScaleY = (actualPos.y - m_StartingPosition.y) / (actualPos.x - m_StartingPosition.x);
		pTempShape->setRadius(shapeRadius);
		pTempShape->setScale(Vector2f(1, shapeScaleY));
	}
	else if (m_ShapeType == SHAPE_TYPE::FREEDRAW) {
		_AddVertex(actualPos);
	}
}

void Drawing::Render(RenderWindow& window)
{
		window.draw(*m_pBaseShape);
}

void Drawing::Clear()
{
	if(m_pBaseShape != nullptr)
	{
		delete m_pBaseShape;
		m_pBaseShape = nullptr;
	}
}

void Drawing::Reset()
{
	if (m_pBaseShape != nullptr)
	{
		m_pBaseShape = nullptr;
	}
}

void Drawing::CreateShape(const Vector2f& startingPos, const SHAPE_TYPE& eShapeType)
{
	m_StartingPosition = startingPos;
	m_ShapeType = eShapeType;

	switch (m_ShapeType) {
	case SHAPE_TYPE::LINE:
		m_pBaseShape = new RectangleShape(Vector2f(0, 0));
		break;
	case SHAPE_TYPE::RECTANGLE:
		m_pBaseShape = new RectangleShape(Vector2f(0, 0));
		break;
	case SHAPE_TYPE::CIRCLE:
		m_pBaseShape = new CircleShape(0);
		break;
	case SHAPE_TYPE::FREEDRAW:
		m_LineVertexCount++;
		ConvexShape* pTempShape = new ConvexShape(m_LineVertexCount);
		pTempShape->setPoint(0, Vector2f(0, 0));
		m_pBaseShape = pTempShape;
		break;
	}

	m_pBaseShape->setPosition(m_StartingPosition);
	m_pBaseShape->setFillColor(sf::Color::Transparent);
	m_pBaseShape->setOutlineThickness(1.0f);
	m_pBaseShape->setOutlineColor(sf::Color::Red);
}

void Drawing::FinishFreeDraw()
{
	ConvexShape* pTempShape = reinterpret_cast<ConvexShape*>(m_pBaseShape);
	size_t pointIndex = pTempShape->getPointCount();
	for (int i = pointIndex-1; i > 0; --i) {
		m_LineVertexCount++;
		pTempShape->setPointCount(m_LineVertexCount);
		pTempShape->setPoint(m_LineVertexCount - 1, pTempShape->getPoint(i));
	}
}

void Drawing::SetColor(Color setColor)
{
	m_pBaseShape->setOutlineColor(setColor);
}

void Drawing::_AddVertex(const Vector2f& actualPos)
{
	
	ConvexShape* pTempShape = reinterpret_cast<ConvexShape*>(m_pBaseShape);
	if (pTempShape->getPoint(m_LineVertexCount-1) == actualPos) {
		return;
	}

	m_LineVertexCount++;
	pTempShape->setPointCount(m_LineVertexCount);
	pTempShape->setPoint(m_LineVertexCount - 1, actualPos - m_StartingPosition);
}


