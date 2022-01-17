/* l4d2trainer.cpp
 * Brian Jopling, 2022
 * Description: A simple trainer for L4D2 written in C++ that modifies assembly instructions loaded into memory to provide cheats like infinite ammo and infinite health.
 * Static offsets from base address were discovered using tools like ReClass, Cheat Engine, & IDA Pro.
 */

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include "mem.h"


/***********
 * GLOBALS *
 ***********/

const wchar_t* procName = L"left4dead2.exe";
const wchar_t* moduleName = L"server.dll";

const int BASE_TO_DEC_AMMO_INSTR = 0x3E5860;
const int NUM_BYTES_DEC_AMMO_INSTR = 6;

const int BASE_TO_DEC_HEALTH_INSTR = 0x41E08;
const int NUM_BYTES_DEC_HEALTH_INSTR = 2;

bool isInfAmmo = false;
bool isInfHealth = false;



/*************
 * FUNCTIONS *
 *************/

void enableInfiniteAmmo(HANDLE hProc, uintptr_t baseModuleAddr) {
	uintptr_t addr = baseModuleAddr + BASE_TO_DEC_AMMO_INSTR;
	setNops(hProc, addr, NUM_BYTES_DEC_AMMO_INSTR);
	isInfAmmo = true;
	printf("[*] Infinite ammo enabled!\n");
}

void disableInfiniteAmmo(HANDLE hProc, uintptr_t baseModuleAddr) {
	uintptr_t addr = baseModuleAddr + BASE_TO_DEC_AMMO_INSTR;
	BYTE origInstr[NUM_BYTES_DEC_AMMO_INSTR] = { 0x89, 0xBE, 0x14, 0x14, 0x00, 0x00 };
	setInstruction(hProc, addr, origInstr, NUM_BYTES_DEC_AMMO_INSTR);
	isInfAmmo = false;
	printf("[*] Infinite ammo disabled!\n");
}


void enableInfiniteHealth(HANDLE hProc, uintptr_t baseModuleAddr) {
	uintptr_t addr = baseModuleAddr + BASE_TO_DEC_HEALTH_INSTR;
	setNops(hProc, addr, NUM_BYTES_DEC_HEALTH_INSTR);
	isInfHealth = true;
	printf("[*] Infinite health enabled!\n");
}

void disableInfiniteHealth(HANDLE hProc, uintptr_t baseModuleAddr) {
	uintptr_t addr = baseModuleAddr + BASE_TO_DEC_HEALTH_INSTR;
	BYTE origInstr[NUM_BYTES_DEC_HEALTH_INSTR] = { 0x89, 0x37 };
	setInstruction(hProc, addr, origInstr, NUM_BYTES_DEC_HEALTH_INSTR);
	isInfHealth = false;
	printf("[*] Infinite health disabled!\n");
}


/**************
 *    MAIN    *
 **************/

int main() {
	printf("--- Polyatomic L4D2 Trainer ----\n");
	wprintf(L"[+] Starting...\n[+] Searching for process named %s...\n", procName);
	uintptr_t procId = getProcId(procName);
	wprintf(L"[*] Process found!\n[+] Searching for module named %s...\n", moduleName);
	uintptr_t baseModuleAddr = getModuleBaseAddress(procId, moduleName);
	printf("[*] Module found!\n\n");
	HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, FALSE, procId);

	printf("Press Z to toggle infinite ammo on/off\n");
	printf("Press X to toggle infinite health on/off\n");
	while (1) {

		if (GetAsyncKeyState(0x5A) & 0x1) {
			if (!isInfAmmo) {
				enableInfiniteAmmo(hProc, baseModuleAddr);
			}
			else {
				disableInfiniteAmmo(hProc, baseModuleAddr);
			}
		}

		if (GetAsyncKeyState(0x58) & 0x1) {
			if (!isInfHealth) {
				enableInfiniteHealth(hProc, baseModuleAddr);
			}
			else {
				disableInfiniteHealth(hProc, baseModuleAddr);
			}
		}
		Sleep(10);
	}

}