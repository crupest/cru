cd ..\CruUI
Remove-Item build -Recurse -ErrorAction Ignore
New-Item build -ItemType Directory
cd .\build
cmake -G "Visual Studio 15 2017 Win64" ..
cmake --build .
