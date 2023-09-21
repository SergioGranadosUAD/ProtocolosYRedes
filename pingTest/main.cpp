#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <chrono>

void ConnectUdpClient();
void SetupUdpServer();
sf::IpAddress ParseIP(std::string ipString);

int main()
{
	std::cout << "Escriba 'c' para conectarse como cliente, o 's' para iniciar un servidor." << std::endl;
	char c;
	std::cin >> c;
	if (c == 'c') {
		ConnectUdpClient();
	}
	else if (c == 's') {
		SetupUdpServer();
	}
	else {
		std::cout << "No se introdujo un valor valido." << std::endl;
	}
	

	
}

void ConnectUdpClient() {
	sf::UdpSocket socket;
	socket.setBlocking(false);
	std::optional<sf::IpAddress> serverIP = ParseIP("192.168.100.31");
	unsigned short serverPort = 25565;

	char recData[50];
	std::size_t received;
	std::optional<sf::IpAddress> senderIP;
	unsigned short senderPort;

	sf::Clock pingClock;
	sf::Clock delayClock;
	sf::Clock timeoutClock;
	sf::Time pingTime;
	sf::Time delayTime;

	//Game loop
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600), 32), "Ping test");
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		pingTime = pingClock.getElapsedTime();
		if (pingTime.asMilliseconds() > 2000) {
			pingClock.restart();
			delayClock.restart();


			char data[] = "Ping";
			if (socket.send(data, sizeof(data), serverIP.value(), serverPort) != sf::Socket::Status::Done) {
				std::cout << "Could not send the message." << std::endl;
			}
			else {
				std::cout << "Ping sent to server." << std::endl;
			}
		}

		if (socket.receive(recData, 50, received, senderIP, senderPort) != sf::Socket::Status::Done) {
			//std::cout << "Could not receive the information." << std::endl;
		}
		else {
			delayTime = delayClock.getElapsedTime();
			std::cout << "Received ping from server. Delay time: " << delayTime.asMilliseconds() << " ms." << std::endl;
		}

		window.clear();
		window.display();

	}
}

void SetupUdpServer() {
	bool clientConnected = true;
	//Se crea el socket y se bindea.
	sf::UdpSocket socket;
	if (socket.bind(25565) != sf::Socket::Status::Done) {
		std::cout << "Could not bind the socket to port 25565." << std::endl;
	}

	char recData[50];
	std::size_t received;
	std::optional<sf::IpAddress> clientIP;
	unsigned short clientPort;

	while (clientConnected) {
		//Se espera recibir mensaje por parte de un cliente.
		if (socket.receive(recData, 50, received, clientIP, clientPort) == sf::Socket::Status::Done) {
			std::cout << "Ping from client. " << recData << std::endl;

			//Mensaje recibido, envía una respuesta de vuelta al cliente.
			char data[] = "Ping.";
			if (socket.send(data, sizeof(data), clientIP.value(), clientPort) != sf::Socket::Status::Done) {
				std::cout << "Could not send the message." << std::endl;
				return;
			}
			else {
				std::cout << "Pinged client back." << std::endl;
			}
		}
		else {
			std::cout << "Could not receive the message." << std::endl;
			return;
		}
	}
}

sf::IpAddress ParseIP(std::string ipString) {
	std::stringstream stream(ipString);
	int bytes[4];
	char dot;
	for (int i = 0; i < 4; ++i) {
		stream >> bytes[i] >> dot;
	}
	return sf::IpAddress(bytes[0], bytes[1], bytes[2], bytes[3]);
}

