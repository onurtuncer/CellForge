

#pragma once

#include "Application.h"

extern Hazel::Application* Hazel::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace CellForge {

	int Main(int argc, char** argv)
	{
		while (g_ApplicationRunning)
		{
			InitializeCore();
			Application* app = CreateApplication(argc, argv);
			CF_CORE_ASSERT(app, "Client Application is null!");
			app->Run();
			delete app;
			ShutdownCore();
		}
		return 0;
	}

}

#if CF_DIST && CF_PLATFORM_WINDOWS

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return CellForge::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return CellForge::Main(argc, argv);
}

#endif // CF_DIST