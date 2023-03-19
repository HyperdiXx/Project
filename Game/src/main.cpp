#include "ewnd.h"

using namespace EProject;

int main()
{
	std::wstring winName = L"GameWindow";
	GameWindow mainWindow(winName);

	MessageLoop([&mainWindow]()
	{
		bool isPainted = false;
		mainWindow.execute(&isPainted);
	});

	return 0;
}