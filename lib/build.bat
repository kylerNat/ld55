@echo off

REM mkdir "build"

rem del "tests/build/test1.exe"
clang -Wno-c++11-narrowing -Wno-writable-strings -ffast-math -O0 -g -gcodeview -o "tests/build/test1.exe" -I "./" -D DEBUG "tests/test1.cpp" -lUser32.lib -lGdi32.lib -lopengl32.lib

rem em++ -Wno-c++11-narrowing -Wno-writable-strings -s ASYNCIFY=1 -s ASYNCIFY_STACK_SIZE=1048576 -ffast-math --source-map-base -O0 -g4 -o "tests/build/web_test1.html" -I "./" -D DEBUG "tests/web_test1.cpp"
rem emcc -Wno-c++11-narrowing -Wno-writable-strings -s ASYNCIFY=1 -s ASYNCIFY_STACK_SIZE=1048576 -ffast-math -O3 -o "tests/build/web_test1.html" -I "./" -D DEBUG "tests/web_test1.cpp"

if not errorlevel 1 (
   echo running...
   "tests/build/test1.exe"
   rem "tests/build/webgen_test.exe"
) else (
   EXIT /B 1
)
popd
