# Installs MicroTest on Windows.
#   .\install.ps1                          -> into the MinGW/MSYS2 toolchain that owns gcc.exe from PATH
#                                             (then gcc finds MicroTest.h and libMicroTest.a without -I/-L)
#   .\install.ps1 -Prefix C:\Libs\MicroTest -> any other folder
#   .\install.ps1 -Uninstall               -> removes the installed files
param(
	[string]$Prefix = "",
	[switch]$Uninstall
)
$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

if($Uninstall){
	cmake --build build --target uninstall
	exit $LASTEXITCODE
}

$generator = @()
$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if($gcc){
	# gcc.exe lives in <toolchain>\bin, its default include/lib folders are <toolchain>\include and <toolchain>\lib
	if($Prefix -eq ""){
		$Prefix = Split-Path (Split-Path $gcc.Source -Parent) -Parent
	}
	if(Get-Command ninja -ErrorAction SilentlyContinue){
		$generator = @("-G", "Ninja")
	}
	else{
		$generator = @("-G", "MinGW Makefiles")
	}
}
elseif($Prefix -eq ""){
	$Prefix = Join-Path $env:LOCALAPPDATA "MicroTest"
}

cmake -S . -B build @generator -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=$Prefix"
if($LASTEXITCODE -ne 0){ exit $LASTEXITCODE }
cmake --build build --config Release
if($LASTEXITCODE -ne 0){ exit $LASTEXITCODE }
cmake --install build --config Release
if($LASTEXITCODE -ne 0){ exit $LASTEXITCODE }

Write-Host "MicroTest installed to $Prefix"
