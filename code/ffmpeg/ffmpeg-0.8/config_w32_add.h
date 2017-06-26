
#ifdef CONFIG_WIN32

#include <windows.h>
static void usleep(int x)
{
	Sleep(x/1000);
}

#endif