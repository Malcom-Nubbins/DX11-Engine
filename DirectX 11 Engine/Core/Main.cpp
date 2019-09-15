#include "Globals/stdafx.h"
#include "Timer.h"
#include "Application.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Timer _appTimer;
	_appTimer.Reset();

	Application* _fypApplication = new Application(_appTimer);
	if (FAILED(_fypApplication->InitialiseApplication(hInstance, nCmdShow)))
	{
		return -1;
	}

	MSG msg = { 0 };

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);	
		}
		else
		{
			_appTimer.Tick();
			if (!_appTimer.IsScenePaused())
			{
				_fypApplication->Update(_appTimer.DeltaTime());
				_fypApplication->Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	_fypApplication->Cleanup();
	delete _fypApplication;
	_fypApplication = nullptr;

	return (int)msg.wParam;
}