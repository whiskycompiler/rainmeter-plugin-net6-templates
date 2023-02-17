call C:\"Program Files"\"Microsoft Visual Studio"\2022\Community\VC\Auxiliary\Build\vcvarsall.bat x64
set shimFolder=%1..\Plugin.Shim
powershell -file Scripts/UpdateCMakePreset.ps1 "%shimFolder%" %2 %3 %4
cd %shimFolder%
cmake . --preset x64-release
cmake --build --preset default
cmake --install %1/../../obj/Plugin.Shim/x64-release