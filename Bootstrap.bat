@echo off
if not exist "Build" mkdir "Build"
git checkout -- Bootstrap.ninja CGen\Bootstrap.ninja Fe\Bootstrap.ninja Makey\Bootstrap.ninja
External\ninja\ninja.exe -C Build -f ..\Bootstrap.ninja Makey
Bin\Win_x64_Release\Makey\Makey.exe platform=Win_x64 config=Release compiler=MSVC