#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <chrono>

void ConnectUdpClient();
void SetupUdpServer();
sf::IpAddress GetIP(std::string ipString);

int main()
{
	std::cout << "Escriba 'c' para conectarse como cliente, o 's' para iniciar un servidor." << std::endl;
	char c;
	std::cin >> c;
	if (c == 'c') {
		ConnectUdpClient();
	}
	else if (c == 's') {
		std::cout << "Se ha iniciado el servidor." << std::endl;
		SetupUdpServer();
	}
	else {
		std::cout << "No se introdujo un valor valido." << std::endl;
	}
	

	
}

void ConnectUdpClient() {
	sf::UdpSocket socket;
	socket.setBlocking(false);
	std::optional<sf::IpAddress> serverIP = GetIP("192.168.100.31");
	unsigned short serverPort = 25565;

	char recData[50];
	std::size_t received;
	std::optional<sf::IpAddress> senderIP;
	unsigned short senderPort;

	sf::Clock cooldownClock;
	sf::Clock delayClock;
	sf::Clock timeoutClock;
	sf::Time cooldownTime;
	sf::Time delayTime;
	sf::Time timeoutTime;

	//Game loop
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600), 32), "Ping test");
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		//Se manda un ping cada dos segundos.
		cooldownTime = cooldownClock.getElapsedTime();
		if (cooldownTime.asMilliseconds() > 2000) {
			cooldownClock.restart();
			delayClock.restart();


			char data[] = "Ping";
			if (socket.send(data, sizeof(data), serverIP.value(), serverPort) != sf::Socket::Status::Done) {
				std::cout << "Could not send the message." << std::endl;
			}
			else {
				std::cout << "Ping sent to server." << std::endl;
			}
		}

		//Se revisa si se recibió un mensaje.
		if (socket.receive(recData, 50, received, senderIP, senderPort) != sf::Socket::Status::Done) {
			//std::cout << "Could not receive the information." << std::endl;
		}
		else {
			timeoutClock.restart();
			delayTime = delayClock.getElapsedTime();
			std::cout << "Received ping from server. Delay time: " << delayTime.asMilliseconds() << " ms." << std::endl;
		}

		//Si lleva 10 segundos sin recibir respuesta del servidor, se cierra el cliente.
		timeoutTime = timeoutClock.getElapsedTime();
		if (timeoutTime.asMilliseconds() > 10000) {
			std::cout << "Client timeout. Disconnecting." << std::endl;
			window.close();
		}

		window.clear();
		window.display();

	}
}

void SetupUdpServer() {
	bool clientConnected = false;
	//Se crea el socket y se bindea.
	sf::UdpSocket socket;
	if (socket.bind(25565) != sf::Socket::Status::Done) {
		std::cout << "Could not bind the socket to port 25565." << std::endl;
	}

	char recData[50];
	std::size_t received;
	std::optional<sf::IpAddress> clientIP;
	unsigned short clientPort;

	//Se espera a recibir un mensaje inicial para saber que el cliente se conectó.
	if (socket.receive(recData, 50, received, clientIP, clientPort) == sf::Socket::Status::Done) {
		std::cout << "Client connected." << std::endl;
		clientConnected = true;
	}

	socket.setBlocking(false);
	sf::Clock timeoutClock;
	sf::Time timeoutTime;

	while (clientConnected) {
		//Se espera recibir mensaje por parte de un cliente.
		if (socket.receive(recData, 50, received, clientIP, clientPort) == sf::Socket::Status::Done) {
			std::cout << "Ping from client. " << recData << std::endl;

			//Mensaje recibido, envía una respuesta de vuelta al cliente.
			char data[] = "Ping.";
			if (socket.send(data, sizeof(data), clientIP.value(), clientPort) != sf::Socket::Status::Done) {
				//std::cout << "Could not send the message." << std::endl;
			}
			else {
				timeoutClock.restart();
				std::cout << "Pinged client back." << std::endl;
			}
		}
		else {
			//std::cout << "Could not receive the message." << std::endl;
		}

		timeoutTime = timeoutClock.getElapsedTime();
		if (timeoutTime.asMilliseconds() > 10000) {
			std::cout << "Client timeout. Disconnecting." << std::endl;
			clientConnected = false;
		}
	}
}

sf::IpAddress GetIP(std::string ipString) {
	std::stringstream stream(ipString);
	int bytes[4];
	char dot;
	for (int i = 0; i < 4; ++i) {
		stream >> bytes[i] >> dot;
	}
	return sf::IpAddress(bytes[0], bytes[1], bytes[2], bytes[3]);
}

