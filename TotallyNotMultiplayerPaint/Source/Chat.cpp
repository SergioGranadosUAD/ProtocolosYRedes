#include "../Include/Chat.h"

Chat::Chat() :
	m_isActive(false)
{
	m_background.setPosition(Vector2f(20, 530));
	m_background.setSize(Vector2f(760, 50));
	m_background.setFillColor(sf::Color(0, 0, 0, 200));
	m_background.setOutlineColor(sf::Color(255, 255, 255, 200));
	m_background.setOutlineThickness(1.0f);
	//m_background.setFillColor(sf::Color::White);

	m_textFont.loadFromFile("Resources/FreePixel.ttf");

	m_inputText = new Text(m_textFont);
	m_inputText->setPosition(Vector2f(20, 530));
	m_inputText->setFillColor(sf::Color::White);
	m_inputText->setOutlineColor(sf::Color::White);
}

Chat::~Chat()
{
	if (m_inputText != nullptr)
	{
		delete m_inputText;
	}
}

void Chat::render(RenderWindow& window)
{
	if (!m_isActive)
	{
		return;
	}

	window.draw(m_background);
	window.draw(*m_inputText);
}

void Chat::addInputText(char32_t text)
{
	if (!m_isActive)
	{
		return;
	}

	m_inputString += text;
	m_inputText->setString(m_inputString);
}

void Chat::removeLatestCharacter()
{
	if (m_inputString.isEmpty() || !m_isActive)
	{
		return;
	}

	m_inputString.erase(m_inputString.getSize() - 1);
	m_inputText->setString(m_inputString);
}

void Chat::toggleActive()
{
	if (m_isActive)
	{
		m_isActive = false;
		m_inputString.clear();
		m_inputText->setString(m_inputString);
	}
	else {
		m_isActive = true;
	}
}
