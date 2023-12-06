#include "Include/Application.h"
#include "Include/NetworkServer.h"

using std::cout;
using std::cin;
using std::endl;

int main() {
	//Programa placeholder en lo que se crea la interfaz gráfica. 

	string input;

	cout << "Escriba 'c' para abrir un cliente o 's' para crear un servidor." << endl;
	cin >> input;

	if (input == "c") 
	{
		Application* app = nullptr;
		app = new Application;
		while (app->isRunning()) {
			app->HandleInput();
			app->Update();
			app->Render();
		}
		cout << "Program finished" << endl;
		delete app;
	}
	else if (input == "s") 
	{
		cout << "Se ha iniciado el servidor en el puerto 25565" << endl;
		NetworkServer* server = nullptr;
		server = new NetworkServer;
		while (server->isRunning()) {
			server->waitForMessage();
			if (server->getPingCooldownTime() > 1000) 
			{
				server->sendPing();
				server->checkForTimeout();
			}
		}
		delete server;
	}
	else {
		cout << "Se introdujo un valor invalido. Terminado programa." << endl;
		return -1;
	}

	
	
}

//////////////////////////////////
//	Estructura original de main.
//////////////////////////////////
/*Application* app = nullptr;
app = new Application;
while (app->isRunning()) {
	app->HandleInput();
	app->Update();
	app->Render();
}*/