set PYTHON_DIR=C:\app\Python38

set uic=%PYTHON_DIR%\Scripts\pyside2-uic.exe

%uic% main_window.ui -o ..\qtui\ui_main_window.py

pause
