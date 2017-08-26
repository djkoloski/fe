@echo off
git checkout -- Build\Win_x64_Release\Bootstrap.ninja Build\Win_x64_Release\FeGen\Bootstrap.ninja Build\Win_x64_Release\Fe\Bootstrap.ninja Build\Win_x64_Release\FeMake\Bootstrap.ninja
External\ninja\ninja.exe -C Build\Win_x64_Release -f Bootstrap.ninja FeMake_codegen
External\ninja\ninja.exe -C Build\Win_x64_Release -f Bootstrap.ninja FeMake
Bin\Win_x64_Release\FeMake\FeMake.exe platform=Win_x64 config=Release compiler=MSVC