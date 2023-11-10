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

struct chatMessage
{
	Text* messageText;
	Clock messageTimer;
};

class Chat
{
public:
	Chat();
	~Chat();

	void update();

	void setInputText(string addedText);
	void addText(string addedText);
	void setActive(bool active);

private:
	vector<chatMessage> m_textPool;
	Text* m_inputText;
	bool m_isActive;

	RectangleShape m_background;
	Font m_textFont;

};