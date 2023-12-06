#include "Engine.h"
#include "box2d/box2d.h"

int main(int argc, char *argv[]) 
{
	IceEngine::Engine::Instance().Start();

	// Main Loop

	while (IceEngine::Engine::Instance().m_running)
	{
		IceEngine::Engine::Instance().Run();
	}

	IceEngine::Engine::Instance().Shutdown();
	
	return (0);
}
