#!/bin/bash

# Check Python version
PYTHON_VERSION=$(python -c 'import sys; print(sys.version_info[0])')

if [ $PYTHON_VERSION -eq 3 ]; then
    echo "Using Python 3"
    PYTHON_EXEC=python
else
    echo "Using Python 2, consider upgrading to Python 3"
    PYTHON_EXEC=python3
fi

# Install Matplotlib and Pygame
$PYTHON_EXEC -m pip install matplotlib pygame

# Launch the GUI script
$PYTHON_EXEC Code/GUI.py

