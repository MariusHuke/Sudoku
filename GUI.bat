@echo off

:: Check Python version
for /f "tokens=1,2,*" %%V in ('python -c "import sys; echo !sys.version_info!"') do set PYTHON_VERSION=%%W

if %PYTHON_VERSION% == 3 (
    echo Using Python 3
    set PYTHON_EXEC=python
) else (
    echo Using Python 2, consider upgrading to Python 3
    set PYTHON_EXEC=python3
)

:: Install Matplotlib and Pygame
%PYTHON_EXEC% -m pip install matplotlib pygame

:: Launch the GUI script
%PYTHON_EXEC% Code/GUI.py
