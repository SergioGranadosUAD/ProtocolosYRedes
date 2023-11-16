#include "../Include/Application.h"

Drawing::Drawing() :
	m_pBaseShape(nullptr),
	m_StartingPosition(Vector2f(0,0)),
	m_ShapeType(SHAPE_TYPE::LINE),
	m_ShapeCreated(false),
	m_pFreeLineShape(nullptr),
	m_colorID(0)
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

	m_FinalPosition = actualPos;
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
	m_pBaseShape->setOutlineThickness(1.0f);
	m_pBaseShape->setOutlineColor(sf::Color::Red);
}


vector<float> Drawing::getFreedrawPositions()
{
	vector<float> vec;
	for (int i = 0; i < m_pFreeLineShape->getVertexCount(); ++i)
	{
		vec.push_back((*m_pFreeLineShape)[i].position.x);
		vec.push_back((*m_pFreeLineShape)[i].position.y);
	}
	return vec;
}

void Drawing::SetColor(unsigned int colorID)
{
	m_colorID = colorID;
	Color setColor;
	switch (colorID)
	{
	case SHAPE_COLOR::BLACK:
		setColor = Color::Black;
		break;
	case SHAPE_COLOR::WHITE:
		setColor = Color::White;
		break;
	case SHAPE_COLOR::RED:
		setColor = Color::Red;
		break;
	case SHAPE_COLOR::GREEN:
		setColor = Color::Green;
		break;
	case SHAPE_COLOR::BLUE:
		setColor = Color::Blue;
		break;
	case SHAPE_COLOR::YELLOW:
		setColor = Color::Yellow;
		break;
	case SHAPE_COLOR::MAGENTA:
		setColor = Color::Magenta;
		break;
	case SHAPE_COLOR::CYAN:
		setColor = Color::Cyan;
		break;
	default:
		setColor = Color::Black;
	}

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

