#include "../Include/Application.h"

Application::Application() :
	m_Window(VideoMode(Vector2u(DEFAULT_WIDTH, DEFAULT_HEIGHT), BITS_PER_PIXEL), "Paint test"),
	m_ActualShape(SHAPE_TYPE::LINE),
	m_PreviewShape(),
	m_MouseButtonDown(false) ,
	m_SelectedColor(Color::White)
{
	m_buttons[0].setPosition(50, 50);
	m_buttons[0].setSize(20, 20);
	m_buttons[0].setColor(Color::Red);

	m_buttons[1].setPosition(70, 50);
	m_buttons[1].setSize(20, 20);
	m_buttons[1].setColor(Color::Green);

	m_buttons[2].setPosition(50, 70);
	m_buttons[2].setSize(20, 20);
	m_buttons[2].setColor(Color::Blue);

	m_buttons[3].setPosition(70, 70);
	m_buttons[3].setSize(20, 20);
	m_buttons[3].setColor(Color::White);

	m_buttons[0].setButtonAction([this]()
		{
			setSelectedColor(Color::Red);

			//Prueba para ver la estructura de mensajes. ELIMINAR.
			MsgConnectRequest* msg = new MsgConnectRequest();
			m_client.sendMessage(msg, E::kLOGIN_REQUEST);
		});
	m_buttons[1].setButtonAction([this]()
		{
			setSelectedColor(Color::Green);
		});
	m_buttons[2].setButtonAction([this]()
		{
			setSelectedColor(Color::Blue);
		});
	m_buttons[3].setButtonAction([this]()
		{
			setSelectedColor(Color::White);
		});
}

Application::Application(int windowWidth, int windowHeight):
	m_Window(VideoMode(Vector2u(windowWidth, windowHeight), BITS_PER_PIXEL), "Paint test"),
	m_ActualShape(SHAPE_TYPE::LINE),
	m_PreviewShape(),
	m_MouseButtonDown(false) 
{

}

Application::~Application() 
{

}

void Application::HandleInput() 
{
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
			for (int i = 0; i < 4; ++i) {
				if (m_buttons[i].isInBounds(GetMousePosition())) {
					m_buttons[i].onClick();
					return;
				}
			}
			m_PreviewShape.CreateShape(GetMousePosition(), m_ActualShape);
			cout << "Shape created at pos " << GetMousePosition().x << ", " << GetMousePosition().y << endl;
			m_MouseButtonDown = true;
			break;
		case Event::MouseButtonReleased:
			if (m_MouseButtonDown) {
				m_PreviewShape.SetColor(m_SelectedColor);
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

	for (auto& buttons : m_buttons) {
		buttons.render(m_Window);
	}

	m_Window.display();
}

void Application::setSelectedColor(Color setColor)
{
	m_SelectedColor = setColor;
}

