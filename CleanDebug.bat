@echo Cleaning up Debug
path=%PATH%;%PROGRAMFILES%\Microsoft Visual Studio 8\Common7\IDE
devenv.com ShokoTD.sln /clean Debug
@echo Removing incremental linker stuff
del *.ilk /s
@echo Removing debug database
del *.pdb