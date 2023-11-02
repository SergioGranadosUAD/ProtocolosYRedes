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

using std::cout;
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
using sf::radians;

class Application {
public:
	Application();
	Application(int windowWidth, int windowHeight);
	virtual ~Application();

	void HandleInput();
	void Update();
	void Render();
	

	bool isRunning() const { return m_Window.isOpen(); };
private:
	RenderWindow m_Window;
	vector<Drawing> m_ShapeList;
	SHAPE_TYPE m_ActualShape;
	Drawing m_PreviewShape;
	bool m_MouseButtonDown;
	Vector2f m_InitialMousePosition;
	Button m_buttons[4];
	Color m_SelectedColor;
	NetworkClient m_client;

	void setSelectedColor(Color setColor);
	Vector2f GetMousePosition() { return m_Window.mapPixelToCoords(sf::Mouse::getPosition(m_Window)); };
};