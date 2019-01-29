// D3DProject.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "D3DProject.h"
#include "ApplicationD3D11.h"

std::unique_ptr<Application> Application::s_pAppInstance = std::make_unique<ApplicationD3D11>();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	
	if (Application::GetApp()->InitApp(hInstance))
	{
		Application::GetApp()->Run();
	}
	return 0;
}
