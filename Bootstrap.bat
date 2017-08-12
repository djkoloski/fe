@echo off
if not exist "Build" mkdir "Build"
git checkout -- Bootstrap.ninja FeGen\Bootstrap.ninja Fe\Bootstrap.ninja FeMake\Bootstrap.ninja
External\ninja\ninja.exe -C Build -f ..\Bootstrap.ninja FeMake
Bin\Win_x64_Release\FeMake\FeMake.exe platform=Win_x64 config=Release compiler=MSVC