// HexMap.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "HexMap.h"
#include "ApplicationD3D11.h"

std::shared_ptr<Application> s_pAppInstance = std::make_shared<ApplicationD3D11>();
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

	
	if (Application::GetApp()->InitApp(hInstance))
	{
		Application::GetApp()->Run();
	}
	return 0;
}
