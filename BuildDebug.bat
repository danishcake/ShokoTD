@echo Building Debug
path=%PATH%;%PROGRAMFILES%\Microsoft Visual Studio 8\Common7\IDE
devenv.com ShokoTD.sln /build Debug
pause