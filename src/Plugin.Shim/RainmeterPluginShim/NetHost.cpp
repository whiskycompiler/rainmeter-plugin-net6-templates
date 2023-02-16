/* -----------------------------------------------------------------------
	Copyright (C) 2023 whiskycompiler

	This file is part of "Plugin.Shim".

	This program is free software: you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation, either version 3
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------*/

#include <cassert>

#include "NetHost.hpp"

NetHost::~NetHost()
{
	get_function_pointer_fptr = nullptr;
	init_fptr = nullptr;
	get_delegate_fptr = nullptr;
	close_fptr = nullptr;
}

int NetHost::GetMethodFromAssembly(
	const char_t* binaryPath,
	const char_t* runtimeConfigPath,
	const char_t* dotnetType,
	const char_t* methodName,
	const char_t* delegateName,
	void** methodPointer)
{
	// STEP 1: Load HostFxr and get exported hosting functions
	if (!IsHostFxrLoaded() && !LoadHostFxr())
	{
		return NETHOST_ERROR_LOADFXR;
	}

	// STEP 2: Initialize and start the .NET Core runtime
	const auto loadAssemblyAndGetFunctionPointer = GetAssemblyFunctionLoader(runtimeConfigPath);
	if (loadAssemblyAndGetFunctionPointer == nullptr)
	{
		return NETHOST_ERROR_LOADCONF_TODO_REFACTOR;
	}

	// STEP 3: Load managed assembly and get function pointer to a managed method
	const int result = loadAssemblyAndGetFunctionPointer(
		binaryPath,
		dotnetType,
		methodName,
		delegateName,
		nullptr /*reserved*/,
		methodPointer);
	if (result != 0 || methodPointer == nullptr)
	{
		return NETHOST_ERROR_LOADFUNC;
	}

	return NETHOST_SUCCESS;
}


// Using the nethost library, discover the location of hostfxr and get exports
bool NetHost::LoadHostFxr()
{
	// Pre-allocate a large buffer for the path to hostfxr
	char_t buffer[MAX_PATH];
	size_t buffer_size = sizeof buffer / sizeof(char_t);
	if (get_hostfxr_path(buffer, &buffer_size, nullptr) != 0)
	{
		return false;
	}

	// Load hostfxr and get desired exports
	void* lib = SafeLoadLibrary(buffer);
	init_fptr = reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(GetExport(lib, "hostfxr_initialize_for_runtime_config"));
	get_delegate_fptr = reinterpret_cast<hostfxr_get_runtime_delegate_fn>(GetExport(lib, "hostfxr_get_runtime_delegate"));
	close_fptr = reinterpret_cast<hostfxr_close_fn>(GetExport(lib, "hostfxr_close"));

	return IsHostFxrLoaded();
}

// Load and initialize .NET Core and get desired function pointer for scenario
load_assembly_and_get_function_pointer_fn NetHost::GetAssemblyFunctionLoader(const char_t* config_path)
{
	if (get_function_pointer_fptr)
	{
		return *get_function_pointer_fptr;
	}

	// Load .NET Core
	hostfxr_handle cxt = nullptr;
	int result = init_fptr(config_path, nullptr, &cxt);
	if (cxt == nullptr) // this is no nullptr only on success
	{
		// TODO: logging and error codes
		//std::cerr << "Init failed: " << std::hex << std::showbase << rc << std::endl;
		close_fptr(cxt);
		return nullptr;
	}

	// Get the load assembly function pointer
	result = get_delegate_fptr(
		cxt,
		hdt_load_assembly_and_get_function_pointer,
		reinterpret_cast<void**>(&get_function_pointer_fptr));
		//&functionPointer);

	// TODO: logging and error code
	//if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
	//    std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc << std::endl;

	close_fptr(cxt);
	return get_function_pointer_fptr;
}

bool NetHost::IsHostFxrLoaded() const
{
	return init_fptr && get_delegate_fptr && close_fptr;
}

void* NetHost::SafeLoadLibrary(const char_t* path)
{
	const HMODULE h = LoadLibraryW(path);
	assert(h != nullptr); // TODO: remove and implement meaningful error handling
	return h;
}

void* NetHost::GetExport(void* hLib, const char* name)
{
	const FARPROC f = GetProcAddress(static_cast<HMODULE>(hLib), name);
	assert(f != nullptr); // TODO: remove and implement meaningful error handling
	return reinterpret_cast<void*>(f);
}