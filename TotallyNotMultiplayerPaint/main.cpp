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
		NetworkServer* server = nullptr;
		server = new NetworkServer;
		while (server->isRunning()) {
			server->waitForMessage();
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