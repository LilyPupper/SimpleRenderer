#include "Application.h"

int main()
{
	Application* app = new Application(25, 25);
	app->Run();
	delete app;

	return 0;
}