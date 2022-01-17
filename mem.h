/* mem.h
 * Brian Jopling, 2022
 */


#pragma once

uintptr_t getProcId(const wchar_t* procName);
uintptr_t getModuleBaseAddress(uintptr_t procId, const wchar_t* moduleName);
uintptr_t getNestedPointerAddress(HANDLE hProc, uintptr_t addr, std::vector<int> offsets);
void setNops(HANDLE hProc, uintptr_t addr, int numBytes);
void setInstruction(HANDLE hProc, uintptr_t addr, BYTE* origInstr, int numBytes);