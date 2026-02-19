cmake --build build
xcopy /i /e /y /q "F:\Projects\Bane\include\bane\*.*" "F:\Projects\BaneEditor\include\bane\"
xcopy /i /e /y /q "F:\Projects\Bane\include\bane\*.*" "F:\Projects\BrackeysDroneGame\game\include\bane\"
xcopy /i /e /y /q "F:\Projects\Bane\build\bane.lib"   "F:\Projects\BaneEditor\lib"
xcopy /i /e /y /q "F:\Projects\Bane\build\bane.lib"   "F:\Projects\BrackeysDroneGame\game\lib"
xcopy /i /e /y /q "F:\Projects\Bane\build\bane.lib"   "F:\Projects\BrackeysDroneGame\exec\lib"
