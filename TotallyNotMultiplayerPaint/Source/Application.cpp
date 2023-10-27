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
			m_PreviewShape.CreateShape(GetMousePosition(), m_ActualShape);
			cout << "Shape created at pos " << GetMousePosition().x << ", " << GetMousePosition().y << endl;
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
		case Event::MouseMoved:
			if (m_MouseButtonDown) {
				if (m_PreviewShape.IsShapeCreated() && m_MouseButtonDown) {
					m_PreviewShape.Update(GetMousePosition());
				}
			}
			break;
		}
	}
}

void Application::Update() {

}

void Application::Render() {
	m_Window.clear();

	if (m_PreviewShape.IsShapeCreated()) {
		m_PreviewShape.Render(m_Window);
	}

	for (auto& shape : m_ShapeList) {
		shape.Render(m_Window);
	}

	m_Window.display();
}

