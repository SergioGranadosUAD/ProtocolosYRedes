#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <chrono>



int main()
{
	std::cout << "Escriba 'c' para conectarse como cliente, o 's' para iniciar un servidor." << std::endl;
	char c;
	std::cin >> c;
	if (c == 'c') {
		ConnectTcpClient();
	}
	else if (c == 's') {
		SetupTcpServer();
	}
	else {
		std::cout << "No se introdujo un valor valido." << std::endl;
	}
	

	
}

void ConnectUdpClient() {


	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600), 32), "Ping test");
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
	}

	window.clear();
	window.display();
}

void SetupTcpServer() {

}

