$ErrorActionPreference = 'Stop'

$vcpkg_root = Join-Path (Split-Path -Parent $PSScriptRoot) 'vcpkg'
$packages = @('pkgconf', 'icu', 'libpng', 'sdl3', 'cairo', 'pango')

if (-not (Test-Path -Path (Join-Path $vcpkg_root 'bootstrap-vcpkg.bat'))) {
    throw "vcpkg not found at '$vcpkg_root'. Make sure the vcpkg submodule is checked out."
}

# Only prepare the shared binary cache directory when it was configured.
if ($env:VCPKG_DEFAULT_BINARY_CACHE) {
    New-Item -ItemType Directory -Force -Path $env:VCPKG_DEFAULT_BINARY_CACHE | Out-Null
}

Push-Location $vcpkg_root
try {
    & .\bootstrap-vcpkg.bat
    if ($LASTEXITCODE -ne 0) {
        throw "bootstrap-vcpkg.bat failed with exit code $LASTEXITCODE."
    }

    & .\vcpkg install @packages
    if ($LASTEXITCODE -ne 0) {
        throw "vcpkg install failed with exit code $LASTEXITCODE."
    }
}
finally {
    Pop-Location
}
