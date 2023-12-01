#include "Engine.h"

int main(int argc, char *argv[]) 
{
	IceEngine::Engine::Instance().Start();

	// Main Loop
	bool quit = false;

	while (!quit)
	{
		IceEngine::Engine::Instance().Run();
	}

	IceEngine::Engine::Instance().Shutdown();
	
	return (0);
}
