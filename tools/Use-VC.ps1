function Use-VC {
    param(
        [Parameter()]
        [ValidateSet('x64', 'x86')]
        $Arch = 'x64'
    )

    if ($Arch -eq 'x86') {
        $p = 'x86';
    }
    else {
        $p = 'amd64'
    }

    $vs = $(vswhere.exe -format value -property installationPath)

    cmd /c "$vs\VC\Auxiliary\Build\vcvars64.bat $p & set" |
    ForEach-Object {
        if ($_ -match '=') {
            $v = $_ -split '='
            Set-Item -Force -Path "ENV:\$($v[0])" -Value "$($v[1])"
        }
    }
    Pop-Location
    Write-Host "Visual Studio 2019 Command Prompt variables set." -ForegroundColor Yellow
}
