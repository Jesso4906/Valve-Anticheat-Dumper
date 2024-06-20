#include <string>
#include <fstream>
#include "memoryTools.h"

typedef void(__stdcall* LoadVACModuleType)(uintptr_t* VACModule, char flags);

void __stdcall HookedLoadVACModule(uintptr_t* VACModule, char flags);