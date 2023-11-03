#include "../Include/Application.h"

Application::Application() :
	m_Window(VideoMode(Vector2u(DEFAULT_WIDTH, DEFAULT_HEIGHT), BITS_PER_PIXEL), "Paint test"),
	m_ActualShape(SHAPE_TYPE::LINE),
	m_PreviewShape(),
	m_MouseButtonDown(false) ,
	m_SelectedColor(Color::White)
{
	/////////////////////
	//	Placeholder
	/////////////////////
	string serverIP;
	uint16 serverPort;
	cout << "Introduzca la IP del servidor al que se desea conectar." << endl;
	cin >> serverIP;
	m_client.setServerIP(serverIP);
	cout << "Introduzca el puerto del servidor al que se desea conectar." << endl;
	cin >> serverPort;
	m_client.setServerPort(serverPort);
	string connectionMode;
	cout << "Introduzca 'l' para iniciar sesion, o 'r' para registrarse." << endl;
	cin >> connectionMode;

	E::NETWORK_MSG connectionType;
	if (connectionMode == "l")
	{
		connectionType = E::kLOGIN_REQUEST;
	}
	else if(connectionMode == "r")
	{
		connectionType = E::kSIGNUP_REQUEST;
	}
	else
	{
		connectionType = E::kLOGIN_REQUEST;
		cout << "Error al seleccionar. Intentando iniciar sesion." << endl;
	}

	MsgConnectRequest connectionRequest;
	m_client.sendMessage(&connectionRequest, connectionType);

	while (!m_client.isConnected())
	{
		uint16 msgType = -1;
		Package unpackedData;
		m_client.waitForMessage(&unpackedData, &msgType);

		if (msgType != -1 && !unpackedData.empty()) 
		{
			handlePackage(unpackedData, msgType);
		}
	}

	/////////////////////
	//	
	/////////////////////

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
	MsgDisconnected msg;
	m_client.sendMessage(&msg, E::kDISCONNECTION);
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
				sendShape();
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
	uint16 msgType;
	Package unpackedData;
	m_client.waitForMessage(&unpackedData, &msgType);
	
	if (msgType != -1 && !unpackedData.empty())
	{
		handlePackage(unpackedData, msgType);
	}
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

void Application::handlePackage(Package unpackedData, uint16 msgType)
{
	switch (msgType)
	{
	case E::kUSERNAME_REQUEST:
	{
		MsgUsernameRequest m;
		string realData = m.m_msgData;
		m.unpackData(&realData, unpackedData.data(), unpackedData.size());
		std::cout << "[SERVER] " << realData << endl;

		string username;
		cin >> username;

		MsgUsernameSent message(username);
		m_client.sendMessage(&message, E::kUSERNAME_SENT);
	}
	break;
	case E::kPASSWORD_REQUEST:
	{
		MsgPasswordRequest m;
		string realData = m.m_msgData;
		m.unpackData(&realData, unpackedData.data(), unpackedData.size());
		std::cout << "[SERVER] " << realData << endl;

		string password;
		cin >> password;

		MsgPasswordSent message(password);
		m_client.sendMessage(&message, E::kPASSWORD_SENT);


	}
	break;
	case E::kCONNECTION_SUCCESSFUL:
	{
		MsgConnected m;
		string realData = m.m_msgData;
		m.unpackData(&realData, unpackedData.data(), unpackedData.size());
		std::cout << "[SERVER] " << realData << endl;

		m_client.setConnection(true);
	}
	break;
	case E::kDISCONNECTION:
	{
		MsgDisconnected m;
		string realData;
		m.unpackData(&realData, unpackedData.data(), unpackedData.size());
		std::cout << "[SERVER] " << realData << endl;
		m_Window.close();
	}
	break;
	case E::kCREATE_LINE:
	{
		MsgCreateLine m;
		MsgCreateLine::LineData realData = *(reinterpret_cast<MsgCreateLine::LineData*>(unpackedData.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		SHAPE_TYPE shpType = SHAPE_TYPE::LINE;

		Drawing sentShape;
		sentShape.CreateShape(Vector2f(realData.initialPosX, realData.initialPosY), shpType);
		sentShape.Update(Vector2f(realData.finalPosX, realData.finalPosY));
		//sentShape.SetColor(static_cast<COLOR_TYPE>(realData.colorID));
		m_ShapeList.push_back(sentShape);

		std::cout << "Shape sync by server." << endl;
	}
	break;
	case E::kCREATE_RECTANGLE:
	{
		MsgCreateRectangle m;
		MsgCreateRectangle::RectangleData realData = *(reinterpret_cast<MsgCreateRectangle::RectangleData*>(unpackedData.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		SHAPE_TYPE shpType = SHAPE_TYPE::RECTANGLE;

		Drawing sentShape;
		sentShape.CreateShape(Vector2f(realData.initialPosX, realData.initialPosY), shpType);
		sentShape.Update(Vector2f(realData.finalPosX, realData.finalPosY));
		//sentShape.SetColor(static_cast<COLOR_TYPE>(realData.colorID));
		m_ShapeList.push_back(sentShape);

		std::cout << "Shape sync by server." << endl;
	}
	break;
	case E::kCREATE_CIRCLE:
	{
		MsgCreateCircle m;
		MsgCreateCircle::CircleData realData = *(reinterpret_cast<MsgCreateCircle::CircleData*>(unpackedData.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		SHAPE_TYPE shpType = SHAPE_TYPE::CIRCLE;

		Drawing sentShape;
		sentShape.CreateShape(Vector2f(realData.initialPosX, realData.initialPosY), shpType);
		sentShape.Update(Vector2f(realData.finalPosX, realData.finalPosY));
		//sentShape.SetColor(static_cast<COLOR_TYPE>(realData.colorID));
		m_ShapeList.push_back(sentShape);

		std::cout << "Shape sync by server." << endl;
	}
	break;
	case E::kCREATE_FREEDRAW:
	{
		MsgCreateFreedraw m;
		MsgCreateFreedraw::FreedrawData realData = *(reinterpret_cast<MsgCreateFreedraw::FreedrawData*>(unpackedData.data()));
		//m.unpackData(&realData, unpackedData.data(), unpackedData.size());

		SHAPE_TYPE shpType = SHAPE_TYPE::FREEDRAW;

		Drawing sentShape;
		sentShape.CreateFreedraw(realData.pointPositions);
		//sentShape.SetColor(static_cast<COLOR_TYPE>(realData.colorID));
		m_ShapeList.push_back(sentShape);
		std::cout << "Shape sync by server." << endl;

		/*Drawing sentShape;
		sentShape.CreateShape(Vector2f(realData.initialPosX, realData.initialPosY), shpType);
		sentShape.Update(Vector2f(realData.finalPosX, realData.finalPosY));
		//sentShape.SetColor(static_cast<COLOR_TYPE>(realData.colorID));
		m_ShapeList.push_back(sentShape);*/
	}
	break;
	}
}

void Application::sendShape()
{
	Vector2f startingPos = m_PreviewShape.getStartingPos();
	Vector2f finalPos = m_PreviewShape.getFinalPos();
	unsigned short colorID = 0;
	switch (m_ActualShape)
	{
	case SHAPE_TYPE::LINE:
	{
		MsgCreateLine msg(startingPos.x, startingPos.y, finalPos.x, finalPos.y, colorID);
		m_client.sendMessage(&msg, E::kCREATE_LINE);
	}
	break;
	case SHAPE_TYPE::RECTANGLE:
	{
		MsgCreateRectangle msg(startingPos.x, startingPos.y, finalPos.x, finalPos.y, colorID);
		m_client.sendMessage(&msg, E::kCREATE_RECTANGLE);
	}
	break;
	case SHAPE_TYPE::CIRCLE:
	{
		MsgCreateCircle msg(startingPos.x, startingPos.y, finalPos.x, finalPos.y, colorID);
		m_client.sendMessage(&msg, E::kCREATE_CIRCLE);
	}
	break;
	case SHAPE_TYPE::FREEDRAW:
	{
		vector<float> vertexPositions = m_PreviewShape.getFreedrawPositions();
		MsgCreateFreedraw msg(colorID, vertexPositions.size(), vertexPositions);
		m_client.sendMessage(&msg, E::kCREATE_FREEDRAW);
	}
	break;
	}
}

