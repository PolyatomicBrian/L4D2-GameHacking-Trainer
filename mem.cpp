/* mem.cpp
 * Brian Jopling, 2022
 */


#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include "mem.h"


uintptr_t getProcId(const wchar_t* procName) {
	PROCESSENTRY32 lppe;
	lppe.dwSize = sizeof(PROCESSENTRY32);

	while (wcscmp(lppe.szExeFile, procName) != 0) {
		HANDLE hSnap = CreateToolhelp32Snapshot(
			TH32CS_SNAPPROCESS,
			NULL
		);
		
		while (wcscmp(lppe.szExeFile, procName) != 0) {
			if (!Process32Next(hSnap, &lppe)) {
				break;
			}
		}
		CloseHandle(hSnap);
		Sleep(100);
	}
	return lppe.th32ProcessID;
}


uintptr_t getModuleBaseAddress(uintptr_t procId, const wchar_t* moduleName) {
	MODULEENTRY32 lppe;
	lppe.dwSize = sizeof(MODULEENTRY32);

	while (wcscmp(lppe.szModule, moduleName) != 0) {
		HANDLE hSnap = CreateToolhelp32Snapshot(
			TH32CS_SNAPMODULE,
			procId
		);

		while (wcscmp(lppe.szModule, moduleName) != 0) {
			if (!Module32Next(hSnap, &lppe)) {
				break;
			}
		}
		CloseHandle(hSnap);
		Sleep(100);
	}
	return (uintptr_t)lppe.modBaseAddr;
}


uintptr_t getNestedPointerAddress(HANDLE hProc, uintptr_t addr, std::vector<int> offsets) {
	for (int i = 0; i < offsets.size(); i++) {
		ReadProcessMemory(hProc, (LPCVOID)addr, &addr, sizeof(uintptr_t), 0);
		addr = addr + offsets[i];
	}
	return addr;
}


void setNops(HANDLE hProc, uintptr_t addr, int numBytes) {
	BYTE* nops = new BYTE[numBytes];
	for (int i = 0; i < numBytes; i++) {
		nops[i] = 0x90;
	}
	WriteProcessMemory(hProc, (LPVOID)addr, nops, numBytes, 0);
}


void setInstruction(HANDLE hProc, uintptr_t addr, BYTE* origInstr, int numBytes) {
	WriteProcessMemory(hProc, (LPVOID)addr, origInstr, numBytes, 0);
}