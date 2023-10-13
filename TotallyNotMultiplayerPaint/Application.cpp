#include "Application.h"

Application::Application() :
	m_Window(VideoMode(Vector2u(DEFAULT_WIDTH, DEFAULT_HEIGHT), BITS_PER_PIXEL), "Paint test"),
	m_ActualShape(SHAPE_TYPE::LINE),
	m_PreviewShape(nullptr),
	m_MouseButtonDown(false) {

}

Application::Application(int windowWidth, int windowHeight):
	m_Window(VideoMode(Vector2u(windowWidth, windowHeight), BITS_PER_PIXEL), "Paint test"),
	m_ActualShape(SHAPE_TYPE::LINE),
	m_PreviewShape(nullptr),
	m_MouseButtonDown(false) {

}

Application::~Application() {
	if (m_PreviewShape != nullptr) {
		delete m_PreviewShape;
	}

	for (Shape* pShape : m_ShapeList) {
		delete pShape;
	}
}

void Application::HandleInput() {
	Event keyboardEvent;
	Vector2i initialMousePixelPos;
	while (m_Window.pollEvent(keyboardEvent)) {
		switch (keyboardEvent.type) {
		case Event::Closed:
			m_Window.close();
			break;
		case Event::KeyPressed:
			if (keyboardEvent.key.code == sf::Keyboard::Num1) {
				m_ActualShape = SHAPE_TYPE::LINE;
			}
			else if (keyboardEvent.key.code == sf::Keyboard::Num2) {
				m_ActualShape = SHAPE_TYPE::RECTANGLE;
			}
			else if (keyboardEvent.key.code == sf::Keyboard::Num3) {
				m_ActualShape = SHAPE_TYPE::CIRCLE;
			}
			else if (keyboardEvent.key.code == sf::Keyboard::Num4) {
				m_ActualShape = SHAPE_TYPE::FREEDRAW;
			}
			break;
		case Event::MouseButtonPressed:
			initialMousePixelPos = sf::Mouse::getPosition(m_Window);
			m_InitialMousePosition = m_Window.mapPixelToCoords(initialMousePixelPos);
			m_PreviewShape = CreateShape(m_InitialMousePosition);
			cout << "Shape created at pos " << m_InitialMousePosition.x << ", " << m_InitialMousePosition.y << endl;
			m_MouseButtonDown = true;
			break;
		case Event::MouseButtonReleased:
			m_PreviewShape->setOutlineColor(sf::Color::White);
			m_ShapeList.push_back(m_PreviewShape);
			m_PreviewShape = nullptr;
			m_MouseButtonDown = false;
			break;
		}
	}
}

void Application::Update() {
	if (m_PreviewShape != nullptr && m_MouseButtonDown) {
		Vector2i actualMousePixelPos = sf::Mouse::getPosition(m_Window);
		Vector2f actualMouseWorldPos = m_Window.mapPixelToCoords(actualMousePixelPos);

		if(m_ActualShape == SHAPE_TYPE::LINE){
			RectangleShape* pTempShape = reinterpret_cast<RectangleShape*>(m_PreviewShape);
			Vector2f shapeSize = actualMouseWorldPos - m_InitialMousePosition;
			float lineSize = sqrt((shapeSize.x * shapeSize.x) + (shapeSize.y * shapeSize.y));
			Angle lineAngle = radians(atan2(actualMouseWorldPos.y - m_InitialMousePosition.y, actualMouseWorldPos.x - m_InitialMousePosition.x));
			pTempShape->setSize(Vector2f(lineSize, 0));
			pTempShape->setRotation(lineAngle);
		}
		else if (m_ActualShape == SHAPE_TYPE::RECTANGLE) {
			RectangleShape* pTempShape = reinterpret_cast<RectangleShape*>(m_PreviewShape);
			Vector2f shapeSize = actualMouseWorldPos - m_InitialMousePosition;
			pTempShape->setSize(shapeSize);
		}
		else if (m_ActualShape == SHAPE_TYPE::CIRCLE) {
			CircleShape* pTempShape = reinterpret_cast<CircleShape*>(m_PreviewShape);
			float shapeRadius = actualMouseWorldPos.x * .5f - m_InitialMousePosition.x * .5f;
			float shapeScaleY = (actualMouseWorldPos.y - m_InitialMousePosition.y) / (actualMouseWorldPos.x - m_InitialMousePosition.x);
			pTempShape->setRadius(shapeRadius);
			pTempShape->setScale(Vector2f(1, shapeScaleY));
		}
		else if (m_ActualShape == SHAPE_TYPE::FREEDRAW) {
			//Todo
		}
	}
}

void Application::Render() {
	m_Window.clear();

	if (m_PreviewShape != nullptr) {
		m_Window.draw(*m_PreviewShape);
	}

	for (Shape* pShape : m_ShapeList) {
		m_Window.draw(*pShape);
	}

	m_Window.display();
}

Shape* Application::CreateShape(Vector2f mousePosition)
{
	Shape* pShape = nullptr;
	switch (m_ActualShape) {
	case SHAPE_TYPE::LINE:
		pShape = new RectangleShape(Vector2f(0,0));
		break;
	case SHAPE_TYPE::RECTANGLE:
		pShape = new RectangleShape(Vector2f(0, 0));
		break;
	case SHAPE_TYPE::CIRCLE:
		pShape = new CircleShape(0);
		break;
	case SHAPE_TYPE::FREEDRAW:
		//Todo
		break;
	}

	pShape->setPosition(mousePosition);
	pShape->setFillColor(sf::Color::Transparent);
	pShape->setOutlineThickness(1.0f);
	pShape->setOutlineColor(sf::Color::Red);
	return pShape;
}

