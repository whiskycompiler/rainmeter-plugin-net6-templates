/* -----------------------------------------------------------------------
    Copyright (C) 2014 Birunthan Mohanathas
    Modifications Copyright (C) 2023 whiskycompiler

    This file is part of "Plugin.Example.SystemVersion".

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

using Plugin.Example.SystemVersion.Extensions;
using Plugin.Example.SystemVersion.NativeInterop;

namespace Plugin.Example.SystemVersion
{
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
        private enum MeasureType
        {
            String,
            Major,
            Minor,
            Number,
        }

        private readonly IRainmeterMeasureApiProxy _rainmeterMeasure;

        private IntPtr _getStringBufferIntPtr;
        private MeasureType _measureType = MeasureType.String;

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
            switch (_measureType)
            {
                case MeasureType.Major:
                    return Environment.OSVersion.Version.Major;
                case MeasureType.Minor:
                    return Environment.OSVersion.Version.Minor;
                case MeasureType.Number:
                    return Environment.OSVersion.Version.Major + (Environment.OSVersion.Version.Minor / 10.0);
                case MeasureType.String:
                {
                    // Update the pointer to the value returned by GetString()
                    $"{Environment.OSVersion.Version.Major}.{Environment.OSVersion.Version.Minor} (Build {Environment.OSVersion.Version.Build})"
                        .RecyclePointerAndSetAsNewValue(ref _getStringBufferIntPtr);

                    // return 0 because the actual value is retrieved via GetString()
                    return 0d;
                }
                default:
                    return 0d;
            }
        }

        /// <inheritdoc cref="NativeInterop.Plugin.Finalize"/>
        public void Dispose()
        {
            _getStringBufferIntPtr.FreeUnmanagedHandle();
        }

        /// <inheritdoc cref="NativeInterop.Plugin.Reload"/>
        public void Reload()
        {
            var type = _rainmeterMeasure.ReadString("Type", string.Empty);
            if (!Enum.TryParse(type, true, out MeasureType measureType))
            {
                _rainmeterMeasure.Log(RainmeterLogLevel.Error, $"Invalid value of option 'Type': {type}");
                return;
            }

            _measureType = measureType;
        }

        /// <inheritdoc cref="NativeInterop.Plugin.GetString"/>
        public IntPtr GetString()
        {
            return _measureType switch
            {
                MeasureType.String => _getStringBufferIntPtr,
                _ => IntPtr.Zero, // this instructs rainmeter to use the value returned by Update()
            };
        }

        /// <inheritdoc cref="NativeInterop.Plugin.ExecuteBang"/>
        public void ExecuteBang()
        {
            _rainmeterMeasure.Log(RainmeterLogLevel.Error, "The plugin does not support this action!");
        }

        /// <inheritdoc cref="NativeInterop.Plugin.CustomFunc"/>
        public IntPtr CustomFunc(string[] arguments)
        {
            _rainmeterMeasure.Log(RainmeterLogLevel.Error, "The plugin does not support this action!");
            return IntPtr.Zero;
        }
    }
}