param(
    [Parameter(Position = 0)][string]$Target = '',
    [ValidateSet('Release', 'Debug')][string]$Config = 'Release',
    [switch]$Run,
    [switch]$List,
    [switch]$Reconfigure
)
$ErrorActionPreference = 'Stop'
$Root = $PSScriptRoot
$BuildDir = Join-Path $Root 'build'
$cmake = (Get-Command cmake -ErrorAction Stop).Source
if ($List) {
    Get-ChildItem (Join-Path $Root 'src') -Filter *.cpp | ForEach-Object { $_.BaseName }
    exit 0
}
if ($Reconfigure -and (Test-Path $BuildDir)) { Remove-Item $BuildDir -Recurse -Force }
& $cmake -S $Root -B $BuildDir
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }
$buildArgs = @('--build', $BuildDir, '--config', $Config, '--parallel')
if ($Target) { $buildArgs += @('--target', $Target) }
& $cmake @buildArgs
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
if ($Run) {
    if (-not $Target) { throw 'Specify a target, e.g. .\build.ps1 04_interactive_cube -Run' }
    $exe = Join-Path $BuildDir "$Config\$Target.exe"
    if (-not (Test-Path $exe)) { $exe = Join-Path $BuildDir "$Target.exe" }
    Push-Location $Root
    try { & $exe } finally { Pop-Location }
}
