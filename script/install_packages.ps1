function CheckLastExitCode
{
    if ($LASTEXITCODE -ne 0)
    {
        exit $LASTEXITCODE
    }
}

git clone https://github.com/Microsoft/vcpkg.git --depth=1
& CheckLastExitCode

cd vcpkg

& .\bootstrap-vcpkg.bat
& CheckLastExitCode

.\vcpkg.exe install folly:x64-windows fmt:x64-windows
& CheckLastExitCode

.\vcpkg.exe integrate install
& CheckLastExitCode
