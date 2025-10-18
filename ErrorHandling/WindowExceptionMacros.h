#pragma once

// error handling exception macro
#define SFWND_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, hr)
#define SFWND_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())	// for windows function without error code
#define SFWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )