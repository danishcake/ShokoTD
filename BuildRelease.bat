@echo Building Release
path=%PATH%;%PROGRAMFILES%\Microsoft Visual Studio 8\Common7\IDE
devenv.com ShokoRocket++.sln /build Release
pause