//#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include "Button.h"
#include "Drawing.h"
#include "NetworkClient.h"
#include "NetworkMessage.h"

const int BITS_PER_PIXEL = 32;
const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 600;
const int BUTTON_COUNT = 12;

using std::cout;
using std::cin;
using std::endl;
using std::vector;

using sf::RenderWindow;
using sf::VideoMode;
using sf::Shape;
using sf::RectangleShape;
using sf::CircleShape;
using sf::Event;
using sf::Vector2i;
using sf::Vector2u;
using sf::Vector2f;
using sf::Shape;
using sf::Angle;
using sf::Vertex;
using sf::radians;

struct Line
{
	Vertex initialPos;
	Vertex finalPos;
	uint32 shapeID;
};

class Application {
public:
	Application();
	Application(int windowWidth, int windowHeight);
	virtual ~Application();

	void initButtons();

	void HandleInput();
	void Update();
	void Render();
	
	void handlePackage(Package unpackedData, uint16 msgType);
	void sendRegularShape();
	void sendFreedraw(const Vector2f& startingPos, const Vector2f& finalPos);
	
	//void removeRegularShape(const uint32& id);
	//void removeFreedraw(const uint32& id);

	inline Line* getLatestFreedraw() { return &m_FreedrawList[m_FreedrawList.size() - 1]; };
	bool isRunning() const { return m_Window.isOpen(); };
private:
	RenderWindow m_Window;
	vector<Drawing> m_ShapeList;
	vector<Line> m_FreedrawList;
	uint32 m_drawingIDCount;
	SHAPE_TYPE m_ActualShape;
	Drawing m_PreviewShape;
	bool m_MouseButtonDown;
	Vector2f m_InitialMousePosition;
	Color m_SelectedColor;
	NetworkClient m_client;

	Button m_buttons[BUTTON_COUNT];
	Texture m_buttonsTexture;

	void setSelectedColor(Color setColor);
	Vector2f GetMousePosition() { return m_Window.mapPixelToCoords(sf::Mouse::getPosition(m_Window)); };
};