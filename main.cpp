// XXL Inspector: Memory watcher and editor for Asterix XXL 1, 2 and OG
// By AdrienTD

#include "global.h"
#include "ver.h"

typedef IDirect3D9* (WINAPI *ftDirect3DCreate9)(int SDKVersion);

char title[] = "XXL Inspector";
HMODULE d3d9 = 0;
ftDirect3DCreate9 oriDirect3DCreate9;
char *exeep, oldepcode[5];
HINSTANCE exehi;
FILE *logfile = 0;

SGameStartInfo* gameStartInfo;

std::map<int, char*> classNames[15];

std::map<KClass*, char*> lvlObjectNames[32];
std::map<KClass*, char*> gameObjectNames;
uint lvlNumSectors = 0;

bool enableCrateRandomizer = false;

///////////////////////////////////////////


char *getClassName(int grp, int id)
{
	return classNames[grp][id];
}

void __stdcall onKalClassInit(uint _this, uint caller, uint arg0, uint arg4,
  uint arg8, uint argC, uint arg10, uint arg14, uint arg18, uint arg1C)
{
	classNames[arg0][arg4] = (char*)arg1C;
	// fprintf(logfile, "%08X: (%08X)->ckinit(%u, %u, %u, 0x%08X, 0x%08X, 0x%08X, 0x%08X, \"%s\")\n",
	// 	caller, _this, arg0, arg4, arg8, argC, arg10, arg14, arg18, arg1C);
}

naked void jmp_47A274()
{
	__asm {
		pushad
		push [esp+32+4+28]
		push [esp+32+4+28]
		push [esp+32+4+28]
		push [esp+32+4+28]
		push [esp+32+4+28]
		push [esp+32+4+28]
		push [esp+32+4+28]
		push [esp+32+4+28]
		push [esp+32+4+28]
		push esi	// Contains copy of ECX "this" pointer (WRONG! FIX THIS!)
		call onKalClassInit
		popad
		ret 32
	}
}

typedef LRESULT (CALLBACK *ftWndProc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall hookWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr;
	lr = IGWndProc(hWnd, uMsg, wParam, lParam);
	return ((ftWndProc)axWndProc)(hWnd, uMsg, wParam, lParam); // XXL1
}

#if XXLVER >= 2
void __stdcall ReadObjectNames(KFile *file, std::map<KClass*, char*> *objectNames)
{
	CKLoadingManager *lm = (CKLoadingManager *)(yellowPages->loadingManager);
	char scrap[32];
	uint numObjects;
	file->read(&numObjects, 4);
	for(uint o = 0; o < numObjects; o++)
	{
		KClass *obj = 0;
		lm->readRef(file, &obj);
		ushort numChars;
		file->read(&numChars, 2);
		char *name = (char*)malloc(numChars+1);
		file->read(name, numChars);
		name[numChars] = 0;
		// fprintf(logfile, "Object name: %s\n", name);

		wchar_t *widename = new wchar_t[numChars+1];
		MultiByteToWideChar(1252, 0, name, numChars+1, widename, numChars+1);
		int u8len = WideCharToMultiByte(CP_UTF8, 0, widename, numChars+1, NULL, 0, NULL, NULL);
		assert(u8len != 0);
		char *u8name = (char*)malloc(u8len+1);
		WideCharToMultiByte(CP_UTF8, 0, widename, numChars+1, u8name, u8len+1, NULL, NULL);

		(*objectNames)[obj] = u8name;
		file->read(scrap, 32);
	}
}

void __stdcall ReadLevelObjectNames(KFile *file)
{
	for(int i = 0; i < 32 ; i++)
		lvlObjectNames[i].clear();
	uint nameTableOffset, numSectors;
	file->read(&nameTableOffset, 4);
	file->read(&numSectors, 4);
	lvlNumSectors = numSectors;
	for(uint sector = 0; sector < numSectors; sector++)
	{
		ReadObjectNames(file, &(lvlObjectNames[sector]));
	}
}
#endif

//const CKYellowPages* yellowPagesPointer = yellowPages;

#if XXLVER == 2
naked void jmp_40D1BD()
{
	__asm {
		pushad
		push edi
		call ReadLevelObjectNames
		popad
		mov eax, 0x663D04
		mov eax, [eax] // CKYellowPages
		mov ecx, 0x40D1C2
		jmp ecx
	}
}

naked void jmp_40D4A6()
{
	__asm {
		pushad
		push offset gameObjectNames
		push edi
		call ReadObjectNames
		popad
		mov eax, 0x663D04
		mov eax, [eax] // CKYellowPages
		mov ecx, 0x40D4AB
		jmp ecx
	}
}

#elif XXLVER == 4

naked void jmp_40BF4D()
{
	__asm {
		pushad
		push edi
		call ReadLevelObjectNames
		popad
		mov eax, 0x765BF8
		mov eax, [eax] // CKYellowPages
		mov ecx, 0x40BF52
		jmp ecx
	}
}

naked void jmp_40C239()
{
	__asm {
		//int 3
		pushad
		push offset gameObjectNames
		push edi
		call ReadObjectNames
		popad
		mov eax, 0x765BF8
		mov eax, [eax] // CKYellowPages
		mov ecx, 0x40C23E
		jmp ecx
	}
}
#endif

#if XXLVER == 1
naked void jmp_40C1EB()
{
	//__asm mov edi, ebp
	//__asm push ebp
	//__asm mov eax, 

}
#endif

void __stdcall correct_fov(float *fovh, uint32_t width, uint32_t height)
{
	float oldaspect = 4.0f / 3.0f;
	float newaspect = (float)width / (float)height;
	float oldt = tanf(*fovh * M_PI * 0.5f / 180.0f);
	float newt = oldt * newaspect / oldaspect;
	*fovh = atanf(newt) * 2 * 180.0f / M_PI;
}

// Widescreen FOV FIX
naked void asm_fov_fix()
{
	__asm {
		// mov edx, [ecx + 4]
		// fld [esp+4]
		// fimul [edx + 0x0c]	// screen heigth
		// fidiv [edx + 0x10]	// screen width
		// fstp [esp+4]

		mov edx, [ecx+4]
		test edx, edx
		jz nocorrect

		lea eax, [esp+4]
		pushad
		push [edx+0x10]
		push [edx+0x0c]
		push eax
		call correct_fov
		popad

nocorrect:
		mov al, [esp + 8]
		sub esp, 10h
		mov edx, 0x00414b77
		jmp edx
	}
}

// Crate randomizer
void __stdcall mod_crates(uint16_t* data, uint32_t size)
{
	if(!enableCrateRandomizer)
		return;
	uint32_t numCrates = size / 8;
	for(uint32_t i = 0; i < numCrates; i++) {
		data[i*4+3] = rand();
	}
}

naked void asm_crate_mod()
{
	__asm {
		push [esp+4]
		push [esp+4]
		call [eax+0x14]
		call mod_crates
		pop edi
		pop esi
		mov eax, 0x0054269D
		jmp eax
	}
}

void PatchStart_XXL()
{
#if XXLVER == 1
	SetImmediateJump((void*)0x47A274, (uint)jmp_47A274);

	gameStartInfo = (SGameStartInfo*)0x665310;
	*(uint*)(0x479E8E) = (uint)hookWndProc;

	// Remove cursor from window class
	memcpy((void*)0x479EAE, "\x83\xC4\x08\x31\xC0\x90", 6); // add esp, 8 \ xor eax, eax

	// FOV Fix
	//SetImmediateJump((void*)0x00414b70, (uint)asm_fov_fix);

	// Crate randomizer
	SetImmediateJump((void*)0x00542698, (uint)asm_crate_mod);

#elif XXLVER == 2
	SetImmediateJump((void*)0x49B854, (uint)jmp_47A274);

	gameStartInfo = (SGameStartInfo*)0x666DC0;
	*(uint*)(0x49B41F) = (uint)hookWndProc;

	// Read object names in LVL and GAME
	SetImmediateJump((void*)0x40D1BD, (uint)jmp_40D1BD);
	SetImmediateJump((void*)0x40D4A6, (uint)jmp_40D4A6);

	// Remove cursor from window class
	memcpy((void*)0x49B43F, "\x83\xC4\x08\x31\xC0\x90", 6); // add esp, 8 \ xor eax, eax

#elif XXLVER == 4
	SetImmediateJump((void*)0x4DB974, (uint)jmp_47A274);

	gameStartInfo = (SGameStartInfo*)0x7F22A0;
	*(uint*)(0x4DB393) = (uint)hookWndProc;

	// Read object names in LVL and GAME
	SetImmediateJump((void*)0x40BF4D, (uint)jmp_40BF4D);
	SetImmediateJump((void*)0x40C239, (uint)jmp_40C239);

	// Remove cursor from window class
	memcpy((void*)0x4DB3B3, "\x83\xC4\x08\x31\xC0\x90", 6); // add esp, 8 \ xor eax, eax

#endif
}

IDirect3D9 *WINAPI myDirect3DCreate9(int SDKVersion)
{
	wchar_t tbuf[256]; HDirect3D9 *hd; IDirect3D9 *id;
	if(!d3d9)
	{
		if(_access("apd3d9.dll", 0) != -1) // If apd3d9.dll exists.
		{
			d3d9 = LoadLibraryA("apd3d9.dll");
		}
		else
		{
			GetSystemDirectoryW(tbuf, 255);
			wcscat_s(tbuf, 255, L"\\d3d9.dll");
			d3d9 = LoadLibraryW(tbuf);
		}
	}
	oriDirect3DCreate9 = (ftDirect3DCreate9)GetProcAddress(d3d9, "Direct3DCreate9");
	id = oriDirect3DCreate9(SDKVersion);
	if(!id) return 0;
	hd = (HDirect3D9*)malloc(sizeof(HDirect3D9));
	hd->lpVtbl = odmvtbld;
	hd->original = id;
	return (IDirect3D9*)hd;
}

void SetImmediateJump(void *p, uint j)
{
	*(char*)p = 0xE9;
	*(uint*)((char*)p+1) = j - ((uint)p + 5);
}

void SetMemProtection(void *mem, int flags)
{
	MEMORY_BASIC_INFORMATION mbi; DWORD unused;
	VirtualQuery(mem, &mbi, sizeof(mbi));
	VirtualProtect(mem, mbi.RegionSize, (mbi.Protect&0xFFFFFF00) | flags, &unused);
}

int VerifyVersion()
{
	// TODO
	return 1;
}

void ReadSettings()
{
	FILE *f; char li[128], s[64]; int p;
	li[127] = s[63] = 0;
	f = fopen("xxl_inspector_config.txt", "r");
	if(!f) return;
	while(!feof(f))
	{
		fgets(li, 127, f);
		sscanf(li, "%63s %i", s, &p);
		//if(!stricmp(s, "higher_time_precision"))
		//	setting_higher_time_precision = p;
	}
	fclose(f);
}

void PatchStart()
{
	// Verify the version of the game.
	VerifyVersion();

	// Read the settings file (xxl_inspector_config.txt).
	ReadSettings();

	// Open log file.
	//logfile = fopen("xxl_inspector_log.txt", "w");

	MessageBox(0, "XXL Inspector for " XXLNAME " enabled!", title, 64);
	PatchStart_XXL();

	// Restore entry point code.
	memcpy(exeep, oldepcode, 5);

	// Make the .text section back to non-writable for security reasons.
	SetMemProtection((void*)0x401000, PAGE_EXECUTE_READ);
}

naked void EntryPointInterception()
{
	__asm {
		call PatchStart
		mov eax, exeep
		jmp eax
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	char *mz, *pe;
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		// Make the .text section writable.
		SetMemProtection((void*)0x401000, PAGE_EXECUTE_READWRITE);

		// Find the entry point address in the PE header.
		exehi = GetModuleHandle(0);
		mz = (char*)exehi;
		if(*(ushort*)mz != 'ZM') return FALSE;
		pe = mz + *(uint*)(mz+0x3C);
		if(*(uint*)pe != 'EP') return FALSE;
		exeep = (char*)( *(uint*)(pe+0x28) + *(uint*)(pe+0x34) );

		// Save the first 5 bytes of the entry point code.
		memcpy(oldepcode, exeep, 5);

		// Put a jump to our function at the beginning of the entry point code.
		SetImmediateJump(exeep, (uint)EntryPointInterception);
	}
	return TRUE;
}
