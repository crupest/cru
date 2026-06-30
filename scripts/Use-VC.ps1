function Use-VC {
    param(
        [Parameter()]
        [ValidateSet('x64', 'x86')]
        $Arch = 'x64',

        [Parameter()]
        [string]
        $DotEnvPath = $null,

        [Parameter()]
        [string]
        $LldbEnvPath = $null
    )

    if ($Arch -eq 'x86') {
        $p = 'x86';
    }
    else {
        $p = 'amd64'
    }

    function write-file {
        param(
            [Parameter()]
            [string]$Path,

            [Parameter()]
            [string]$Content,

            [Parameter()]
            [string]$Message
        )

        if ($Path) {
            $parent = Split-Path -Parent $Path
            if ($parent) {
                New-Item -ItemType Directory -Path $parent -Force | Out-Null
            }
            $Content | Out-File -FilePath $Path -Encoding utf8
            Write-Host $Message -ForegroundColor Yellow
        }
    }

    function get-escape-for-lldb {
        param(
            [Parameter()]
            [string]$value
        )
        # Escape backslashes and double quotes
        $escaped = $value -replace '\\', '\\' -replace '"', '\"'
        return $escaped
    }


    $dot_env_content = ""
    $lldb_env_content = ""
    cmd /c "`"$(vswhere.exe -format value -property installationPath)\VC\Auxiliary\Build\vcvars64.bat`" $p & set" |
    ForEach-Object {
        if ($_ -match '=') {
            $v = $_ -split '=', 2
            $key = $v[0]
            $value = $v[1]
            Set-Item -Force -Path "ENV:\$key" -Value "$value"
            if ($key -eq 'PATH') {
                # We currently only export PATH to env file, since it's the only variable needed for ASAN to work.
                # Some other variables may have special characters in keys, which breaks the extension's functionality.
                $dot_env_content += "$_`n"
                $escaped_key = get-escape-for-lldb -value $key
                $escaped_value = get-escape-for-lldb -value $value
                $lldb_env_content += "settings set target.env-vars `"$escaped_key`"=`"$escaped_value`"`n"
            }
        }
    }

    Write-Host "Visual Studio Command Prompt variables set." -ForegroundColor Yellow
    write-file -Path $DotEnvPath -Content $dot_env_content -Message "Environment variable dot env file written to $DotEnvPath"
    write-file -Path $LldbEnvPath -Content $lldb_env_content -Message "Environment variable LLDB env file written to $LldbEnvPath"
}
