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

const wchar_t* procName = L"left4dead2.exe";
const wchar_t* moduleName = L"server.dll";

const int BASE_TO_GLOBAL_ENTITY_LIST = 0x75A9B8;
std::vector<int> OFFSET_ENTITY_LIST_HEALTH = { 0x14, 0xEC };
const int OFFSET_ENTITY_LIST_HEALTH_ITERATOR = 0x10;  // Apply to first value in OFFSET_ENTITY_LIST_HEALTH to cycle through all players.
const int NUM_SURVIVORS = 4;

const int BASE_TO_PLAYER = 0x7DC674;
std::vector<int> OFFSET_HEALTH = { 0xEC };
const int MAX_HEALTH = 200;  // Above 100 prevents Witch from insta-incapping.

const int BASE_TO_DEC_AMMO_INSTR = 0x3E5860;
const int NUM_BYTES_DEC_AMMO_INSTR = 6;

const int BASE_TO_DEC_HEALTH_INSTR = 0x41E08;
const int NUM_BYTES_DEC_HEALTH_INSTR = 2;

bool isInfAmmo = false;		  // Survivors
bool isInfHealth = false;     // Player survivor
bool isInfTeamHealth = false; // Survivors
bool isInfHealthAll = false;  // Special infected & survivors



void freezeTeamHealth(HANDLE hProc, uintptr_t baseModuleAddr) {
	if (!isInfTeamHealth || isInfHealthAll) {
		return;
	}
	for (int i = 0; i < NUM_SURVIVORS; i++) {
		uintptr_t addr = baseModuleAddr + BASE_TO_GLOBAL_ENTITY_LIST;
		std::vector<int> offset_entity_list_health_cur = OFFSET_ENTITY_LIST_HEALTH;
		offset_entity_list_health_cur[0] += i * OFFSET_ENTITY_LIST_HEALTH_ITERATOR;
		addr = getNestedPointerAddress(hProc, addr, offset_entity_list_health_cur);
		WriteProcessMemory(hProc, (LPVOID)addr, &MAX_HEALTH, sizeof(MAX_HEALTH), 0);
	}
}

void freezeHealth(HANDLE hProc, uintptr_t baseModuleAddr) {
	if (!isInfHealth || isInfTeamHealth || isInfHealthAll) {  // If team health enabled, then don't duplicate work.
		return;
	}
	uintptr_t addr = baseModuleAddr + BASE_TO_PLAYER;
	addr = getNestedPointerAddress(hProc, addr, OFFSET_HEALTH);
	WriteProcessMemory(hProc, (LPVOID)addr, &MAX_HEALTH, sizeof(MAX_HEALTH), 0);
}

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


void enableInfiniteHealthAll(HANDLE hProc, uintptr_t baseModuleAddr) {
	uintptr_t addr = baseModuleAddr + BASE_TO_DEC_HEALTH_INSTR;
	setNops(hProc, addr, NUM_BYTES_DEC_HEALTH_INSTR);
	isInfHealthAll = true;
	printf("[*] Infinite team health and hardmode enabled!\n");
}

void disableInfiniteHealthAll(HANDLE hProc, uintptr_t baseModuleAddr) {
	uintptr_t addr = baseModuleAddr + BASE_TO_DEC_HEALTH_INSTR;
	BYTE origInstr[NUM_BYTES_DEC_HEALTH_INSTR] = { 0x89, 0x37 };
	setInstruction(hProc, addr, origInstr, NUM_BYTES_DEC_HEALTH_INSTR);
	isInfHealthAll = false;
	printf("[*] Infinite team health and hardmode disabled!\n");
}


int main() {
	printf("--- Polyatomic L4D2 Trainer ----\n");
	wprintf(L"[+] Starting...\n[+] Searching for process named %s...\n", procName);
	uintptr_t procId = getProcId(procName);
	wprintf(L"[*] Process found!\n[+] Searching for module named %s...\n", moduleName);
	uintptr_t baseModuleAddr = getModuleBaseAddress(procId, moduleName);
	printf("[*] Module found!\n\n");
	HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, FALSE, procId);

	printf("Press Z to toggle infinite team ammo on/off\n");
	printf("Press X to toggle infinite player health on/off\n");
	printf("Press C to toggle infinite team health (survivors) on/off\n");
	printf("Press B to toggle infinite team health... hardmode on/off\n");
	while (1) {

		// Key Z
		if (GetAsyncKeyState(0x5A) & 0x1) {
			if (!isInfAmmo) {
				enableInfiniteAmmo(hProc, baseModuleAddr);
			}
			else {
				disableInfiniteAmmo(hProc, baseModuleAddr);
			}
		}

		// Key X
		if (GetAsyncKeyState(0x58) & 0x1) {
			isInfHealth = !isInfHealth;
			(isInfHealth) ? printf("[*] Infinite player health enabled!\n") : printf("[*] Infinite player health disabled!\n");
		}
		freezeHealth(hProc, baseModuleAddr);

		// Key C
		if (GetAsyncKeyState(0x43) & 0x1) {
			isInfTeamHealth = !isInfTeamHealth;
			(isInfTeamHealth) ? printf("[*] Infinite team health enabled!\n") : printf("[*] Infinite team health disabled!\n");
		}
		freezeTeamHealth(hProc, baseModuleAddr);

		// Key B
		if (GetAsyncKeyState(0x42) & 0x1) {
			if (!isInfHealthAll) {
				enableInfiniteHealthAll(hProc, baseModuleAddr);
			}
			else {
				disableInfiniteHealthAll(hProc, baseModuleAddr);
			}
		}
		Sleep(5);
	}
	

}