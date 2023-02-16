/* -----------------------------------------------------------------------
    Copyright (C) 2023 whiskycompiler

    This file is part of "Plugin.Example.Empty".

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

using Plugin.Example.Empty.Extensions;
using Plugin.Example.Empty.NativeInterop;

namespace Plugin.Example.Empty;

/// <summary>
/// This is the place where you can implement your own measure plugin.<br/>
/// All native interop is in its own namespace and not really relevant most of the time.<br/>
/// You can get all files to put in the rainmeter plugin folder by using the VS publish funtion (see README.MD for more info).
/// </summary>
/// <remarks>
/// Sealed to use simple dispose pattern.
/// </remarks>
public sealed class Measure : IDisposable
{
    private readonly IRainmeterMeasureApiProxy _rainmeterMeasure;

    private IntPtr _getStringBufferIntPtr;
    private IntPtr _customFunctionBufferIntPtr;

    /// <summary>
    /// Initializes a new instance of the <see cref="Measure"/> class.
    /// </summary>
    public Measure(IRainmeterMeasureApiProxy rainmeterMeasure)
    {
        _rainmeterMeasure = rainmeterMeasure;
    }

    /// <inheritdoc cref="NativeInterop.Plugin.Update"/>
    public double Update()
    {
        _rainmeterMeasure.Log(RainmeterLogLevel.Debug, nameof(Update));

        // update the value of the pointer used by GetString
        "Hello World!".RecyclePointerAndSetAsNewValue(ref _getStringBufferIntPtr);

        return 0d;
    }

    /// <inheritdoc cref="NativeInterop.Plugin.Finalize"/>
    public void Dispose()
    {
        _rainmeterMeasure.Log(RainmeterLogLevel.Debug, nameof(Dispose));
    }

    /// <inheritdoc cref="NativeInterop.Plugin.Reload"/>
    public void Reload()
    {
        _rainmeterMeasure.Log(RainmeterLogLevel.Debug, nameof(Reload));
    }

    /// <inheritdoc cref="NativeInterop.Plugin.GetString"/>
    public IntPtr GetString()
    {
        _rainmeterMeasure.Log(RainmeterLogLevel.Debug, nameof(GetString));
        return _getStringBufferIntPtr;
    }

    /// <inheritdoc cref="NativeInterop.Plugin.ExecuteBang"/>
    public void ExecuteBang()
    {
        _rainmeterMeasure.Log(RainmeterLogLevel.Debug, nameof(GetString));
    }

    /// <inheritdoc cref="NativeInterop.Plugin.CustomFunc"/>
    public IntPtr CustomFunc(string[] arguments)
    {
        _rainmeterMeasure.Log(RainmeterLogLevel.Debug, nameof(GetString));
        return "Your custom invocation could be here!"
            .RecyclePointerAndSetAsNewValue(ref _customFunctionBufferIntPtr);
    }
}