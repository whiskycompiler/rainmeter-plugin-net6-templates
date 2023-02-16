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

#include "MeasureShim.hpp"

Measure::Measure(string_t binaryPath, string_t runtimeConfigPath, string_t dotnetPluginType)
	: binaryPath(std::move(binaryPath))
	, runtimeConfigPath(std::move(runtimeConfigPath))
	, dotnetPluginType(std::move(dotnetPluginType))
{
	netHost = new NetHost;
}

Measure::~Measure()
{
	// first net method pointers
	update = nullptr;
	initialize = nullptr;

	delete netHost;
	netHost = nullptr;

	data = nullptr; // this is freed by the .NET plugin
}

void Measure::Initialize(void* rainmeter)
{
	if (EnsureInitializedNetMethodPointer(L"Initialize", L"InitializeDelegate", reinterpret_cast<void**>(&initialize)))
	{
		initialize(&data, rainmeter);
	}
	else
	{
		// TODO: logging
	}
}

double Measure::Update()
{
	if(EnsureInitializedNetMethodPointer(L"Update", L"UpdateDelegate", reinterpret_cast<void**>(&update)))
	{
		return update(data);
	}
	else
	{
		// TODO: logging
	}

	return -1.0;
}

void Measure::Finalize()
{
	if (EnsureInitializedNetMethodPointer(L"Finalize", L"FinalizeDelegate", reinterpret_cast<void**>(&finalize)))
	{
		finalize(data);
	}
	else
	{
		// TODO: logging
	}

	data = nullptr;
}

void Measure::Reload(void* rainmeter, double* maxValue)
{
	if (EnsureInitializedNetMethodPointer(L"Reload", L"ReloadDelegate", reinterpret_cast<void**>(&reload)))
	{
		reload(data, rainmeter, maxValue);
	}
	else
	{
		// TODO: logging
	}
}

LPCWSTR Measure::GetString()
{
	if (EnsureInitializedNetMethodPointer(L"GetString", L"GetStringDelegate", reinterpret_cast<void**>(&getString)))
	{
		return getString(data);
	}
	else
	{
		// TODO: logging
	}

	return nullptr;
}

void Measure::ExecuteBang(LPCWSTR args)
{
	if (EnsureInitializedNetMethodPointer(L"ExecuteBang", L"ExecuteBangDelegate", reinterpret_cast<void**>(&executeBang)))
	{
		executeBang(data, args);
	}
	else
	{
		// TODO: logging
	}
}

bool Measure::EnsureInitializedNetMethodPointer(
	const wchar_t* entryPointName,
	const wchar_t* delegateName,
	void** methodPointer) const
{
	if (*methodPointer == nullptr)
	{
		string_t* fullDelegateName;
		GetFullDelegateName(delegateName, &fullDelegateName);
		netHost->GetMethodFromAssembly(
			binaryPath.c_str(),
			runtimeConfigPath.c_str(),
			dotnetPluginType.c_str(),
			entryPointName,
			fullDelegateName->c_str(),
			methodPointer);

		delete fullDelegateName;
		fullDelegateName = nullptr;

		if(*methodPointer == nullptr)
		{
			return false;
		}
	}

	return true;
}

void Measure::GetFullDelegateName(const wchar_t* delegateName, string_t** fullDelegateName) const
{
	const auto name = new string_t(dotnetPluginType);
	const auto pos = name->find_last_of(L',');
	if (pos == string_t::npos)
	{
		*fullDelegateName = nullptr;
		return;
	}

	name->insert(pos, delegateName);
	name->insert(pos, L"+");
	*fullDelegateName = name;
}
