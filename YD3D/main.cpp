#include "YD3D_Header.h"
#include "Helper/WICImage.h"
#include "TestWindow.h"
#include "Helper/UploadHelper.h"

using namespace YD3D;

int main() 
{
	garbage_collection::start_up(1);
	YD3D::WICImage::Initialize();
	

	TestWindow win;
	win.Create(800, 600, L"hello");

	return win.Run();
}

