$ErrorActionPreference = "Stop"
$jsonPath = "$($args[0])\\CMakePresets.json"
$pluginName = $args[1]
$json = Get-Content $jsonPath | ConvertFrom-Json
($json.configurePresets | Where-Object -FilterScript { $_.name -eq "windows-base" })[0].cacheVariables | Add-Member -NotePropertyName PLUGIN_NAME -NotePropertyValue $pluginName -Force;
Set-Content -Path $jsonPath -Value ($json | ConvertTo-Json -Depth 10)