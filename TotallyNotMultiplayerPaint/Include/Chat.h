#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
#include <string>

using std::string;
using std::vector;
using sf::Text;
using sf::Clock;
using sf::RectangleShape;
using sf::Font;
using sf::RenderWindow;
using sf::Vector2f;
using sf::String;

//struct chatMessage
//{
//	Text messageText;
//	Clock messageTimer;
//};

class Chat
{
public:
	Chat();
	~Chat();

	//void update();
	void render(RenderWindow& window);

	void addInputText(char32_t text);
	void removeLatestCharacter();
	//void addText(string addedText);
	void toggleActive();
	bool isActive() { return m_isActive; };
	bool isEmpty() { return m_inputString.isEmpty(); };
	string getText() { return m_inputString; };

private:
	//vector<chatMessage> m_textPool;
	String m_inputString;
	Text* m_inputText;
	bool m_isActive;

	RectangleShape m_background;
	Font m_textFont;
};