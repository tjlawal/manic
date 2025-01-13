// Copyright Frost Gorilla, Inc. All Rights Reserved.

// System info (implemented per-os)

internal OS_SystemInfo *os_get_system_info(void) { return &os_w32_state.system_info; }

internal OS_ProcessInfo *os_get_process_info(void) { return &os_w32_state.process_info; }

////////////////////////
// Memory allocation (implemented per-os)

internal void *os_reserve(u64 size) {
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);

  if (result == NULL) {
    DWORD error = GetLastError();
    // Log or debug print the error
    printf("VirtualAlloc failed with error: %lu\n", error);
  }
  return result;
}

internal b32 os_commit(void *ptr, u64 size) {
  b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

internal void os_decommit(void *ptr, u64 size) { VirtualFree(ptr, size, MEM_DECOMMIT); }

internal void os_release(void *ptr, u64 size) {
  // NOTE(tijani): the size is not actually used in win32,
  // VirtualFree must be 0 to release the spcified page region
  // back to windows.
  // 'size' is therefore not used on windows but is necessary
  // for other OSes.
  VirtualFree(ptr, 0, MEM_RELEASE);
}

internal void *os_reserve_large(u64 size) {
  // NOTE(tijani): windows requires that large-page memory must
  // be reserved and commited as a single operation.
  void *result = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
  return result;
}

internal b32 os_commit_large(void *ptr, u64 size) {
  // NOTE(tijani): see os_reserve_large().
  return 1;
}

// Aborting (implemented per-os)
internal void os_abort(s32 exit_code) { ExitProcess(exit_code); }

// Win32 entry point
#include <dbghelp.h>
#undef OS_WINDOWS // shlwapi uses OS_WINDOWS in its own context
#include <shlwapi.h>

internal HRESULT WINAPI win32_dialog_callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LONG_PTR data) {
  if (msg == TDN_HYPERLINK_CLICKED) {
    ShellExecuteW(NULL, L"open", (LPWSTR)lparam, NULL, NULL, SW_SHOWNORMAL);
  }
  return S_OK;
}

// clang-format off
internal LONG WINAPI win32_exception_filter(EXCEPTION_POINTERS *exception_ptrs) {
  // TODO(tijani): when this is multithreaded, make sure other threads
  // do not popup the same message box. only show the exception message
  // box on the first thread to crash and shutdown the application.

  WCHAR buffer[4096] = {0};
  int buflen = 0;

  DWORD exception_code = exception_ptrs->ExceptionRecord->ExceptionCode;
  buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen,
                       L"Fatal exception occured (code 0x%x). Terminating.\n", exception_code);

  // NOTE(tijani): dbghelp available features differs depending on the OS. loading dynamically in
  // case it is missing.
  HMODULE dbghelp = LoadLibraryA("dbghelp.dll");
  if (dbghelp) {
		DWORD(WINAPI *dbg_SymSetOptions)(DWORD SymOptions);
    BOOL(WINAPI  *dbg_SymInitializeW)(HANDLE hProcess, PCWSTR UserSearchPath, BOOL fInvadeProcess);
    BOOL(WINAPI  *dbg_StackWalk64)(DWORD MachineType, HANDLE hProcess, HANDLE hThread, LPSTACKFRAME64 StackFrame, PVOID ContextRecord,
																	 PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE64 FuncionTableAccessRoutine,
																	 PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
    PVOID(WINAPI  * dbg_SymFunctionTableAccess64)(HANDLE hProcess, DWORD64 AddrBase);
		DWORD64(WINAPI *dbg_SymGetModuleBase64)(HANDLE hProcess, DWORD64 qwAddr);
		BOOL(WINAPI *dbg_SymFromAddrW)(HANDLE hProcess, DWORD64 ADdress, PDWORD64 Displacement, PSYMBOL_INFOW Symbol);
		BOOL(WINAPI *dbg_SymGetLineFromAddrW64)(HANDLE hProcess, DWORD64 dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINEW64 SourceLine);
		BOOL(WINAPI *dbg_SymGetModuleInfoW64)(HANDLE hProcess, DWORD64 qwAddr, PIMAGEHLP_MODULEW64 ModuleInfo);


		*(FARPROC*)&dbg_SymSetOptions 					 = GetProcAddress(dbghelp, "SymSetOptions");
		*(FARPROC*)&dbg_SymInitializeW 					 = GetProcAddress(dbghelp, "SymInitializeW");
		*(FARPROC*)&dbg_StackWalk64 						 = GetProcAddress(dbghelp, "StackWalk64");
		*(FARPROC*)&dbg_SymFunctionTableAccess64 = GetProcAddress(dbghelp, "SymFunctionTableAccess64");
		*(FARPROC*)&dbg_SymGetModuleBase64 			 = GetProcAddress(dbghelp, "SymGetModuleBase64");
		*(FARPROC*)&dbg_SymFromAddrW 			 			 = GetProcAddress(dbghelp, "SymFromAddrW");
		*(FARPROC*)&dbg_SymGetLineFromAddrW64 	 = GetProcAddress(dbghelp, "SymGetLineFromAddrW64");
		*(FARPROC*)&dbg_SymGetModuleInfoW64 		 = GetProcAddress(dbghelp, "SymGetModuleInfoW64");

		if(dbg_SymSetOptions && dbg_SymInitializeW && dbg_StackWalk64 && dbg_SymFunctionTableAccess64 && dbg_SymGetModuleBase64 && dbg_SymFromAddrW && dbg_SymGetLineFromAddrW64 && dbg_SymGetModuleInfoW64){
			HANDLE process = GetCurrentProcess();
			HANDLE thread = GetCurrentThread();
			CONTEXT *context = exception_ptrs->ContextRecord;
			
			WCHAR module_path[MAX_PATH];
			printf("%lu\n", GetModuleFileNameW(NULL, module_path, array_count(module_path)));
			PathRemoveFileSpecW(module_path);

			dbg_SymSetOptions(SYMOPT_EXACT_SYMBOLS | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
			if(dbg_SymInitializeW(process, module_path, TRUE)){
				// NOTE(tijani): check manic.pdb is good to go
				b32 pdb_valid = 0;
				{
					IMAGEHLP_MODULEW64 module = {0};
					module.SizeOfStruct = sizeof(module);
					if(dbg_SymGetModuleInfoW64(process, (DWORD64)&win32_exception_filter, &module)){
						pdb_valid = (module.SymType == SymPdb);
					}
				}

				if(!pdb_valid){
					buflen += wnsprintfW(buffer + buflen, sizeof(buffer) - buflen, L"\nThe PDB debug information file for this executable is not valid or was not found. Rebuild binary to get the call stack.\n");
				} else {
					STACKFRAME64 frame = {0};
					DWORD machine_type;
					#if defined(_M_AMD64)
						machine_type = IMAGE_FILE_MACHINE_AMD64;
						
						frame.AddrPC.Mode = AddrModeFlat;
						frame.AddrPC.Offset = context->Rip;
						
						frame.AddrPC.Mode = AddrModeFlat;
						frame.AddrPC.Offset = context->Rsp;

						frame.AddrPC.Mode = AddrModeFlat;
						frame.AddrPC.Offset = context->Rbp;

					#elif defined(_M_ARM64)
						machine_type = IMAGE_FILE_MACHINE_ARM64;

						frame.AddrPC.Mode = AddrModeFlat;
						frame.AddrPC.Offset = context->Pc;
						
						frame.AddrPC.Mode = AddrModeFlat;
						frame.AddrPC.Offset = context->Sp;
						
						frame.AddrPC.Mode = AddrModeFlat;
						frame.AddrPC.Offset = context->Fp;
					#else
						#error Architecture not supported.
					#endif

					for(u32 idx = 0; ;idx++){
						const u32 max_frames = 32;
						if(idx == max_frames){
							buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L"....");
							break;
						}

						// NOTE(tijani): StackWalk64 should work without issues on ARM CPUs because of "Windows on ARM (WOA)".
						if(!dbg_StackWalk64(machine_type, process, thread, &frame, context, 0, dbg_SymFunctionTableAccess64, dbg_SymGetModuleBase64, 0)) break;

						u64 address = frame.AddrPC.Offset;
						if(address == 0) break;

						if(idx == 0){
							#if BUILD_CONSOLE_INTERFACE
								buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L"\nPlease report this issue at: %S\n\n", BUILD_ISSUES_LINK_STRING_LITERAL);
							#else
								buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L"\nPress Ctrl+C to copy this text to clipboard, then report this issue at:\n"
																																										L"<a href=\"%S\">%S</a>\n\n", BUILD_ISSUES_LINK_STRING_LITERAL, BUILD_ISSUES_LINK_STRING_LITERAL);
							#endif

							buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L"Call stack:\n");
						}

						buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L"%u. [0x%I64x]", idx + 1,  address);

						struct {
							SYMBOL_INFOW info;
							WCHAR name[MAX_SYM_NAME];
						} symbol = {0};

						symbol.info.SizeOfStruct = sizeof(symbol.info);
						symbol.info.MaxNameLen = MAX_SYM_NAME;

						DWORD64 displacement = 0;
						if(dbg_SymFromAddrW(process, address, &displacement, &symbol.info)){
							buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L", %s line %u", symbol.info.Name, (DWORD)displacement);

							IMAGEHLP_LINEW64 line = {0};
							line.SizeOfStruct = sizeof(line);
							
							DWORD line_displacement = 0;
							if(dbg_SymGetLineFromAddrW64(process, address, &line_displacement, &line)){
								buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L", %s line %u", PathFindFileNameW(line.FileName), line.LineNumber);
							}
						} 
						
						else {
							IMAGEHLP_MODULEW64 module = {0};
							module.SizeOfStruct = sizeof(module);
							if(dbg_SymGetModuleInfoW64(process, address, &module)){
								buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L" %s", module.ModuleName);
							}
						}
						buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L"\n");
					}
				}
			}
		}
  }

	buflen += wnsprintfW(buffer + buflen, array_count(buffer) - buflen, L"\nBuild Version: %S%S", BUILD_VERSION_STRING_LITERAL, BUILD_SVN_REVISION_STRING_LITERAL_APPEND);

	// NOTE(tijani): Now show the exception dialog
	#if BUILD_CONSOLE_INTERFACE
		fwprintf(stderr, L"\n--- Fatal Exception ---\n");
		fwprintf(stderr, L"%s\n\n", buffer);
	#else
		TASKDIALOGCONFIG dialog = {0}; 
		dialog.cbSize = sizeof(dialog);
		dialog.dwFlags = TDF_SIZE_TO_CONTENT | TDF_ENABLE_HYPERLINKS | TDF_ALLOW_DIALOG_CANCELLATION;
		dialog.pszMainIcon = TD_ERROR_ICON;
		dialog.dwCommonButtons = TDCBF_CLOSE_BUTTON;
		dialog.pszWindowTitle = L"FatalException";
		dialog.pszContent = buffer;
		dialog.pfCallback = &win32_dialog_callback;
		TaskDialogIndirect(&dialog, 0, 0, 0);
	#endif

  ExitProcess(1);
}
// clang-format on

#undef OS_WINDOWS
#define OS_WINDOWS 1

// TODO(tijani): Add argc and argv
internal void w32_entry_point_caller() {
  SetUnhandledExceptionFilter(&win32_exception_filter);

  // NOTE(tijani): Do initialization stuff here before calling into the
  // "real" entry point

  // TODO(tijani): see if we can allow large pages

  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  {
    OS_SystemInfo *system_information = &os_w32_state.system_info;
    system_information->logical_processor_count = (u64)sys_info.dwNumberOfProcessors;
    system_information->page_size = sys_info.dwPageSize;
    system_information->large_page_size = GetLargePageMinimum();
    system_information->allocation_granularity = sys_info.dwAllocationGranularity;
  }

  // TODO(tijani): extract arguments

  // tijani: setup thread context
  local ThreadContext thread_context;
  thread_context_init_and_equip(&thread_context);

  // NOTE(tijani): call into the "real" entry point
  main_thread_base_entry_point();
}

#if BUILD_CONSOLE_INTERFACE
int wmain() {
  w32_entry_point_caller();
  return 0;
}
#elif BUILD_DEBUG
int main() {
  w32_entry_point_caller();
  return 0;
}
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
  w32_entry_point_caller();
  return 0;
}

#endif