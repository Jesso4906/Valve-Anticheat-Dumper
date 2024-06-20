#include "dllmain.h"

LoadVACModuleType gateway = nullptr;

std::vector<DWORD> foundModuleSizes;

DWORD WINAPI Thread(LPVOID param)
{
	uintptr_t steamServiceDll = (uintptr_t)GetModuleHandleA("SteamService.dll");
	if(steamServiceDll == 0)
	{
		FreeLibraryAndExitThread((HMODULE)param, 0);
		return 0;
	}

	uintptr_t loadVacModuleFuncPtr = FindArrayOfBytes(steamServiceDll, (BYTE*)"\x55\x8B\xEC\x83\xEC\x24\x53\x56\x8B\x75\x08\x8B\xD9", 13, 0x01);
	if (loadVacModuleFuncPtr == 0)
	{
		FreeLibraryAndExitThread((HMODULE)param, 0);
		return 0;
	}

	BYTE originalBytes[6];
	SetBytes(originalBytes, (BYTE*)loadVacModuleFuncPtr, 6);

	gateway = (LoadVACModuleType)TrampolineHook((void*)loadVacModuleFuncPtr, HookedLoadVACModule, 6, false);

	FreeLibraryAndExitThread((HMODULE)param, 0);
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) 
	{ 
		CreateThread(0, 0, Thread, hModule, 0, 0); 
	}
	
	return TRUE;
}

int moduleNumber = 1;

void __stdcall HookedLoadVACModule(uintptr_t* VACModule, char flags)
{
	if (VACModule[5] != 0 && VACModule[6] != 0)
	{
		for (int i = 0; i < foundModuleSizes.size(); i++) 
		{
			if (foundModuleSizes[i] == VACModule[5]) 
			{
				return gateway(VACModule, flags);
			}
		}
		
		CreateDirectoryA("C:\\VACDumps", NULL);

		std::ofstream file("C:\\VACDumps\\vacmodule" + std::to_string(moduleNumber) + ".dll", std::ios::binary | std::ios::out);
		file.write((const char*)VACModule[6], VACModule[5]);
		file.close();

		moduleNumber++;

		foundModuleSizes.push_back(VACModule[5]);
	}

	gateway(VACModule, flags);
}