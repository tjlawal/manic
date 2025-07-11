#pragma once

#define WIN32_LEAN_AND_MEAN

// All WIN32 imports and lib includes should be defined here!
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <processthreadsapi.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "shell32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "comctl32")

// This line is required for loading the correct comctl32 dll
// file. It ensures the correct version (6) is selected to enable visual styles
// for the exception window.
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace Starlight::Foundation;

namespace Starlight {
	namespace Platform {

		// EXE folder stuff
		WCHAR EXE_FOLDER[256];
		WCHAR CURRENT_FOLDER[256];
		
		// Win32 State
		struct Win32State {
			Arena* arena;
			SystemInfo system_info;
			ProcessInfo process_info;
			u64 microsecond_resolution;
		};

		// Global
		global Win32State w32_state = {};

		// Win32 file information retrieval helpers
		internal FilePropertyFlag w32_file_property_flags_from_dwFileAttributes(DWORD file_attributes);

		// Win32 time conversion helpers
		internal void w32_dense_time_from_file_time(DenseTime *output, FILETIME *input);
		internal void w32_date_time_from_system_time(DateTime *output, SYSTEMTIME *input);
		internal void w32_system_time_from_date_time(SYSTEMTIME *output, DateTime *input);
	}
}