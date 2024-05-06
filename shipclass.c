/*
  shipclass.c - Define additional ship types.

  Jason Hood, 24 December, 2009.

  Adds more categories to the [Ship] ship_class value in
  DATA\SHIPS\shiparch.ini.

	ship_class     resource 	text
	    0		 923		Light Fighter
	    1		 924		Heavy Fighter
	    2		 925		Freighter
	    3		 926		Very Heavy Fighter
	    4		 r+0		Gunboat
	    5		 r+1		Cruiser
	    6		 r+2		Destroyer
	    7		 r+3		Battleship
	    8		 r+4		Capital
	    9		 r+5		Transport
	   10		 r+6		Large Transport
	   11		 r+7		Train
	   12		 r+8		Large Train
	   13		 r+9
	   14		 r+10
	   15		 r+11
	   16		 r+12
	   17		 r+13
	   18		 r+14
	   19		 r+15

  Build (VC6):
	rc shipclass.rc
	cl /nologo /W3 /Ox /Gf /MD /LD shipclass.c shipclass.res
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define NAKED	__declspec(naked)
#define STDCALL __stdcall


// Replace the table address with our own.
#define ADDR_SHIPCLASS1 ((PDWORD)(0x4b79ca+3))
#define ADDR_SHIPCLASS2 ((PDWORD)(0x4b808d+3))
#define ADDR_SHIPCLASS3 ((PDWORD)(0x4b8be7+3))
#define ADDR_SHIPCLASS4 ((PDWORD)(0x4b8fd2+3))
#define ADDR_SHIPCLASS5 ((PDWORD)(0x4b95b1+3))

// Increase the count.
#define ADDR_SHIPCLASSN1 ((PBYTE)(0x4b7682+2))
#define ADDR_SHIPCLASSN2 ((PBYTE)(0x4b807c+2))

// Add this DLL to [Resources].
#define ADDR_RSRC	((PDWORD)(0x5b1caa+1))


DWORD dummy;
#define ProtectX( addr, size ) \
  VirtualProtect( addr, size, PAGE_EXECUTE_READWRITE, &dummy );

#define RELOFS( from, to ) \
  *(PDWORD)(from) = (DWORD)(to) - (DWORD)(from) - 4;


HMODULE g_hinst;
DWORD	old_resource;


UINT ShipClassTable[] =
{
  // Original values, copied from Freelancer.exe.
  923, 924, 925, 926,
  // These values get updated with our resource identifier.
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};


void STDCALL UpdateResources( UINT rsrcid )
{
  int i;

  for (i = 4; i < 20; ++i)
    ShipClassTable[i] += rsrcid;
}


NAKED
void Resource_Hook()
{
  __asm push	[esp+4]
  __asm call	old_resource
  __asm add	esp, 4

  __asm mov	eax, ds:[0x67c404]	// get the number of our resource
  __asm sub	eax, ds:[0x67c400]
  __asm shl	eax, 14
  __asm push	eax
  __asm call	UpdateResources

  __asm mov	eax, 0x57d800
  __asm push	g_hinst
  __asm call	eax
  __asm add	esp, 4
  __asm ret
}


void Patch()
{
  ProtectX( ADDR_SHIPCLASSN1, 1 );
  //ProtectX( ADDR_SHIPCLASS1, 4 );
  ProtectX( ADDR_SHIPCLASSN2, 1 );
  //ProtectX( ADDR_SHIPCLASS2, 4 );
  //ProtectX( ADDR_SHIPCLASS3, 4 );
  //ProtectX( ADDR_SHIPCLASS4, 4 );
  ProtectX( ADDR_SHIPCLASS5, 4 );
  ProtectX( ADDR_RSRC,	     4 );

  *ADDR_SHIPCLASS1 =
  *ADDR_SHIPCLASS2 =
  *ADDR_SHIPCLASS3 =
  *ADDR_SHIPCLASS4 =
  *ADDR_SHIPCLASS5 = (DWORD)ShipClassTable;

  *ADDR_SHIPCLASSN1 =
  *ADDR_SHIPCLASSN2 = 20;

  // Hook in code to load ourself as a resource DLL.
  old_resource = *ADDR_RSRC + (DWORD)ADDR_RSRC + 4;
  RELOFS( ADDR_RSRC, Resource_Hook );
}


BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
  if (fdwReason == DLL_PROCESS_ATTACH)
  {
    g_hinst = hinstDLL;
    Patch();
  }

  return TRUE;
}
