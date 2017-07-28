if not exist "Build" mkdir "Build"
External\ninja\ninja.exe -C Build -f ..\Bootstrap.ninja Makey
Bin\Win_x64_Release\Makey\Makey.exe platform=Win_x64 config=Release compiler=MSVC