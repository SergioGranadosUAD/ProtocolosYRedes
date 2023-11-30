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
		MsgConnectRequest connectionRequest;
		m_client.sendMessage(&connectionRequest, connectionType);
	}
	else if(connectionMode == "r")
	{
		connectionType = E::kSIGNUP_REQUEST;
		MsgSignupRequest connectionRequest;
		m_client.sendMessage(&connectionRequest, connectionType);
	}
	else
	{
		connectionType = E::kLOGIN_REQUEST;
		cout << "Error al seleccionar. Intentando iniciar sesion." << endl;
		MsgConnectRequest connectionRequest;
		m_client.sendMessage(&connectionRequest, connectionType);
	}

	

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

	m_buttonsTexture.loadFromFile("Resources/buttons.png");
	initButtons();
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

void Application::initButtons()
{
	for (int i = 0; i < BUTTON_COUNT; ++i) {
		m_buttons[i].setTexture(m_buttonsTexture);
	}

	//Color buttons
	m_buttons[0].setPosition(50, 50);
	m_buttons[0].setTextureRect(100, 0, 20, 20);

	m_buttons[1].setPosition(70, 50);
	m_buttons[1].setTextureRect(120, 0, 20, 20);

	m_buttons[2].setPosition(50, 70);
	m_buttons[2].setTextureRect(100, 20, 20, 20);

	m_buttons[3].setPosition(70, 70);
	m_buttons[3].setTextureRect(120, 20, 20, 20);

	m_buttons[4].setPosition(50, 90);
	m_buttons[4].setTextureRect(100, 40, 20, 20);

	m_buttons[5].setPosition(70, 90);
	m_buttons[5].setTextureRect(120, 40, 20, 20);

	m_buttons[6].setPosition(50, 110);
	m_buttons[6].setTextureRect(100, 60, 20, 20);

	m_buttons[7].setPosition(70, 110);
	m_buttons[7].setTextureRect(120, 60, 20, 20);

	//Shape buttons
	m_buttons[8].setPosition(20, 450);
	m_buttons[8].setTextureRect(0, 0, 50, 50);

	m_buttons[9].setPosition(80, 450);
	m_buttons[9].setTextureRect(50, 0, 50, 50);

	m_buttons[10].setPosition(20, 510);
	m_buttons[10].setTextureRect(0, 50, 50, 50);

	m_buttons[11].setPosition(80, 510);
	m_buttons[11].setTextureRect(50, 50, 50, 50);


	m_buttons[0].setButtonAction([this]()
		{
			setSelectedColor(Color::Black);
		});
	m_buttons[1].setButtonAction([this]()
		{
			setSelectedColor(Color::White);
		});
	m_buttons[2].setButtonAction([this]()
		{
			setSelectedColor(Color::Red);
		});
	m_buttons[3].setButtonAction([this]()
		{
			setSelectedColor(Color::Green);
		});

	m_buttons[4].setButtonAction([this]()
		{
			setSelectedColor(Color::Blue);
		});

	m_buttons[5].setButtonAction([this]()
		{
			setSelectedColor(Color::Yellow);
		});

	m_buttons[6].setButtonAction([this]()
		{
			setSelectedColor(Color::Magenta);
		});

	m_buttons[7].setButtonAction([this]()
		{
			setSelectedColor(Color::Cyan);
		});

	m_buttons[8].setButtonAction([this]()
		{
			m_ActualShape = SHAPE_TYPE::LINE;
		});

	m_buttons[9].setButtonAction([this]()
		{
			m_ActualShape = SHAPE_TYPE::RECTANGLE;
		});

	m_buttons[10].setButtonAction([this]()
		{
			m_ActualShape = SHAPE_TYPE::CIRCLE;
		});

	m_buttons[11].setButtonAction([this]()
		{
			m_ActualShape = SHAPE_TYPE::FREEDRAW;
		});
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
			if (keyboardEvent.key.code == sf::Keyboard::Escape && m_MouseButtonDown) {
				m_MouseButtonDown = false;
				m_PreviewShape.Clear();
			}


			if (keyboardEvent.key.code == sf::Keyboard::Z)
			{
				removeLatestElementAdded();
			}


			break;
		case Event::MouseButtonPressed:
			for (int i = 0; i < BUTTON_COUNT; ++i) {
				if (m_buttons[i].isInBounds(GetMousePosition())) {
					m_buttons[i].onClick();
					return;
				}
			}

			m_MouseButtonDown = true;
			if (m_ActualShape == SHAPE_TYPE::FREEDRAW)
			{
				Vector2f mousePos = GetMousePosition();
				Line tempLine;
				tempLine.initialPos = Vertex(mousePos, m_SelectedColor);
				tempLine.finalPos = Vertex(mousePos, m_SelectedColor);
				tempLine.shapeID = 0;
				m_FreedrawList.push_back(tempLine);
				m_latestElementAdded.push_back(FREEDRAW_SHAPE);
				break;
			}

			m_PreviewShape.CreateShape(GetMousePosition(), m_ActualShape);
			cout << "Shape created at pos " << GetMousePosition().x << ", " << GetMousePosition().y << endl;
			break;
		case Event::MouseButtonReleased:
			if (m_MouseButtonDown) {
				m_MouseButtonDown = false;
				if (m_ActualShape == SHAPE_TYPE::FREEDRAW)
				{
					Vector2f latestVectorPosition = getLatestFreedraw()->initialPos.position;
					Vector2f actualVectorPosition = GetMousePosition();
					getLatestFreedraw()->finalPos.position = actualVectorPosition;
					sendFreedraw(latestVectorPosition, actualVectorPosition);
					break;
				}

				m_PreviewShape.SetColor(m_SelectedColor);
				m_ShapeList.push_back(m_PreviewShape);
				m_latestElementAdded.push_back(DRAWING_SHAPE);
				sendRegularShape();
				m_PreviewShape.Reset();
			}
			break;
		case Event::MouseMoved:
			if (m_MouseButtonDown) {
				if (m_PreviewShape.IsShapeCreated()) {
					m_PreviewShape.Update(GetMousePosition());
					break;
				}

				Vector2f latestVectorPosition = getLatestFreedraw()->initialPos.position;
				Vector2f actualVectorPosition = GetMousePosition();
				if (latestVectorPosition != actualVectorPosition)
				{
					getLatestFreedraw()->finalPos.position = actualVectorPosition;
					Line tempLine;
					tempLine.initialPos = Vertex(actualVectorPosition, m_SelectedColor);
					tempLine.finalPos = Vertex(actualVectorPosition, m_SelectedColor);
					tempLine.shapeID = 0;
					m_FreedrawList.push_back(tempLine);
					m_latestElementAdded.push_back(FREEDRAW_SHAPE);
					sendFreedraw(latestVectorPosition, actualVectorPosition);
					break;
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

	if (m_FreedrawList.size() > 0) {
		for (int i = 0; i < m_FreedrawList.size() - 1; ++i)
		{
			Vertex vertexLine[2] = { m_FreedrawList[i].initialPos, m_FreedrawList[i].finalPos };
			m_Window.draw(vertexLine, 2, sf::PrimitiveType::Lines);
		}
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

		MsgSyncUser message;
		m_client.sendMessage(&message, E::kSYNC_USER);

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
		MsgCreateLine message;
		MsgCreateLine::LineData realData;
		message.unpackData(unpackedData.data(), &realData, unpackedData.size());

		SHAPE_TYPE shpType = SHAPE_TYPE::LINE;

		Drawing sentShape;
		sentShape.CreateShape(Vector2f(realData.initialPosX, realData.initialPosY), shpType);
		sentShape.Update(Vector2f(realData.finalPosX, realData.finalPosY));
		sentShape.SetColor(realData.colorID);
		m_ShapeList.push_back(sentShape);

		std::cout << "Shape sync by server." << endl;
	}
	break;
	case E::kCREATE_RECTANGLE:
	{
		MsgCreateRectangle message;
		MsgCreateRectangle::RectangleData realData;
		message.unpackData(unpackedData.data(), &realData, unpackedData.size());

		SHAPE_TYPE shpType = SHAPE_TYPE::RECTANGLE;

		Drawing sentShape;
		sentShape.CreateShape(Vector2f(realData.initialPosX, realData.initialPosY), shpType);
		sentShape.Update(Vector2f(realData.finalPosX, realData.finalPosY));
		sentShape.SetColor(realData.colorID);
		m_ShapeList.push_back(sentShape);

		std::cout << "Shape sync by server." << endl;
	}
	break;
	case E::kCREATE_CIRCLE:
	{
		MsgCreateCircle message;
		MsgCreateCircle::CircleData realData;
		message.unpackData(unpackedData.data(), &realData, unpackedData.size());

		SHAPE_TYPE shpType = SHAPE_TYPE::CIRCLE;

		Drawing sentShape;
		sentShape.CreateShape(Vector2f(realData.initialPosX, realData.initialPosY), shpType);
		sentShape.Update(Vector2f(realData.finalPosX, realData.finalPosY));
		sentShape.SetColor(realData.colorID);
		m_ShapeList.push_back(sentShape);

		std::cout << "Shape sync by server." << endl;
	}
	break;
	case E::kCREATE_FREEDRAW:
	{
		MsgCreateFreedraw message;
		MsgCreateFreedraw::FreedrawData realData;
		message.unpackData(unpackedData.data(), &realData, unpackedData.size());

		SHAPE_TYPE shpType = SHAPE_TYPE::FREEDRAW;

		Vector2f initialPos(realData.initialPosX, realData.initialPosY);
		Vector2f finalPos(realData.finalPosX, realData.finalPosY);

		Line tempLine;
		tempLine.initialPos = Vertex(initialPos, realData.colorID);
		tempLine.finalPos = Vertex(finalPos, realData.colorID);
		tempLine.shapeID = realData.messageID;
		m_FreedrawList.push_back(tempLine);
		//std::cout << "Shape sync by server." << endl;
	}
	break;
	case E::kUNDO_MESSAGE:
	{
		string realData(unpackedData.data());
		realData.resize(unpackedData.size());

		string undo;
		string idValue;

		stringstream ss(realData);
		ss << undo << idValue;

		uint32 messageID = stoi(idValue);

		removeShapeFromID(messageID);
	}
	break;
	}
}

void Application::sendRegularShape()
{
	Vector2f startingPos = m_PreviewShape.getStartingPos();
	Vector2f finalPos = m_PreviewShape.getFinalPos();
	Color colorID = m_PreviewShape.getColorID();
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
	}
}

void Application::sendFreedraw(const Vector2f& startingPos, const Vector2f& finalPos)
{
	MsgCreateFreedraw msg(startingPos.x, startingPos.y, finalPos.x, finalPos.y, m_SelectedColor);
	m_client.sendMessage(&msg, E::kCREATE_FREEDRAW);
}

void Application::removeLatestElementAdded()
{
	if (m_latestElementAdded.size() < 1)
	{
		return;
	}

	bool latestElement = m_latestElementAdded[m_latestElementAdded.size() - 1];
	if (latestElement == DRAWING_SHAPE)
	{
		m_ShapeList.erase(m_ShapeList.end() - 1);
	}
	else
	{
		m_FreedrawList.erase(m_FreedrawList.end() - 1);
	}
	m_latestElementAdded.erase(m_latestElementAdded.end() - 1);

	MsgUndo msg;
	m_client.sendMessage(&msg, E::kUNDO_MESSAGE);
}

void Application::removeShapeFromID(const uint32& id)
{
	for (auto it = m_ShapeList.rbegin(); it != m_ShapeList.rend(); ++it)
	{
		auto& cl = *it;
		if (cl.getShapeID() == id)
		{
			m_ShapeList.erase((it + 1).base());
			return;
		}
	}

	for (auto it = m_FreedrawList.rbegin(); it != m_FreedrawList.rend(); ++it)
	{
		auto& cl = *it;
		if (cl.shapeID == id)
		{
			m_FreedrawList.erase((it + 1).base());
			return;
		}
	}
}

