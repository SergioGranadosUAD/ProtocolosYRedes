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
	socket.setBlocking(true);
	sf::IpAddress clientIP = ParseIP("192.168.100.31");
	std::optional<sf::IpAddress> serverIP = ParseIP("192.168.100.31");

	char data[5] = "Hola";
	if (socket.send(data, 5, clientIP, 25565) != sf::Socket::Status::Done) {
		std::cout << "No se pudo enviar el mensaje" << std::endl;
	}
	else {
		std::cout << "Message sent:" << data << std::endl;
	}

	unsigned short serverPort = 0;
	char recData[14];
	std::size_t received;
	if (socket.receive(recData, 14, received, serverIP, serverPort) != sf::Socket::Status::Done) {
		std::cout << "No se pudo recibir la informacion" << std::endl;
	}
	else {
		std::cout << "Message received: " << recData << std::endl;
	}
	/*
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
	*/
}

void SetupUdpServer() {
	sf::UdpSocket socket;
	unsigned short clientPort = 25564;
	sf::IpAddress serverIP = ParseIP("192.168.100.31");
	std::optional<sf::IpAddress> clientIP = ParseIP("192.168.100.31");
	if (socket.bind(25565) != sf::Socket::Status::Done) {
		std::cout << "No se pudo bindear el socket al puerto 25565." << std::endl;
	}
	else {

	}

	socket.setBlocking(true);
	char recData[100];
	std::size_t received;
	if (socket.receive(recData, 100, received, clientIP, clientPort) == sf::Socket::Status::Done) {
		std::cout << "Se ha recibido el mensaje: " << recData << std::endl;
		
	}

	char data[14] = "Hola cliente.";
	if (socket.send(data, 14, clientIP.value(), clientPort) != sf::Socket::Status::Done) {
		std::cout << "No se pudo enviar el mensaje" << std::endl;
	}
	else {
		std::cout << "Se envio el mensaje de vuelta: " << data << std::endl;
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

