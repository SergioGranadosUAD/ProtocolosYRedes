#include "Include/Application.h"

int main() {
	Application* app = nullptr;
	app = new Application;
	while (app->isRunning()) {
		app->HandleInput();
		app->Update();
		app->Render();
	}
}