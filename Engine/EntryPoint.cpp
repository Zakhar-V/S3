#include "Engine.hpp"

//----------------------------------------------------------------------------//
// ENTRY POINT
//----------------------------------------------------------------------------//

#if 1
#include <Windows.h>
extern int APIENTRY wWinMain(_In_ HINSTANCE instance_handle,
	_In_opt_ HINSTANCE prev_instance_handle,
	_In_ LPWSTR command_line,
	_In_ int cmd_show);

void main(void)
{
	wWinMain(GetModuleHandle(nullptr), nullptr, nullptr, SW_NORMAL);
}
#endif

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

