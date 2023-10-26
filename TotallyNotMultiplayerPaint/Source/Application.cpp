#include "../Include/Application.h"

Application::Application() :
	m_Window(VideoMode(Vector2u(DEFAULT_WIDTH, DEFAULT_HEIGHT), BITS_PER_PIXEL), "Paint test"),
	m_ActualShape(SHAPE_TYPE::LINE),
	m_PreviewShape(),
	m_MouseButtonDown(false) {

}

Application::Application(int windowWidth, int windowHeight):
	m_Window(VideoMode(Vector2u(windowWidth, windowHeight), BITS_PER_PIXEL), "Paint test"),
	m_ActualShape(SHAPE_TYPE::LINE),
	m_PreviewShape(),
	m_MouseButtonDown(false) {

}

Application::~Application() {

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
			else if (keyboardEvent.key.code == sf::Keyboard::Escape && m_MouseButtonDown) {
				m_MouseButtonDown = false;
				m_PreviewShape.Clear();
			}
			break;
		case Event::MouseButtonPressed:
			initialMousePixelPos = sf::Mouse::getPosition(m_Window);
			m_InitialMousePosition = GetMousePosition();
			m_PreviewShape.CreateShape(m_InitialMousePosition, m_ActualShape);
			cout << "Shape created at pos " << m_InitialMousePosition.x << ", " << m_InitialMousePosition.y << endl;
			m_MouseButtonDown = true;
			break;
		case Event::MouseButtonReleased:
			if (m_MouseButtonDown) {
				m_PreviewShape.SetColor(sf::Color::White);
				m_ShapeList.push_back(m_PreviewShape);
				m_PreviewShape.Reset();
				m_MouseButtonDown = false;
			}
			break;
		}
	}
}

void Application::Update() {
	if (m_PreviewShape.GetShapePtr() != nullptr && m_MouseButtonDown) {
		m_PreviewShape.Update(GetMousePosition());
	}
}

void Application::Render() {
	m_Window.clear();

	if (m_PreviewShape.GetShapePtr() != nullptr) {
		m_PreviewShape.Render(m_Window);
	}

	for (auto& shape : m_ShapeList) {
		shape.Render(m_Window);
	}

	m_Window.display();
}

