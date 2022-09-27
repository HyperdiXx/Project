#include <iostream>

#include "ewnd.h"

using namespace EProject;

int main()
{
	std::wstring winName = L"GameWindow";
	GameWindow mainWindow(winName);

	MessageLoop([&mainWindow]()
	{
			bool isPainted = false;
			mainWindow.paint(&isPainted);
	});

	return 0;
}