using namespace Starlight::Foundation;

namespace Starlight {
	namespace Platform {

		// Win32 file information retrieval helpers
		internal FilePropertyFlag w32_file_property_flags_from_dwFileAttributes(DWORD file_attributes) {
			FilePropertyFlag flags = {};
			if(file_attributes & FILE_ATTRIBUTE_DIRECTORY) {
				//flags |= FilePropertyFlag_IsFolder;
				//flags = (FilePropertyFlag)(static_cast<u32>(flags) | static_cast<u32>(FilePropertyFlag_IsFolder));
				flags = FilePropertyFlag_IsFolder; // @WARN This could be a problem?? Not sure.
			}
			return flags;
		}

		// Win32 time conversion helpers
		internal void w32_date_time_from_system_time(DateTime *output, SYSTEMTIME *input) {
			output->year = input->wYear;
			output->mon = input->wMonth - 1;
			output->wday = input->wDayOfWeek;
			output->day = input->wDay;
			output->hour = input->wHour;
			output->minute = input->wMinute;
			output->second = input->wSecond;
			output->milli_second = input->wMilliseconds;
		}

		internal void w32_dense_time_from_file_time(DenseTime *output, FILETIME *input) {
			SYSTEMTIME system_time = {0};
			FileTimeToSystemTime(input, &system_time);
			DateTime date_time = {0};
			w32_date_time_from_system_time(&date_time, &system_time);
			*output = dense_time_from_date_time(date_time);
		}


		internal void w32_system_time_from_date_time(SYSTEMTIME *output, DateTime *input) {}
	
		// System info
		internal SystemInfo*  get_system_info(void) { return &w32_state.system_info; }
		internal ProcessInfo* get_process_info(void) { return &w32_state.process_info; }

		internal void sleep(u64 ns) { Sleep(ns); }

		// Memory allocation
		// @REVISE: Should errors be handled here? what is the best way to do it?
		internal void* mem_reserve(u64 size) {
			void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);

			if (result == NULL) {
				DWORD error = GetLastError();
				// Log or debug print the error
				printf("VirtualAlloc failed with error: %lu\n", error);
			}
			return result;
		}

		internal b32 mem_commit(void *ptr, u64 size) {
			b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
			return result;
		}

		internal void mem_decommit(void *ptr, u64 size) { VirtualFree(ptr, size, MEM_DECOMMIT);  }

		// On Win32, size is not used, but keeping since its harmless and very useful for in other OSes.
		// VirtualFree must be 0 to release the specified page region back to windows.
		internal void mem_release(void *ptr, u64 size) { VirtualFree(ptr, 0, MEM_RELEASE); }

		// Aborting (implemented per-os)
		internal void abort(s32 exit_code) { ExitProcess(exit_code); }

		// File system (implemented per-os)
		internal FileProperty properties_from_file(Handle file) {
			if(handle_match(file, handle_zero())) { 
				FileProperty r = {0};
				return r;
			}

			FileProperty properties = {0};
			HANDLE file_handle = reinterpret_cast<HANDLE>(file.handle[0]);
			BY_HANDLE_FILE_INFORMATION file_info;
			BOOL file_good = GetFileInformationByHandle(file_handle, &file_info);
			if(file_good) {
				u32 _lo = file_info.nFileSizeLow;
				u32 _hi = file_info.nFileSizeHigh;
				properties.size = static_cast<u64>(_lo) | (static_cast<u64>(_hi) << 32);
				w32_dense_time_from_file_time(&properties.modified, &file_info.ftLastWriteTime);
				w32_dense_time_from_file_time(&properties.created, &file_info.ftCreationTime);
				properties.flags = w32_file_property_flags_from_dwFileAttributes(file_info.dwFileAttributes);
			}

			return properties;
		}

		internal Handle open_file(AccessFlags flags, string8 path) {
			Handle result = {0};
			Temp scratch = scratch_begin(0, 0);
			string16 path16 = str16_from_8(scratch.arena, path);
			DWORD access_flags = 0;
			DWORD share_mode = 0;
			DWORD creation_disposition = OPEN_EXISTING;
			SECURITY_ATTRIBUTES security_attributes = { sizeof(security_attributes), 0, 0};

			if(flags & AccessFlag_Read) access_flags |= GENERIC_READ;
			if(flags & AccessFlag_Write) {
				access_flags |= GENERIC_WRITE;
				creation_disposition = CREATE_ALWAYS;
			}
			if (flags & AccessFlag_Append) {
				creation_disposition = OPEN_ALWAYS;
				access_flags = FILE_APPEND_DATA;
			}
			if(flags & AccessFlag_ShareRead) share_mode |= FILE_SHARE_READ;
			if(flags & AccessFlag_ShareWrite) share_mode |= FILE_SHARE_WRITE | FILE_SHARE_READ;

			HANDLE file = CreateFileW(reinterpret_cast<WCHAR *>(path16.str), access_flags, share_mode, &security_attributes, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);

			if (file != INVALID_HANDLE_VALUE) {
				result.handle[0] = (u64)file;
			}
			scratch_end(scratch);
			return result;
		}

		internal void close_file(Handle file) {
			if (handle_match(file, handle_zero())) return;
			HANDLE handle = reinterpret_cast<HANDLE>(file.handle[0]);
			BOOL result = CloseHandle(handle);
			(void)result;
		}

		internal u64 read_file(Handle file, Rng1u64 rng, void *data_dest) {
			if (handle_match(file, handle_zero())) {
				return 0;
			}

			HANDLE file_handle = (HANDLE)file.handle[0];
			u64 size = 0;
			GetFileSizeEx(file_handle, (LARGE_INTEGER *)&size);
			Rng1u64 clamped_range = rng1u64(clamp_min(rng.minimum, size), clamp_min(rng.maximum, size));
			u64 total_read_size = 0;

			// This is equivalent to reading the entire file, but it is done in a chunked manner.
			// Reason is cause WIN32 only allows reading a max of 32-bit(4GB) at once so this reads the file
			// that is bigger than that in 32-bit chunks but all at once.
			{
				u64 bytes_to_read = sizeof_rng1u(clamped_range);
				for (u64 offset = rng.minimum; total_read_size < bytes_to_read;) {
					u64 amt64 = bytes_to_read - total_read_size;
					u32 amt32 = saturate_u32_from_u64(amt64);

					DWORD read_size = 0;
					OVERLAPPED overlapped = {0};
					overlapped.Offset = (offset & 0x00000000ffffffff);
					overlapped.OffsetHigh = (offset & 0xffffffff00000000) >> 32;
					ReadFile(file_handle, static_cast<u8*>(data_dest) + total_read_size, amt32, &read_size, &overlapped);
					offset += read_size;
					total_read_size += read_size;
					if (read_size != amt32) {
						break;
					}
				}
			}
			return total_read_size;
		}

		internal b32 write_file(Handle file, Rng1u64 range, void* data) {
			ProfFunction(profDebug_plum);
			if(handle_match(file, handle_zero())) return 0;

			HANDLE handle = reinterpret_cast<HANDLE>(file.handle[0]);
			u64 src_offset = 0;
			u64 dst_offset = range.minimum;
			u64 total_write_size = sizeof_rng1u(range);

			for(;;) {
				void* bytes_src = static_cast<u8*>(data) + src_offset;
				u64 bytes_left = total_write_size - src_offset;
				DWORD write_size = min(MB(1), bytes_left);
				DWORD bytes_written = 0;
				OVERLAPPED overlapped = {0};
				overlapped.Offset = (dst_offset & 0x00000000ffffffff);
				overlapped.OffsetHigh = (dst_offset & 0xffffffff00000000) >> 32;

				BOOL success = WriteFile(handle, bytes_src, write_size, &bytes_written, &overlapped);

				// @IMPROVE: Get the reason why the file write failed and provide to the caller.
				if(success == 0) break;
				src_offset += bytes_written;
				dst_offset += bytes_written;
				
				if(bytes_left == 0) break;
				return src_offset;
			}
		}

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

		internal LONG WINAPI win32_exception_filter(EXCEPTION_POINTERS *exception_ptrs) {
			// @TODO: when this is multithreaded, make sure other threads
			// do not popup the same message box. only show the exception message
			// box on the first thread the proceed to crash and burn.

			WCHAR buffer[4096] = {0};
			int buflen = 0;

			DWORD exception_code = exception_ptrs->ExceptionRecord->ExceptionCode;
			buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen,
													 L"Fatal exception occured (code 0x%x). Terminating.\n", exception_code);

			//  dbghelp available features differs depending on the OS. 
			// Loading dynamically in case it is missing.
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
					printf("%lu\n", GetModuleFileNameW(NULL, module_path, ArrayCount(module_path)));
					PathRemoveFileSpecW(module_path);

					dbg_SymSetOptions(SYMOPT_EXACT_SYMBOLS | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
					if(dbg_SymInitializeW(process, module_path, TRUE)){
						// Check pdb is good to go
						b32 pdb_valid = 0;

						{
							IMAGEHLP_MODULEW64 module = {0};
							module.SizeOfStruct = sizeof(module);
							if(dbg_SymGetModuleInfoW64(process, (DWORD64)&win32_exception_filter, &module)){
								pdb_valid = (module.SymType == SymPdb);
							}
						}

						if(!pdb_valid){
							buflen += wnsprintfW(buffer + buflen, sizeof(buffer) - buflen, L"\nThe PDB debug information file for this executable is not valid or was not found. Rebuild binary to get the call stack!\n");
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
								#error "Architecture not supported."
							#endif

							for(u32 idx = 0; ;idx++){
								const u32 max_frames = 32;
								if(idx == max_frames){
									buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L"....");
									break;
								}

								// @note: StackWalk64 should work without issues on ARM CPUs because of "Windows on ARM (WOA)".
								if(!dbg_StackWalk64(machine_type, process, thread, &frame, context, 0, dbg_SymFunctionTableAccess64, dbg_SymGetModuleBase64, 0)) break;

								u64 address = frame.AddrPC.Offset;
								if(address == 0) break;

								if(idx == 0){
									#if BUILD_CONSOLE_INTERFACE
									buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L"\nPlease report this issue at: %S\n\n", BUILD_ISSUES_LINK_STRING_LITERAL);
									#else
									buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L"\nPress Ctrl+C to copy this text to clipboard, then report this issue at:\n"
																			 L"<a href=\"%S\">%S</a>\n\n", BUILD_ISSUES_LINK_STRING_LITERAL, BUILD_ISSUES_LINK_STRING_LITERAL);
									#endif

									buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L"Call stack:\n");
								}

								buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L"%u. [0x%I64x]", idx + 1,  address);

								struct {
									SYMBOL_INFOW info;
									WCHAR name[MAX_SYM_NAME];
								} symbol = {0};

								symbol.info.SizeOfStruct = sizeof(symbol.info);
								symbol.info.MaxNameLen = MAX_SYM_NAME;

								DWORD64 displacement = 0;
								if(dbg_SymFromAddrW(process, address, &displacement, &symbol.info)){
									buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L", %s line %u", symbol.info.Name, (DWORD)displacement);

									IMAGEHLP_LINEW64 line = {0};
									line.SizeOfStruct = sizeof(line);
							
									DWORD line_displacement = 0;
									if(dbg_SymGetLineFromAddrW64(process, address, &line_displacement, &line)){
										buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L", %s line %u", PathFindFileNameW(line.FileName), line.LineNumber);
									}
								} 
						
								else {
									IMAGEHLP_MODULEW64 module = {0};
									module.SizeOfStruct = sizeof(module);
									if(dbg_SymGetModuleInfoW64(process, address, &module)){
										buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L" %s", module.ModuleName);
									}
								}
								buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L"\n");
							}
						}
					}
				}
			}

			buflen += wnsprintfW(buffer + buflen, ArrayCount(buffer) - buflen, L"\nBuild Version: %S%S", BUILD_VERSION_STRING_LITERAL, BUILD_SVN_REVISION_STRING_LITERAL_APPEND);

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

		#undef OS_WINDOWS
		#define OS_WINDOWS 1

		// @todo: Add argc and argv
		internal void w32_entry_point_caller() {
			ProfFunction(profDebug_plum);

			SetUnhandledExceptionFilter(&win32_exception_filter);

			// Do initialization stuff here before calling into the "real" entry point

			// TODO: See if large pages support is available here!

			SYSTEM_INFO sys_info;
			GetSystemInfo(&sys_info);
			{
				SystemInfo *system_information = &w32_state.system_info;
				system_information->logical_processor_count = (u64)sys_info.dwNumberOfProcessors;
				system_information->page_size = sys_info.dwPageSize;
				system_information->large_page_size = GetLargePageMinimum();
				system_information->allocation_granularity = sys_info.dwAllocationGranularity;
			}

			// Setup thread context
			local ThreadContext thread_context;
			tctxt_init_and_engage(&thread_context);

			// Call into the game
			main_thread_entry_point();
		}
	}
}

#if BUILD_DEBUG
	#	if OS_WINDOWS
		int wmain(int argc, wchar_t** argv) {
	#	else
		int main(int argc, char** argv) {
	#endif
		w32_entry_point_caller();
		return 0;
	}
	#else
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
		w32_entry_point_caller();
		return 0;
	}
#endif