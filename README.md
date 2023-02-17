# Rainmeter Plugin Examples/Templates for .NET 6

# State: beta

## What is this?
<p>Examples/Templates you can use to create rainmeter plugins with .NET 6 and probably later versions as well. The <a href="https://github.com/rainmeter/rainmeter-plugin-sdk">existing SDK</a> (2023-02-16) only supports the older .NET Framework which is easier to access from native C++.</p>
<br/>

### How does it work?
<p>Contrary to the old .NET Framework the newer versions require native code to host the .NET application manually and load the necessary runtime in order to access the managed code directly.
For more info about how this works <a href="https://learn.microsoft.com/en-us/dotnet/core/tutorials/netcore-hosting">see here</a> (2023-02-16).
In these examples/templates a native C++ DLL is built specifically for your plugin to take care of this job. It proxies all Rainmeter plugin functionality into your C# plugin.</p>

<p>The native C++ DLL project that serves as a proxy for the C# plugin can be found in the "src/Plugin.Shim" folder. When you use the Visual Studio publish action in one of the C# example projects it will automatically trigger the CMake build process of the shim DLL and declare the name of the project as a compile constant. This constant will be used to name the resulting shim DLL and make it target the projects C# plugin DLL (which is expected to be in a subfolder with the project name e.g. "Plugin.Example.Empty/Plugin.Example.Empty.dll").
The constant must have the same name as the projects root namespace because the Shim will call into the static <code>Plugin</code> class located in the <code>{project name}.NativeInterop</code> namespace.<br/>

<br/>

## How do I use it?
### Preparations
1. Make sure you have the necessary tools installed:
    - Visual Studio 2022 or newer with .NET and C++ workloads
    - <a href="https://github.com/rainmeter/rainmeter">Rainmeter</a> and the <a href="https://github.com/rainmeter/rainmeter-plugin-sdk">Rainmeter Plugin SDK</a>
2. Set the following environment variables for the build process:
    - <code>RainmeterPluginSdkDir</code> = Path to the root folder of the Rainmeter Plugin SDK on your local machine
    - <code>NetCoreNativeRuntimePackDir</code> = Path to the x64 .NET runtime hosting pack binaries folder that contains "nethost.h", "nethost.lib", "nethost.dll", "hostfxr.h" and "coreclr_delegates.h" (will probably look like this "C:\Program Files\dotnet\packs\Microsoft.NETCore.App.Host.win-x64\6.0.13\runtimes\win-x64\native")
3. Make the "nethost.dll" from the previous step discoverable for the "rainmeter.exe" by putting it into the rainmeter program folder or maybe adding it to the Path environment variable.
4. Depending on your Visual Studio installation you might need to adjust the path to the "vcvarsall.bat" file in the "AfterPublish.bat" script located in the "Scripts" folder of each C# plugin project.<br/>

### Usage
0. I would recommend to read "How does it work?" first so you don't accidentally break the plugin build process. 
1. Choose any <code>Plugin.Example...</code> as a starting or reference point. You may delete any other project starting with <code>Plugin.Example</code>.
2. You can develop your plugin inside of the chosen example or by using it as a reference.
3. Get the binaries to put in the plugin folder using the Visual Studio publish action. By default they will be placed in the example repository root "publish/{project name}" directory. You need at least the "{project name}.dll" and the folder with the same name.
4. Put the files in the Rainmeter plugin folder and use an example skin (see folder in repository root) or your own skin to load the the plugin.

<br/>

## Known Issues / Missing features
- some TODOs left in the code
- performance compared to .NET Framework 4.x plugins unknown
- not sure if this is/will be compatible with the Rainmeter skin installer
    - version of the shim is not dynamic yet
- currently only builds x64
- Rainmeter custom function invocation is not implemeted yet
