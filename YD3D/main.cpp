#include "YD3D_Header.h"
#include "TestWindow.h"

using namespace YD3D;

int main() 
{
	garbage_collection::start_up(2);

	TestWindow win;
	win.Create(800, 600, L"hello");

	return win.Run();
}

