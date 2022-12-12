#include "Application.h"

int main()
{
	Application* app = new Application(120, 40);
	app->Run();
	delete app;

	return 0;
}