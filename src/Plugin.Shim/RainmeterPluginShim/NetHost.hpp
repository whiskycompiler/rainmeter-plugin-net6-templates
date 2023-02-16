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

#pragma once
#include "include.hpp"

constexpr auto NETHOST_ERROR_LOADFXR = 1;
constexpr auto NETHOST_ERROR_LOADCONF_TODO_REFACTOR = 2;
constexpr auto NETHOST_ERROR_LOADFUNC = 3;
constexpr auto NETHOST_SUCCESS = 4;

struct AssemblyInfo
{
	const string_t* binaryPath;
	const string_t* runtimeConfigPath;
	const string_t* dotnetType;
	const string_t* methodName;
	const string_t* delegateName;

	~AssemblyInfo()
	{
		delete binaryPath;
		delete runtimeConfigPath;
		delete dotnetType;
		delete methodName;
		delete delegateName;

		binaryPath = nullptr;
		runtimeConfigPath = nullptr;
		dotnetType = nullptr;
		methodName = nullptr;
		delegateName = nullptr;
	}
};

class NetHost
{
public:
	~NetHost();
	int GetMethodFromAssembly(
		const char_t* binaryPath,
		const char_t* runtimeConfigPath,
		const char_t* dotnetType,
		const char_t* methodName,
		const char_t* delegateName,
		void** methodPointer);

private:
	load_assembly_and_get_function_pointer_fn get_function_pointer_fptr = nullptr;
	hostfxr_initialize_for_runtime_config_fn init_fptr = nullptr;
	hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
	hostfxr_close_fn close_fptr = nullptr;

	bool LoadHostFxr();
	[[nodiscard]] bool IsHostFxrLoaded() const;

	load_assembly_and_get_function_pointer_fn GetAssemblyFunctionLoader(const char_t* config_path);

	static void* SafeLoadLibrary(const char_t* path);
	static void* GetExport(void* hLib, const char* name);
};