@echo off

mkdir "build"

rem del "game.exe"

set OPTIMIZATION_LEVEL=-O0
set DEFINES=-D DEBUG -D DEV_CHEATS

set FLAGS=-ftime-trace -Wno-deprecated-declarations -Wno-braced-scalar-init -Wno-c++11-narrowing -Wno-writable-strings -ferror-limit=0 -g -gcodeview -MT -msse -msse2 -msse3

set OPTIONS=%FLAGS% %OPTIMIZATION_LEVEL% %DEFINES%

set INCLUDES=-I "lib/"
set SOURCES="code/win32_vk_main.cpp"
set LIBS= -lUser32.lib -lGdi32.lib -lComdlg32.lib -L"C:/VulkanSDK/1.3.280.0/Lib" -lvulkan-1.lib -lOle32.lib
set OUTPUT="build/game.exe"
rem set OUTPUT=%OUTPUT%.s -c -S

echo.
echo compiling shaders...

REM TODO: probably can move this to a list somewhere
glslc -std=450core -fshader-stage=vert -DVERT "shaders/circle.glsl" -o "data/shaders/circle_vert.spv"
glslc -std=450core -fshader-stage=frag -DFRAG "shaders/circle.glsl" -o "data/shaders/circle_frag.spv"
glslc -std=450core -fshader-stage=vert -DVERT "shaders/rectangle.glsl" -o "data/shaders/rectangle_vert.spv"
glslc -std=450core -fshader-stage=frag -DFRAG "shaders/rectangle.glsl" -o "data/shaders/rectangle_frag.spv"
glslc -std=450core -fshader-stage=vert -DVERT "shaders/sphere.glsl" -o "data/shaders/sphere_vert.spv"
glslc -std=450core -fshader-stage=frag -DFRAG "shaders/sphere.glsl" -o "data/shaders/sphere_frag.spv"
glslc -std=450core -fshader-stage=vert -DVERT "shaders/world.glsl" -o "data/shaders/world_vert.spv"
glslc -std=450core -g -fshader-stage=frag -DFRAG "shaders/world.glsl" -o "data/shaders/world_frag.spv"
glslc -std=450core -fshader-stage=vert -DVERT "shaders/lightprobe_visualization.glsl" -o "data/shaders/lightprobe_visualization_vert.spv"
glslc -std=450core -fshader-stage=frag -DFRAG "shaders/lightprobe_visualization.glsl" -o "data/shaders/lightprobe_visualization_frag.spv"
glslc -std=450core -fshader-stage=vert -DVERT "shaders/aabb_visualization.glsl" -o "data/shaders/aabb_visualization_vert.spv"
glslc -std=450core -fshader-stage=frag -DFRAG "shaders/aabb_visualization.glsl" -o "data/shaders/aabb_visualization_frag.spv"
glslc -std=450core -fshader-stage=vert -DVERT "shaders/text.glsl" -o "data/shaders/text_vert.spv"
glslc -std=450core -fshader-stage=frag -DFRAG "shaders/text.glsl" -o "data/shaders/text_frag.spv"
glslc -std=450core -fshader-stage=vert -DVERT "shaders/fullscreen_image.glsl" -o "data/shaders/fullscreen_image_vert.spv"
glslc -std=450core -fshader-stage=frag -DFRAG "shaders/fullscreen_image.glsl" -o "data/shaders/fullscreen_image_frag.spv"
glslc -std=450core -fshader-stage=vert -DVERT "shaders/draw_replay_frame.glsl" -o "data/shaders/draw_replay_frame_vert.spv"
glslc -std=450core -fshader-stage=frag -DFRAG "shaders/draw_replay_frame.glsl" -o "data/shaders/draw_replay_frame_frag.spv"
glslc -g -std=450core -fshader-stage=comp "shaders/lightmap_cast.glsl" -o "data/shaders/lightmap_cast.spv"
glslc -g -std=450core -fshader-stage=comp "shaders/lightmap_accumulate_color.glsl" -o "data/shaders/lightmap_accumulate_color.spv"
glslc -g -std=450core -fshader-stage=comp "shaders/lightmap_accumulate_depth.glsl" -o "data/shaders/lightmap_accumulate_depth.spv"

REM create and delete a file to update the filetime of data/shaders/
echo.2>"data/shaders/temp.txt"
del "data\shaders\temp.txt"
popd
echo done compiling shaders

echo.

2>nul (
>>"build/game.exe" echo off
) && (
    echo compiling...
    clang %OPTIONS% -o %OUTPUT% %INCLUDES% %SOURCES% %LIBS%
    rem gcc %OPTIONS% -o %OUTPUT% %INCLUDES% %SOURCES% %LIBS%
    echo done compiling
) || (
    echo exe is locked, skipping recompilation
    EXIT /B 0
)

if not errorlevel 1 (
    echo running...
    echo.
    RunDetached "./build/game.exe"
) else (
    EXIT /B 1
)
