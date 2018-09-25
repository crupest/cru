function CheckLastExitCode {
    if ($LASTEXITCODE -ne 0)
    {
        exit $LASTEXITCODE
    }
}

vcpkg.exe install folly:x64-windows fmt:x64-windows
& CheckLastExitCode

& "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" CruUI.sln /target:Rebuild /p:Platform=x64 /p:Configuration=Debug
& CheckLastExitCode
