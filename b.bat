cd build
ninja -v
cd ..
xcopy /i /e /y "F:\Programming\OpenGL\BaneEngine\include\bane\*.*" "F:\GameDev\BaneProjects\TestProject1\include\bane\"
xcopy /i /e /y "F:\Programming\OpenGL\BaneEngine\build\bane.lib" "F:\GameDev\BaneProjects\TestProject1\lib"
