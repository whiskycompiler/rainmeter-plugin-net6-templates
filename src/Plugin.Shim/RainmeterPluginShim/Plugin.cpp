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

#include <vector>

#include "Plugin.hpp"
#include "NetHost.hpp"
#include "MeasureShim.hpp"

#define DIR_SEPARATOR L'\\'

// Overview: This is a blank canvas on which to build your plugin.

// Note: GetString, ExecuteBang and an unnamed function for use as a section variable
// have been commented out. Uncomment any functions as needed.
// For more information, see the SDK docs: https://docs.rainmeter.net/developers/plugin/cpp/

string_t* GetShimBinaryDirectory()
{
	HMODULE hm = nullptr;

	if (GetModuleHandleExW(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCWSTR>(&GetShimBinaryDirectory),
		&hm) == 0)
	{
		return nullptr;
	}

	std::vector<wchar_t> pathBuffer;
	DWORD copied;
	do {
		pathBuffer.resize(pathBuffer.size() + MAX_PATH);
		copied = GetModuleFileNameW(hm, pathBuffer.data(), static_cast<DWORD>(pathBuffer.size()));
		if (copied == 0)
		{
			return nullptr;
		}
	} while (copied >= pathBuffer.size());

	pathBuffer.resize(copied);

	string_t rootPath = pathBuffer.data();
	const auto pos = rootPath.find_last_of(DIR_SEPARATOR);
	if (pos == string_t::npos)
	{
		return nullptr;
	}

	rootPath = rootPath.substr(0, pos + 1);
	return new string_t(rootPath);
}

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
	const auto rootPath = *GetShimBinaryDirectory();
	const auto pluginFilePath = rootPath + PLUGIN_NAME_STRING + DIR_SEPARATOR + PLUGIN_NAME_STRING;
	const auto pluginType = string_t(PLUGIN_NAME_STRING) + L".NativeInterop.Plugin, " + PLUGIN_NAME_STRING;
	const auto measure = new Measure(
		string_t(pluginFilePath + L".dll"),
		string_t(pluginFilePath + L".runtimeconfig.json"),
		string_t(pluginType));

	RmLog(rm, LOG_DEBUG, pluginFilePath.c_str());
	RmLog(rm, LOG_DEBUG, pluginType.c_str());

	measure->Initialize(rm);
	*data = measure;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
	const auto measure = static_cast<Measure*>(data);
	measure->Reload(rm, maxValue);
}

PLUGIN_EXPORT double Update(void* data)
{
	const auto measure = static_cast<Measure*>(data);
	return measure->Update();
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
	const auto measure = static_cast<Measure*>(data);
	return measure->GetString();
}

PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
{
	const auto measure = static_cast<Measure*>(data);
	measure->ExecuteBang(args);
}

//PLUGIN_EXPORT LPCWSTR (void* data, const int argc, const WCHAR* argv[])
//{
//	Measure* measure = (Measure*)data;
//	return nullptr;
//}

PLUGIN_EXPORT void Finalize(void* data)
{
	const auto measure = static_cast<Measure*>(data);
	measure->Finalize();
	delete measure;
}
