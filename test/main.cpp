#include <iostream>
#include <SFML/Network.hpp>
#include <sstream>

void ConnectTcpClient();
void SetupTcpServer();
sf::IpAddress ParseIP(std::string ipString);

int main()
{
    std::cout << "Escriba 'c' para conectarse como cliente, o 's' para iniciar un servidor." << std::endl;
    char c;
    std::cin >> c;
    if (c == 'c') {
        ConnectTcpClient();
    } else if(c == 's'){
        SetupTcpServer();
    }
    else {
        std::cout << "No se introdujo un valor valido." << std::endl;
    }
}

void ConnectTcpClient() {
    sf::TcpSocket socket;
    std::string ipAddress;
    std::cout << "Introduzca la direccion IP a la que desea conectarse." << std::endl;
    std::cin >> ipAddress;
    sf::Socket::Status status = socket.connect(ParseIP(ipAddress), 50000);
    if (status != sf::Socket::Status::Done) {
        std::cout << "Hubo un problema al conectar a la direccion indicada." << std::endl;
        return;
    }
    std::cout << "Se ha establecido conexion con el servidor." << std::endl;

    char sentData[35] = "This is a message from the client.";
    if (socket.send(sentData, 35) != sf::Socket::Status::Done) {
        std::cout << "Error al enviar el mensaje." << std::endl;
    }

    char receivedData[35];
    std::size_t received;

    if (socket.receive(receivedData, 35, received) != sf::Socket::Status::Done) {
        std::cout << "No se pudo recibir el mensaje" << std::endl;
    }

    for (int i = 0; i < 35; ++i) {
        std::cout << receivedData[i];
    }
    std::cout << std::endl;
}

void SetupTcpServer() {
    sf::TcpListener listener;
    if (listener.listen(50000) != sf::Socket::Status::Done) {
        std::cout << "Hubo un problema al bindear el listen port." << std::endl;
        return;
    }

    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Status::Done) {
        std::cout << "No se pudo conectar con el cliente." << std::endl;
        return;
    }

    std::cout << "Un cliente se ha conectado." << std::endl;

    char sentData[35] = "This is a message from the server.";
    if (client.send(sentData, 35) != sf::Socket::Status::Done) {
        std::cout << "Error al enviar el mensaje." << std::endl;
    }

    char receivedData[35];
    std::size_t received;

    if (client.receive(receivedData, 35, received) != sf::Socket::Status::Done) {
        std::cout << "No se pudo recibir el mensaje" << std::endl;
    }

    for (int i = 0; i < 35; ++i) {
        std::cout << receivedData[i];
    }
    std::cout << std::endl;
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