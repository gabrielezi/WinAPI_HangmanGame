#include "main.h"

// a sample exported function
void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}
char * DLL_EXPORT generateSecretWord(char * WordArray, int length)
{
    const int MaxL = 30;
    static char Unknown[MaxL];
    int i;

    for (i = 0; i < length; i++)
	{
		Unknown[i]='*';
		if(WordArray[i] == ' ' || WordArray[i] == '\r')
		{
			Unknown[i] = ' ';
		}
	}
	Unknown[i]='\0';
	return Unknown;
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
