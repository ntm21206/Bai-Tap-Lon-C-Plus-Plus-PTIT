@echo off
echo ===================================
echo BUILD IMGUI GPA MANAGER
echo ===================================
echo.

echo [1/3] Compiling...

g++ -o GPAManager_GUI.exe main_imgui.cpp ^
    imgui/imgui.cpp ^
    imgui/imgui_draw.cpp ^
    imgui/imgui_tables.cpp ^
    imgui/imgui_widgets.cpp ^
    imgui/imgui_demo.cpp ^
    imgui/backends/imgui_impl_glfw.cpp ^
    imgui/backends/imgui_impl_opengl3.cpp ^
    src/glad.c ^
    -Iimgui ^
    -Iimgui/backends ^
    -Iglad ^
    -IKHR ^
    -IGLFW ^
    -Iinclude ^
    -LGLFW ^
    -lglfw3 -lopengl32 -lgdi32 -limm32 ^
    -std=c++17 ^
    -static-libgcc -static-libstdc++

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [2/3] Compile SUCCESS!
    echo [3/3] Running...
    echo.
    GPAManager_GUI.exe
) else (
    echo.
    echo [X] Compile FAILED!
    echo Please check errors above.
    pause
)