#include <iostream>
#include <Windows.h>
#include <vector>

#include "SmartString.hpp"

#define Log(Arg, ...) printf(Arg, __VA_ARGS__)
static HMODULE DllModule = 0;

template <typename T>
bool Write(DWORD32 Address, T Value)
{
    if (Address != 0)
    {
        *reinterpret_cast<DWORD32*>(Address) = Value;
        return true;
    }
    
    return false;
}

template <typename T>
T Read(DWORD32 Address)
{
    return *reinterpret_cast<DWORD32*>(Address); /* Dereference our ptr (with the given address) then return the value */
}

DWORD32 GetRelativeAddress(DWORD32 Address, std::vector<DWORD32> Pointers)
{
    if (!Address)
        throw std::runtime_error("DWORD32 Module handle returned: NULL");

    DWORD32 Result = (DWORD32)(GetModuleHandleA("GameAssembly.dll")) + Address;

    for (DWORD32 i = 0; i < Pointers.size(); i++)
    {
        Log("GetRelativeAddress (Result): 0x%llx\n", Result);

        Result += *reinterpret_cast<DWORD32*>(Result);
        Result += Pointers[i];
    }

    Log("> GetRelativeAddress (Final Result): 0x%llx\n", Result);

    return Result;
}

ULONG __stdcall MainThread(void* CalledParam)
{
    /* Allocate the console..*/

    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

    printf((SmartString("CalledParam: 0x%llx\n", CalledParam)).Get().c_str());

    /* Check if we unload the DLL ( Dynamic Link Library )*/

    static bool speedHack = false;

    do
    {
        if (GetAsyncKeyState(VK_F1) & true)
        {
            speedHack = !speedHack;
        }

        /* Dereference our pointers to get the RVA ( Relative virtual address ) for the speed hack.. */
        /* 
            You maybe be asking.. What is 0x01BA9CE4? This is the "pointer" to the localPlayer class.
            Getting this pointer isn't a challange using cheat engine :) or Reclass.net.
        */
        auto speedHackAddress = GetRelativeAddress(0x01BA9CE4, { 0x5C, 0x4, 0x14 });
        if (speedHackAddress)
        {
            /* Check our current player speed.. */
            float currentSpeed = ( Read<float>(speedHackAddress) > 25 ? 1.0f : Read<float>(speedHackAddress) );

            if (speedHack)
            {
                /* Write the value of our speed to 15 */
                Write<float>(speedHackAddress, 15);
            }
            else
            {
                /* Reset the speed to the original value.. */
                Write<float>(speedHackAddress, currentSpeed);
            }
        }
        else
        {
            MessageBoxA(0, SmartString("The hack needs to be updated. Tough story man.. Address returned: 0x%llx\n", speedHackAddress).Get().c_str(), "", MB_OK);
            FreeLibraryAndExitThread(DllModule, 0);
        }
    } while (!GetAsyncKeyState(VK_END) & true);

    /* Cleanup the console. */
    fclose((FILE*)stdout);
    FreeConsole();

    /* Unload the DLL. */
    FreeLibraryAndExitThread(DllModule, 0);

    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason,LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        /* Set our global static variable so we can unload the dll later on lol*/
        DllModule = hModule;

        /* Make a valid pointer to our thread object, once the thread is created we close the handle to it.*/
        HANDLE ThreadObject = CreateThread(0, 0, static_cast<PTHREAD_START_ROUTINE>(MainThread), lpReserved, 0, 0);
        if (ThreadObject != INVALID_HANDLE_VALUE) /* Check if the handle is even valid.. */
        {
            CloseHandle(ThreadObject);
        }
    }
    return TRUE;
}

