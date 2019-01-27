#include "stdafx.h"
#include "ApplicationD3D11.h"
using namespace std;

unique_ptr<ApplicationBase> s_pAppInstance = std::make_unique<ApplicationD3D11>(_T("Hex-Map"));

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。


	if (ApplicationBase::GetApp()->InitApp(hInstance))
	{
		ApplicationBase::GetApp()->Run();
	}

	return 0;
}
