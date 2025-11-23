cd build
ninja
cd ..
xcopy /i /e /y /q "F:\Programming\OpenGL\BaneEngine\include\bane\*.*" "F:\GameDev\BaneProjects\BaneEditor\include\bane\"
xcopy /i /e /y /q "F:\Programming\OpenGL\BaneEngine\include\bane\*.*" "F:\GameDev\BaneProjects\template\game\include\bane\"
xcopy /i /e /y /q "F:\Programming\OpenGL\BaneEngine\include\bane\*.*" "F:\GameDev\BaneProjects\template\exec\include\bane\"
xcopy /i /e /y /q "F:\Programming\OpenGL\BaneEngine\build\bane.lib" "F:\GameDev\BaneProjects\BaneEditor\lib"
xcopy /i /e /y /q "F:\Programming\OpenGL\BaneEngine\build\bane.lib" "F:\GameDev\BaneProjects\template\game\lib"
xcopy /i /e /y /q "F:\Programming\OpenGL\BaneEngine\build\bane.lib" "F:\GameDev\BaneProjects\template\exec\lib"
